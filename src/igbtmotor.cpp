#include <Arduino.h>
#include "soc/gpio_struct.h"
#include "soc/gpio_reg.h"
#include "driver/gpio.h"
#include "config.h"
#include "web_server.h"

cfgOptions cfg;
const bool CFG_FULL_CUT_DEFAULT = true;

// Timing and measurement variables
volatile unsigned long currTime          = 0;
volatile unsigned long speedPulses       = 0;
volatile unsigned long dwellBeginTime[4] = {0};
volatile unsigned long lastDwellTime[4]  = {0};
volatile unsigned long lastRPMdelta[4]   = {0};
volatile bool lastMeasureState[4]        = {0};
volatile bool shiftingTrig               = false;
volatile bool limitingRPM                = false;
volatile int currRetard                  = 0;
volatile int currRestore                 = 0;

unsigned long lastCycle      = 0;
unsigned long lastRead       = 0;
unsigned long lastCut        = 0;
unsigned long lastSpeedRead  = 0;

int currHoldTime             = 0;
volatile int lastRPM         = 0;
int lastWheelRPM             = 0;
float lastWheelSpeed         = 0;
int pressureValue            = 0;
int limiterState             = OFF;
bool buttonPressed           = false;
bool lastButtonState         = false;
bool waitHyst                = true;

// Hardware timer instances
hw_timer_t *t_cut[4] = {NULL, NULL, NULL, NULL};

// Pin definitions
const int HALL_PINS[2]    = {10, 11};
const int IGBT_PINS[4]    = {2, 3, 4, 1};
const int MEASURE_PINS[4] = {9, 14, 7, 8};
const int WHEEL_PIN       = 15;
const int PIEZO_PIN       = 13;
const int GREEN_PIN       = 5;
const int RED_PIN         = 6;

// Timer callbacks to release IGBT pins
void IRAM_ATTR on_t0_cut() { GPIO.out_w1ts = (1 << IGBT_PINS[0]); }
void IRAM_ATTR on_t1_cut() { GPIO.out_w1ts = (1 << IGBT_PINS[1]); }
void IRAM_ATTR on_t2_cut() { GPIO.out_w1ts = (1 << IGBT_PINS[2]); }
void IRAM_ATTR on_t3_cut() { GPIO.out_w1ts = (1 << IGBT_PINS[3]); }

// Coil dwell measurement & ignition retard/cut interrupt
void coilInterrupt(int ch)
{
  bool bPin = digitalRead(MEASURE_PINS[ch]);
  unsigned long lTime = micros();

  if (bPin && !lastMeasureState[ch])
  {
    lastRPMdelta[ch] = lTime - dwellBeginTime[ch];
    unsigned long delayForRetard = ((unsigned long)currRetard * (lTime - dwellBeginTime[ch])) / cfg.wastedSpark;

    if (!shiftingTrig && currRetard > 0) {
      currRetard = max(0, currRetard - currRestore);
    }

    dwellBeginTime[ch] = lTime;

    if (currRetard > 0)
    {
      if (shiftingTrig && cfg.fullCut)
      {
        delayForRetard += ((currHoldTime * 1000 / lastRPMdelta[ch]) + 1) * lastRPMdelta[ch];
        shiftingTrig = false;
      }

      GPIO.out_w1tc = (1 << IGBT_PINS[ch]);
      timerWrite(t_cut[ch], 0);
      timerAlarmWrite(t_cut[ch], lastDwellTime[ch] + delayForRetard, false);
      timerAlarmEnable(t_cut[ch]);
    }
  }
  else if (!bPin && !shiftingTrig && currRetard == 0 && dwellBeginTime[ch])
  {
    lastDwellTime[ch] = lTime - dwellBeginTime[ch];
  }

  lastMeasureState[ch] = bPin;
}

// Trampolines for coil interrupts
void IRAM_ATTR onPC0() { coilInterrupt(0); }
void IRAM_ATTR onPC1() { coilInterrupt(1); }
void IRAM_ATTR onPC2() { coilInterrupt(2); }
void IRAM_ATTR onPC3() { coilInterrupt(3); }

// Wheel speed pulse counter
void IRAM_ATTR onWheelSpeed()
{
  speedPulses++;
}

void setup()
{
  Serial.begin(115200);

  // Status LEDs (active low)
  pinMode(GREEN_PIN, OUTPUT);
  GPIO.out_w1ts = (1 << GREEN_PIN);
  pinMode(RED_PIN, OUTPUT);
  GPIO.out_w1ts = (1 << RED_PIN);

  // Sensor inputs
  int adcPins[4] = {HALL_PINS[0], HALL_PINS[1], PIEZO_PIN, WHEEL_PIN};
  for (int i = 0; i < 4; i++) {
    pinMode(adcPins[i], INPUT);
  }

  // IGBT outputs and dwell measurement inputs
  for (int i = 0; i < 4; i++) {
    pinMode(IGBT_PINS[i], OUTPUT);
    GPIO.out_w1ts = (1 << IGBT_PINS[i]);
    pinMode(MEASURE_PINS[i], INPUT);
  }

  // Hardware timers (1 MHz tick)
  void (*on_cut_funcs[4])() = {on_t0_cut, on_t1_cut, on_t2_cut, on_t3_cut};
  for (int i = 0; i < 4; i++) {
    t_cut[i] = timerBegin(i, 80, true);
    timerAttachInterrupt(t_cut[i], on_cut_funcs[i], true);
    timerAlarmDisable(t_cut[i]);
  }

  // Coil measurement interrupts
  void (*pcFuncs[4])() = {onPC0, onPC1, onPC2, onPC3};
  for (int i = 0; i < 4; i++) {
    attachInterrupt(digitalPinToInterrupt(MEASURE_PINS[i]), pcFuncs[i], CHANGE);
  }

  // Optional rear wheel speed interrupt
  attachInterrupt(digitalPinToInterrupt(WHEEL_PIN), onWheelSpeed, RISING);

  // Initialize modular web portal & access point
  webServerInit();
}

void loop()
{
  currTime = micros();

  // 1 kHz main control cycle
  if ((currTime - lastCycle) >= 1000)
  {
    // ADC sampling (20 Hz)
    if ((currTime - lastRead) >= 50000)
    {
      const int inputs[] = {PIEZO_PIN, HALL_PINS[0], HALL_PINS[1], PIEZO_PIN};

      pressureValue = analogRead(inputs[cfg.pressureInput]);
      if (cfg.pressureInput == 0)
        pressureValue = 4095 - pressureValue;

      buttonPressed = cfg.buttonInput ? !digitalRead(inputs[cfg.buttonInput]) : false;

      if (buttonPressed && !lastButtonState)
      {
        switch (limiterState)
        {
          case PIT:
          case LAUNCH:
            limiterState = OFF;
            break;
          case OFF:
            if (lastWheelSpeed < cfg.limiterMaxSpeed && cfg.wheelSensor)
              limiterState = LAUNCH;
            else
              limiterState = PIT;
            break;
        }
      }

      if (cfg.limiterAlways)
        limiterState = PIT;

      lastButtonState = buttonPressed;

      if (lastRPMdelta[0] > 0)
      {
        lastRPM = ((cfg.wastedSpark > 700) ? 120000000UL : 60000000UL) / lastRPMdelta[0];
      }

      lastRead = currTime;
    }

    // Wheel speed calculation (2 Hz)
    if (cfg.wheelSensor && (currTime - lastSpeedRead) >= 500000)
    {
      lastWheelRPM = (int)(speedPulses * (120 / (float)cfg.sensorPulses));
      speedPulses = 0;
      lastWheelSpeed = (lastWheelRPM * cfg.speedScale) / 60000.f;
      lastSpeedRead = currTime;
    }

    // Pressure hysteresis check
    if (pressureValue < (cfg.cutSens - cfg.cutHyst))
      waitHyst = false;

    // 2-Step Launch Control / Pit Limiter logic
    if ((cfg.launchRPM || cfg.limiterRPM) && cfg.limiterCut && cfg.limiterRetard)
    {
      switch (limiterState)
      {
        case LAUNCH:
          if (lastWheelSpeed > cfg.limiterMaxSpeed)
            limiterState = OFF;

          if (cfg.launchRPM && lastRPM > cfg.launchRPM && !shiftingTrig)
          {
            shiftingTrig = true;
            limitingRPM = true;
            waitHyst = true;
            currRetard = cfg.limiterRetard;
            currHoldTime = min((int)((lastRPM - cfg.launchRPM) * cfg.limiterCut / 100.f), 200);
            currRestore = (int)(cfg.limiterRetard / cfg.limiterDiv);
            cfg.fullCut = cfg.limiterFullCut;
            lastCut = currTime;
          }
          break;

        case PIT:
          if (cfg.limiterRPM && lastRPM > cfg.limiterRPM && !shiftingTrig)
          {
            shiftingTrig = true;
            limitingRPM = true;
            waitHyst = true;
            currRetard = cfg.limiterRetard;
            currHoldTime = min((int)((lastRPM - cfg.limiterRPM) * cfg.limiterCut / 100.f), 200);
            currRestore = (int)(cfg.limiterRetard / cfg.limiterDiv);
            cfg.fullCut = cfg.limiterFullCut;
            lastCut = currTime;
          }
          break;

        case OFF:
          if (limitingRPM)
          {
            limitingRPM = false;
            shiftingTrig = false;
            cfg.fullCut = CFG_FULL_CUT_DEFAULT;
          }
          break;
      }
    }

    if (lastRPM < cfg.minRPM)
      waitHyst = true;

    // Quickshifter trigger condition
    if (pressureValue > cfg.cutSens && lastRPM >= cfg.minRPM && !waitHyst && !shiftingTrig && !limitingRPM && (currTime - lastCut) >= (unsigned long)cfg.deadTime * 1000)
    {
      shiftingTrig = true;
      currRetard = map(lastRPM, cfg.minRPM, cfg.maxRPM, cfg.retardLow, cfg.retardHigh);
      currHoldTime = map(lastRPM, cfg.minRPM, cfg.maxRPM, cfg.holdTimeLow, cfg.holdTimeHigh);
      currRestore = (int)(currRetard / cfg.restore) + 1;
      waitHyst = true;

      GPIO.out_w1tc = (1 << GREEN_PIN);
      lastCut = currTime;
    }

    // End of shift cut window
    if (shiftingTrig && (currTime - lastCut) >= (unsigned long)currHoldTime * 1000)
    {
      shiftingTrig = false;
      GPIO.out_w1ts = (1 << GREEN_PIN);
    }

    lastCycle = currTime;
  }

  webServerHandle();
}
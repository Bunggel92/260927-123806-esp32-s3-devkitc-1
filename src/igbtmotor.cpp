/* ============================================================================
   Quickshifter / Ignition-Cut Controller — ESP32
   ----------------------------------------------------------------------------
   This firmware kills or retards ignition on up to 4 coils for a short,
   configurable window whenever a "shift" is detected (via a pressure/strain
   sensor on the shift linkage), so the rider can shift gear under load
   without closing the throttle ("clutchless" quickshifting).

   It also implements a simple 2-step RPM limiter (PIT / LAUNCH modes) using
   the same ignition-retard/cut mechanism, plus optional rear wheel speed
   sensing (used to gate launch control by road speed).

   NOTE (this revision): the web configuration portal (WiFi AP + HTTP config
   page + WebSocket live-debug page) has been removed. All tuning values now
   live in the `cfg` struct below as compile-time defaults — edit them
   directly in this file and reflash to change settings ("bare firmware"
   configuration). A different configuration method can be layered back in
   later without touching the ignition logic itself.
   ============================================================================ */

/* TODO:
  - Maybe add some kind of detection for cylinder count (of course only while engine is running) so cfg.restore gets scaled accordingly (makes config more generally usable)
  - Rewrite so each Channel doesn't need its own timer
    -> meh, semi fixed by removing the need for a seperate timekeeping timer so all 4 timers can be used for qs channels
  - Implement volatile unsigned long lowStartTime[4] -> Prevent coils from overheating because of extended dwell time > ~200ms
  - Think how cfg.restore is affected by cylinder/coil count with wasted spark (double the cylinder count needs double cfg.restore for same smoothness, change to float for more precision or unnecessary?)
  - Rename cfg.wastedSpark to something that describes it better (crank degrees per ignition pulse per coil) (Change back to boolean, create global variable that stores 360 or 720 for coilInterrupt function but keeps this simple)
  - Try to improve performance of coilInterrupt interrupt function (is micros() bad? less calculations)
  - Make currRetard calculation logarithmic instead of linear (more change at low rpm, less change at high rpm, with configurable breakpoint percentage, maybe see throttle expo in betaflight)
*/

#include <Arduino.h>

#include "soc/gpio_struct.h"  // Direct GPIO register access (GPIO.out_w1ts/w1tc) - much faster than digitalWrite() inside ISRs/timer callbacks
#include "soc/gpio_reg.h"
#include "driver/gpio.h"

// ============================================================================
// USER-EDITABLE CONFIGURATION
// ----------------------------------------------------------------------------
// Every tunable that used to live on the web config page is now a member of
// this struct, with its old web-form default as the initial value and its
// old web-form min/max range noted in the comment. Edit the numbers below,
// then re-flash the ESP32 to apply new settings — there is no runtime UI.
// ============================================================================
struct cfgOptions
{
  // --- Quickshifter ignition retard (triggered by pressure/strain sensor) ---
  int retardLow      = 40;    // deg   [0-180]     Ignition retard applied when shifting at minRPM
  int retardHigh     = 60;    // deg   [0-180]     Ignition retard applied when shifting at maxRPM
                               //                   (retard is linearly interpolated between these two by RPM)
  int restore        = 20;    // pulses[0-1000]    How many ignition pulses it takes to smoothly ramp
                               //                   currRetard back down to 0 after a shift (higher = smoother/slower)

  int minRPM         = 2900;  // 1/min [1000-20000] Below this RPM, quickshift triggering is disabled entirely
  int maxRPM         = 12500; // 1/min [2000-20000] Upper end of the retard/hold-time interpolation range

  int holdTimeLow    = 49;    // ms    [1-1000]    Ignition cut/retard duration when shifting at minRPM
  int holdTimeHigh   = 72;    // ms    [1-1000]    Ignition cut/retard duration when shifting at maxRPM

  int deadTime       = 300;   // ms    [1-5000]    Minimum time that must pass between two shift events
  int cutSens        = 1000;  // ADC   [1-4095]    Pressure/force reading that arms/triggers a shift
  int cutHyst        = 500;   // ADC   [1-4095]    Hysteresis: pressure must drop below (cutSens - cutHyst)
                               //                   before the next shift can be armed

  bool  fullCut      = true;  // [checkbox]        true  = fully cut ignition during a shift
                               //                   false = only retard ignition (softer, no full miss)
  int wastedSpark  = 360; // [checkbox: 360/720] Ignition system type:
                               //                     360 = wasted spark  (1 ignition pulse per crank revolution)
                               //                     720 = sequential    (1 ignition pulse per 2 crank revolutions)
                               //   (original web option was a checkbox "Wasted Spark Setup": checked -> 360, unchecked -> 720)

  // --- 2-step / launch control RPM limiter ---
  bool limiterAlways    = false;  // [checkbox]     true = limiter is always armed in PIT mode (no button press needed)
  bool limiterFullCut   = false; // [checkbox]     Use full ignition cut (instead of retard) while the limiter is active
  int  limiterRPM       = 3600;  // 1/min [0-20000] RPM ceiling while in PIT mode
  int  launchRPM        = 2500;  // 1/min [0-20000] RPM ceiling while in LAUNCH mode (holding revs at the line)
  int  limiterCut       = 10;    // ms/100rpm [1-1000] Gain: how much extra hold-time per 100 rpm over the limiter target
  int  limiterRetard    = 30;    // deg   [0-180]  Ignition retard applied while the limiter is actively cutting
  int  limiterDiv       = 1;     // [1-20]         limiterRetard / limiterDiv = per-pulse restore rate while limiting
  int  limiterMaxSpeed  = 15;    // km/h  [0-1000] Below this road speed, LAUNCH mode is allowed / stays armed

  // --- Misc / sensor routing ---
  int  pressureInput   = 0;     // 0=Piezo, 1=ADC1 (Hall 1), 2=ADC2 (Hall 2) — which input carries the shift-force signal
  int  buttonInput     = 0;     // 0=None,  1=ADC1, 2=ADC2                  — which input carries the handlebar mode button
  bool wheelSensor     = false; // Enable rear wheel speed sensing (needed for LAUNCH mode's speed gate)
  int  speedScale      = 1528;  // mm [1-20000] Effective wheel circumference incl. sprocket ratio, used for km/h calc
                                 //   e.g. Grom: 15/34 sprocket ratio * 1529mm wheel dia = 675mm travel per rear wheel rev
  int  sensorPulses    = 70;    // [1-1000] Pulses produced by the wheel speed sensor per one rear wheel revolution
} cfg;

// The limiter temporarily overwrites cfg.fullCut with cfg.limiterFullCut while
// active, then needs to restore the *original* user-configured value once the
// limiter releases. Previously this came from re-reading the web config
// ("conf.getBool("fullCut")"); now it's just a snapshot of the default above.
const bool CFG_FULL_CUT_DEFAULT = true; // keep in sync with cfg.fullCut's initializer above

// Global variables
volatile unsigned long currTime          = 0;     // current timestamp in 1 µs steps (1 MHz)
volatile unsigned long speedPulses       = 0;     // Pulse count from the wheel speed sensor in the last interval (500 ms / 2 Hz)
volatile unsigned long dwellBeginTime[4] = {0};   // time of last dwell start in µs steps
volatile unsigned long lastDwellTime[4]  = {0};   // duration of last dwell pulse in µs steps
volatile unsigned long lastRPMdelta[4]   = {0};   // Stores time bewteen two ignition pulses, needed for RPM calculation
volatile bool lastMeasureState[4]        = {0};   // last state of the filtered coil state input
volatile bool shiftingTrig               = false; // Controls whether ignition is retarded
volatile bool limitingRPM                = false; // Temporarily changes restore time to 999 while RPM limiting
volatile int currRetard                  = 0;     // Used for gradual ignition retard recovery to normal operation
volatile int currRestore                 = 0;     // Calculated deg/ignition pulse recovery speed

unsigned long lastCycle           = 0;     // When was the last main loop cycle (1 kHz)
unsigned long lastRead            = 0;     // When was the last ADC sensor reading (20 Hz)
unsigned long lastCut             = 0;     // When was the last ignition cut/retard begin
unsigned long lastSpeedRead       = 0;     // When was the last rear wheel speed measurement (2 Hz)

int currHoldTime                  = 0;     // Interpolated hold time based on current RPM and low/high time
int lastRPM                       = 0;
int lastWheelRPM                  = 0;     // Measured rear wheel speed (1/min)
float lastWheelSpeed              = 0;     // Calculated rear wheel speed (km/h)
int pressureValue                 = 0;     // Piezo/Hall sensor pressure value
int limiterState                  = 0;     // Current state of RPM limiter mode
bool buttonPressed                = false; // Handlebar button state
bool lastButtonState              = false; // Last button state for edge detection
bool waitHyst                     = true;  // Needs to be low before another upshift is allowed

// Timer instances
hw_timer_t *t_cut[4]  = {NULL, NULL, NULL, NULL};

// Pin definitions
const int HALL_PINS[2]    = {10, 11};
const int IGBT_PINS[4]    = {2, 3, 4, 1};
const int MEASURE_PINS[4] = {9, 14, 7, 8}; // Handwired first revision (xj6)
//const int MEASURE_PINS[4] = {9, 7, 8, 12}; // PCB Design Pins (grom, R3b)

const int WHEEL_PIN = 15; // 12 on my grom, 15 on my xj6, 14 in R3b PCB revision
const int PIEZO_PIN = 13;
const int GREEN_PIN = 5;
const int RED_PIN   = 6;

// 2-step limiter state machine. OFF = no limiting. PIT = pit-lane style RPM
// cap (e.g. while stationary). LAUNCH = holding a lower RPM ceiling at the
// start line, released once the bike accelerates past limiterMaxSpeed.
enum lim {
  OFF = 0,
  LAUNCH = 1,
  PIT = 2
};

// ============================================================================
// IGNITION CUT TIMER CALLBACKS
// ----------------------------------------------------------------------------
// Each coil channel has its own hardware timer (t_cut[i]). When a shift (or
// limiter event) begins, coilInterrupt() pulls the corresponding IGBT_PINS[i]
// LOW (closing the IGBT, i.e. cutting/retarding that cylinder's spark) and
// arms the matching timer for "dwell + retard delay" µs in the future. When
// the timer fires, one of these four callbacks runs and releases the IGBT
// (drives the pin HIGH again), letting that cylinder fire (retarded) or
// simply re-arming it for the next normal ignition pulse (full cut).
// GPIO.out_w1ts / out_w1tc are direct "write 1 to set/clear" register writes
// — equivalent to digitalWrite(pin, HIGH/LOW) but far faster, which matters
// because these run from a hardware timer ISR with tight timing requirements.
// ============================================================================
void IRAM_ATTR on_t0_cut()
{
  // digitalWrite(IGBT_PINS[0], HIGH);
  GPIO.out_w1ts = (1 << IGBT_PINS[0]);
}
void IRAM_ATTR on_t1_cut()
{
  // digitalWrite(IGBT_PINS[1], HIGH);
  GPIO.out_w1ts = (1 << IGBT_PINS[1]);
}
void IRAM_ATTR on_t2_cut()
{
  // digitalWrite(IGBT_PINS[2], HIGH);
  GPIO.out_w1ts = (1 <<IGBT_PINS[2]);
}
void IRAM_ATTR on_t3_cut()
{
  // digitalWrite(IGBT_PINS[3], HIGH);
  GPIO.out_w1ts = (1 << IGBT_PINS[3]);
}

// ============================================================================
// COIL / DWELL INTERRUPT — the heart of the ignition-retard mechanism
// ----------------------------------------------------------------------------
// Attached to MEASURE_PINS[ch] on CHANGE. MEASURE_PINS mirrors what the OEM
// ECU is doing to its ignition coil driver transistor: the ECU pulls this
// line low to begin charging ("dwelling") the coil, then releases it to fire
// the spark. This function watches that signal to:
//   1) time how long the ECU normally dwells the coil (lastDwellTime),
//   2) measure the time between consecutive ignition pulses (lastRPMdelta,
//      used for RPM calculation in loop()),
//   3) and, when a retard is active, delay the moment this firmware releases
//      the IGBT so the spark actually happens `delayForRetard` µs later than
//      the ECU intended — i.e. retarded ignition timing.
// ============================================================================
void coilInterrupt(int ch)
{
  bool bPin = digitalRead(MEASURE_PINS[ch]); // Measurement Pin
  unsigned long lTime = micros();

  // When ECU pulls coil to ground (rising edge on logic signal) — this is
  // the ECU *starting* to charge the coil for the next spark.
  if (bPin && !lastMeasureState[ch])
  {
    // Time since the previous dwell start = time between ignition pulses on this channel (used for RPM calc)
    lastRPMdelta[ch] = lTime - dwellBeginTime[ch];

    // How long (in µs) we need to hold this cylinder's IGBT closed on top of
    // its normal dwell time, to shift its spark later by currRetard degrees.
    // currRetard is in "degrees / cfg.wastedSpark of a full pulse cycle";
    // scaling by the measured pulse period converts that to microseconds.
    unsigned long delayForRetard = ((unsigned long)currRetard * (lTime - dwellBeginTime[ch])) / cfg.wastedSpark;

    // Gradual recovery: each new ignition pulse (as long as we're not mid-shift)
    // reduces currRetard by currRestore, so ignition timing eases back to
    // normal over several pulses instead of snapping back instantly.
    if (!shiftingTrig && currRetard > 0) {
      currRetard = max(0, currRetard - currRestore);
    }

    dwellBeginTime[ch] = lTime;

    // Either actively retarding ignition or still recovering from a previous one
    if (currRetard > 0)
    {
      // Full-cut mode: instead of just delaying the spark, skip firing this
      // pulse entirely for the configured hold time (currHoldTime), then let
      // the next pulse fire on time. shiftingTrig is cleared here so the
      // outer retard-recovery logic above can start ramping down afterwards.
      if (shiftingTrig && cfg.fullCut)
      {
        delayForRetard += ((currHoldTime*1000 / lastRPMdelta[ch]) + 1) * lastRPMdelta[ch];
        shiftingTrig = false;
      }

      // Close the IGBT now (cuts spark / begins the delayed dwell)
      // digitalWrite(IGBT_PINS[ch], LOW); // Close IGBT
      GPIO.out_w1tc = (1 << IGBT_PINS[ch]);
      // Arm this channel's hardware timer to reopen the IGBT after
      // (normal dwell time + retard delay) µs — that reopening is what
      // actually fires the spark, at the retarded moment.
      timerWrite(t_cut[ch], 0);
      timerAlarmWrite(t_cut[ch], lastDwellTime[ch] + delayForRetard, false);
      timerAlarmEnable(t_cut[ch]);
    }
    // else: no retard active — IGBT is left alone and the ECU drives ignition normally.
  }
  // When ECU releases coil (falling edge on logic signal) and we are NOT
  // currently retarding/cutting: just measure how long the ECU dwelled the
  // coil, so we know the "normal" dwell time to add the retard delay onto above.
  else if (!bPin && !shiftingTrig && currRetard == 0 && dwellBeginTime[ch])
  {
    lastDwellTime[ch] = lTime - dwellBeginTime[ch];
  }

  lastMeasureState[ch] = bPin;
}

// Trampolines: attachInterrupt() needs a plain void(*)() per pin, so each
// channel gets a tiny wrapper that calls the shared coilInterrupt() logic.
void IRAM_ATTR onPC0() { coilInterrupt(0); }
void IRAM_ATTR onPC1() { coilInterrupt(1); }
void IRAM_ATTR onPC2() { coilInterrupt(2); }
void IRAM_ATTR onPC3() { coilInterrupt(3); }

// Rear wheel speed sensor ISR — just counts pulses; loop() converts the
// count-per-interval into RPM/km-h every 500 ms (see "2 Hz" block below).
void IRAM_ATTR onWheelSpeed()
{
  speedPulses++;
}

// ============================================================================
// SETUP — pin/timer/interrupt initialization. No networking, no config
// portal: cfg is already fully populated at compile time from the struct
// initializer above.
// ============================================================================
void setup()
{
  Serial.begin(115000); // 115200 baud for debug output

  // Status LEDs (active-low: driven HIGH = off by default)
  pinMode(GREEN_PIN, OUTPUT);
  // digitalWrite(GREEN_PIN, HIGH); // Off default
  GPIO.out_w1ts = (1 << GREEN_PIN);
  pinMode(RED_PIN, OUTPUT);
  // digitalWrite(RED_PIN, HIGH); // Off default
  GPIO.out_w1ts = (1 << RED_PIN);

  // Hall sensors, Piezo sensor, wheel speed pin — all plain digital/analog inputs
  int adcPins[4] = {HALL_PINS[0], HALL_PINS[1], PIEZO_PIN, WHEEL_PIN};
  for (int i = 0; i < 4; i++) {
    pinMode(adcPins[i], INPUT);
  }

  // Ignition IGBT driver pins (output, default HIGH = IGBT open/not cutting)
  // and their matching coil dwell measurement pins (input), one pair per cylinder channel.
  for (int i = 0; i < 4; i++) {
    pinMode(IGBT_PINS[i], OUTPUT);
    // digitalWrite(IGBT_PINS[i], HIGH); // Open IGBT default
    GPIO.out_w1ts = (1 << IGBT_PINS[i]);

    pinMode(MEASURE_PINS[i], INPUT);
  }

  // One hardware timer per channel, used to time the delayed IGBT release (see on_tX_cut above)
  // On the newer ESP32 Arduino core, timerBegin() needs the hardware timer index,
  // divider, and count-up mode; a divider of 80 gives a 1 MHz tick from the 80 MHz APB clock.
  void (*on_cut_funcs[4])() = {on_t0_cut, on_t1_cut, on_t2_cut, on_t3_cut};
  for (int i = 0; i < 4; i++) {
    t_cut[i] = timerBegin(i, 80, true); // 1 MHz timer tick (1 µs resolution)
    timerAttachInterrupt(t_cut[i], on_cut_funcs[i], true);
    timerAlarmDisable(t_cut[i]);
  }

  // Attach interrupts to MEASURE_PINS[i] — this is what drives coilInterrupt() above
  void (*pcFuncs[4])() = {onPC0, onPC1, onPC2, onPC3};
  for (int i = 0; i < 4; i++) {
    attachInterrupt(digitalPinToInterrupt(MEASURE_PINS[i]), pcFuncs[i], CHANGE);
  }

  // Rear wheel speed sensing (optional, gated by cfg.wheelSensor at runtime in loop())
  attachInterrupt(digitalPinToInterrupt(WHEEL_PIN), onWheelSpeed, RISING);

  // NOTE: web config portal (WiFi AP, HTTP config page, WebSocket debug
  // stream) previously initialized here has been removed. cfg is already
  // populated by its struct initializer above — nothing left to do here.
}

// ============================================================================
// MAIN LOOP — runs as fast as possible, but internally gated to a 1 kHz
// cadence using micros() timestamps, with slower sub-tasks (20 Hz ADC read,
// 2 Hz wheel speed calc) nested inside that same 1 kHz tick.
// ============================================================================
void loop()
{
  static unsigned long lastDebugPrint = 0;
if (micros() - lastDebugPrint >= 1000000) { // Every 1 second
  Serial.printf("[ESP32 STATUS] RPM: %d | PressureVal: %d | waitHyst: %d | shiftingTrig: %d\n",
                lastRPM, pressureValue, waitHyst, shiftingTrig);
  lastDebugPrint = micros();
}
  // 1 µs precision (1 MHz)
  currTime = micros();

  // Run every 1 ms (1 kHz)
  if ((currTime - lastCycle) >= 1000)
  {
    // ---- Read ADC sensors every 50 ms (20 Hz) ----
    if ((currTime - lastRead) >= 50000)
    {
      const int inputs[] = {PIEZO_PIN, HALL_PINS[0], HALL_PINS[1], PIEZO_PIN};

      // Read whichever sensor cfg.pressureInput selects as the shift-force input
      pressureValue = analogRead(inputs[cfg.pressureInput]);
      if (cfg.pressureInput == 0)
        pressureValue = 4095 - pressureValue; // Piezo reads inverted vs. the Hall inputs

      // Handlebar mode button (optional; active-low on the selected ADC pin used as digital input)
      buttonPressed = cfg.buttonInput ? !digitalRead(inputs[cfg.buttonInput]) : false;

      // Rising edge on the button toggles the limiter state machine
      if (buttonPressed && !lastButtonState)
      {
        switch (limiterState)
        {
          case PIT:    limiterState = OFF; break;
          case LAUNCH: limiterState = OFF; break;

          case OFF:
            // Pick LAUNCH if we're basically stationary (below limiterMaxSpeed) and have a speed sensor,
            // otherwise fall back to plain PIT-style RPM capping.
            if (lastWheelSpeed < cfg.limiterMaxSpeed && cfg.wheelSensor)
              limiterState = LAUNCH;
            else
              limiterState = PIT;
            break;
        }
      }

      // If configured, the limiter is always on (no button needed) and forced into PIT mode
      if (cfg.limiterAlways)
        limiterState = PIT;                                    // TODO: Possible conflict with decelRetarding

      lastButtonState = buttonPressed;

      // RPM from the time between ignition pulses on channel 0.
      // 60,000,000 µs/min / delta = RPM for one pulse per crank rev (wasted spark).
      // 120,000,000 is used instead for sequential (720°) setups, where each
      // channel only fires once every 2 crank revolutions.
      if (lastRPMdelta[0] > 0)
      {
        lastRPM = ((cfg.wastedSpark > 700) ? 120000000UL : 60000000UL) / lastRPMdelta[0];
        //lastRPM = (cfg.wastedSpark ? 6000000UL : 12000000UL) / lastRPMdelta[0];
      }

      lastRead = currTime;
    }

    // ---- Read wheel speed every 500 ms (2 Hz) ----
    if (cfg.wheelSensor && (currTime - lastSpeedRead) >= 500000)
    {
      // pulses -> RPM: pulses * (60s / 0.5s interval) / pulses-per-rev = pulses * 120 / sensorPulses
      lastWheelRPM = (int)(speedPulses * (120 / (float)cfg.sensorPulses));
      speedPulses = 0;

      // RPM -> km/h using the configured wheel circumference (speedScale, mm)
      lastWheelSpeed = (lastWheelRPM * cfg.speedScale) / 60000.f;

      lastSpeedRead = currTime;
    }

    // Hysteresis: force must drop well below cutSens before we'll arm the next shift
    if (pressureValue < (cfg.cutSens - cfg.cutHyst))
      waitHyst = false;

    // ---- 2-step launch control / RPM limiter ----
    if ((cfg.launchRPM || cfg.limiterRPM) && cfg.limiterCut && cfg.limiterRetard)
    {
      switch (limiterState)
      {
        case LAUNCH:
          // Once the bike is moving fast enough, launch control releases automatically
          if (lastWheelSpeed > cfg.limiterMaxSpeed)
            limiterState = OFF;

          // Over the LAUNCH RPM ceiling and not already mid-cut: start limiting
          if (cfg.launchRPM && lastRPM > cfg.launchRPM && !shiftingTrig)
          {
            shiftingTrig = true;
            limitingRPM = true;
            waitHyst = true;
            currRetard = cfg.limiterRetard;
            // Proportional hold time from how far over the RPM ceiling we are, capped at 200 ms
            currHoldTime = min((int)((lastRPM - cfg.launchRPM) * cfg.limiterCut / 100.f), 200); // rpmError * gain and limit to 200 ms
            currRestore = (int)(cfg.limiterRetard / cfg.limiterDiv);
            cfg.fullCut = cfg.limiterFullCut; // temporarily override full-cut mode for the limiter
            lastCut = currTime;
          }
          break;

        case PIT:
          // Same idea as LAUNCH, but against the (usually lower/stationary) PIT RPM ceiling
          if (cfg.limiterRPM && lastRPM > cfg.limiterRPM && !shiftingTrig)
          {
            shiftingTrig = true;
            limitingRPM = true;
            waitHyst = true;
            currRetard = cfg.limiterRetard;
            currHoldTime = min((int)((lastRPM - cfg.limiterRPM) * cfg.limiterCut / 100.f), 200); // rpmError * gain and limit to 200 ms
            currRestore = (int)(cfg.limiterRetard / cfg.limiterDiv);
            cfg.fullCut = cfg.limiterFullCut;
            lastCut = currTime;
          }
          break;

        case OFF:
          // Limiter just released: restore the ordinary (non-limiter) full-cut setting
          if (limitingRPM)
          {
            limitingRPM = false;
            shiftingTrig = false;
            cfg.fullCut = CFG_FULL_CUT_DEFAULT;
          }
          break;
      }
    }

    // Below minRPM, always require the hysteresis to clear again before the next shift can trigger
    if (lastRPM < cfg.minRPM)
      waitHyst = true;

    // ---- Quickshifter trigger: force sensor over threshold, RPM in range, not already shifting/limiting, dead time elapsed ----
    if (pressureValue > cfg.cutSens && lastRPM >= cfg.minRPM && !waitHyst && !shiftingTrig && !limitingRPM && (currTime - lastCut) >= cfg.deadTime*1000)
    {
      shiftingTrig = true;
      // Interpolate retard angle and hold/cut time between the low-RPM and high-RPM settings, by current RPM
      currRetard = map(lastRPM, cfg.minRPM, cfg.maxRPM, cfg.retardLow, cfg.retardHigh);
      currHoldTime = map(lastRPM, cfg.minRPM, cfg.maxRPM, cfg.holdTimeLow, cfg.holdTimeHigh);

      // How much to reduce currRetard by on each subsequent ignition pulse (see coilInterrupt above)
      currRestore = (int)(currRetard / cfg.restore) + 1;

      waitHyst = true; // force must drop back down before the next shift can be armed

      // digitalWrite(GREEN_PIN, LOW); // On when shifting
      GPIO.out_w1tc = (1 << GREEN_PIN);

      lastCut = currTime;
    }

    // End the shift window once currHoldTime has elapsed (retard recovery continues afterwards via coilInterrupt)
    if (shiftingTrig && (currTime - lastCut) >= currHoldTime*1000)
    {
      shiftingTrig = false;
      // digitalWrite(GREEN_PIN, HIGH); // Off
      GPIO.out_w1ts = (1 << GREEN_PIN);
    }

    lastCycle = currTime;
  }
}
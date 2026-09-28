#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

enum lim {
  OFF = 0,
  LAUNCH = 1,
  PIT = 2
};

struct cfgOptions {
  // Quickshifter settings
  int retardLow       = 40;
  int retardHigh      = 60;
  int restore         = 20;
  int minRPM          = 2900;
  int maxRPM          = 12500;
  int holdTimeLow     = 49;
  int holdTimeHigh    = 72;
  int deadTime        = 300;
  int cutSens         = 1000;
  int cutHyst         = 500;
  bool fullCut        = true;
  int wastedSpark     = 360;

  // 2-step / Launch control / Pit limiter settings
  bool limiterAlways  = true;
  bool launchEnabled  = true;
  bool limiterFullCut = false;
  int limiterRPM      = 3600;
  int launchRPM       = 2500;
  int limiterCut      = 10;
  int limiterRetard   = 30;
  int limiterDiv      = 1;
  int limiterMaxSpeed = 15;

  // Sensor and hardware routing
  int pressureInput   = 0;
  int buttonInput     = 0;
  bool wheelSensor    = false;
  int speedScale      = 1528;
  int sensorPulses    = 70;

  // Gear-specific cut times (ms)
  int cutTimeGear[6]  = {65, 60, 55, 50, 45, 40};
};

extern cfgOptions cfg;
extern const bool CFG_FULL_CUT_DEFAULT;

// Telemetry & state variables shared with web portal
extern volatile int lastRPM;
extern float lastWheelSpeed;
extern int pressureValue;
extern int limiterState;
extern volatile bool shiftingTrig;
extern volatile bool limitingRPM;
extern bool waitHyst;

// Configuration persistence functions
void loadConfig();
void saveConfig();
void resetConfig();

#endif // CONFIG_H

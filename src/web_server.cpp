#include "web_server.h"
#include "dashboard_html.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

static WebServer server(80);
static Preferences prefs;
static TaskHandle_t webTaskHandle = NULL;

static int parseJsonInt(const String& json, const char* key, int defaultValue) {
  String searchKey = String("\"") + key + "\":";
  int idx = json.indexOf(searchKey);
  if (idx < 0) return defaultValue;
  int start = idx + searchKey.length();
  while (start < (int)json.length() && (json[start] == ' ' || json[start] == '\t')) start++;
  int end = start;
  while (end < (int)json.length() && (isDigit(json[end]) || json[end] == '-')) end++;
  if (start == end) return defaultValue;
  return json.substring(start, end).toInt();
}

static bool parseJsonBool(const String& json, const char* key, bool defaultValue) {
  String searchKey = String("\"") + key + "\":";
  int idx = json.indexOf(searchKey);
  if (idx < 0) return defaultValue;
  int start = idx + searchKey.length();
  while (start < (int)json.length() && (json[start] == ' ' || json[start] == '\t')) start++;
  if (json.startsWith("true", start)) return true;
  if (json.startsWith("false", start)) return false;
  return defaultValue;
}

static void parseJsonGears(const String& json, int gears[6]) {
  String searchKey = "\"cutTimeGear\":[";
  int idx = json.indexOf(searchKey);
  if (idx < 0) return;
  int pos = idx + searchKey.length();
  for (int i = 0; i < 6; i++) {
    while (pos < (int)json.length() && (json[pos] == ' ' || json[pos] == ',')) pos++;
    int start = pos;
    while (pos < (int)json.length() && isDigit(json[pos])) pos++;
    if (pos > start) {
      gears[i] = json.substring(start, pos).toInt();
    }
  }
}

void loadConfig() {
  prefs.begin("quickshift", true);
  if (!prefs.isKey("initialized")) {
    prefs.end();
    return;
  }

  cfg.retardLow       = prefs.getInt("retLow", cfg.retardLow);
  cfg.retardHigh      = prefs.getInt("retHigh", cfg.retardHigh);
  cfg.restore         = prefs.getInt("restore", cfg.restore);
  cfg.minRPM          = prefs.getInt("minRPM", cfg.minRPM);
  cfg.maxRPM          = prefs.getInt("maxRPM", cfg.maxRPM);
  cfg.holdTimeLow     = prefs.getInt("htLow", cfg.holdTimeLow);
  cfg.holdTimeHigh    = prefs.getInt("htHigh", cfg.holdTimeHigh);
  cfg.deadTime        = prefs.getInt("deadTime", cfg.deadTime);
  cfg.cutSens         = prefs.getInt("cutSens", cfg.cutSens);
  cfg.cutHyst         = prefs.getInt("cutHyst", cfg.cutHyst);
  cfg.fullCut         = prefs.getBool("fullCut", cfg.fullCut);
  cfg.wastedSpark     = prefs.getInt("wastedSpk", cfg.wastedSpark);

  cfg.limiterAlways   = prefs.getBool("limAlways", cfg.limiterAlways);
  cfg.launchEnabled   = prefs.getBool("launchEn", cfg.launchEnabled);
  cfg.limiterFullCut  = prefs.getBool("limFullCut", cfg.limiterFullCut);
  cfg.limiterRPM      = prefs.getInt("limRPM", cfg.limiterRPM);
  cfg.launchRPM       = prefs.getInt("launchRPM", cfg.launchRPM);
  cfg.limiterCut      = prefs.getInt("limCut", cfg.limiterCut);
  cfg.limiterRetard   = prefs.getInt("limRet", cfg.limiterRetard);
  cfg.limiterDiv      = prefs.getInt("limDiv", cfg.limiterDiv);
  cfg.limiterMaxSpeed = prefs.getInt("limSpeed", cfg.limiterMaxSpeed);

  cfg.pressureInput   = prefs.getInt("pressIn", cfg.pressureInput);
  cfg.buttonInput     = prefs.getInt("btnIn", cfg.buttonInput);
  cfg.wheelSensor     = prefs.getBool("whlSens", cfg.wheelSensor);
  cfg.speedScale      = prefs.getInt("spdScale", cfg.speedScale);
  cfg.sensorPulses    = prefs.getInt("snsPulse", cfg.sensorPulses);

  for (int i = 0; i < 6; i++) {
    char key[8];
    snprintf(key, sizeof(key), "g%d", i);
    cfg.cutTimeGear[i] = prefs.getInt(key, cfg.cutTimeGear[i]);
  }

  prefs.end();

  limiterState = cfg.limiterAlways ? PIT : (cfg.launchEnabled ? LAUNCH : OFF);
}

void saveConfig() {
  prefs.begin("quickshift", false);
  prefs.putBool("initialized", true);

  prefs.putInt("retLow", cfg.retardLow);
  prefs.putInt("retHigh", cfg.retardHigh);
  prefs.putInt("restore", cfg.restore);
  prefs.putInt("minRPM", cfg.minRPM);
  prefs.putInt("maxRPM", cfg.maxRPM);
  prefs.putInt("htLow", cfg.holdTimeLow);
  prefs.putInt("htHigh", cfg.holdTimeHigh);
  prefs.putInt("deadTime", cfg.deadTime);
  prefs.putInt("cutSens", cfg.cutSens);
  prefs.putInt("cutHyst", cfg.cutHyst);
  prefs.putBool("fullCut", cfg.fullCut);
  prefs.putInt("wastedSpk", cfg.wastedSpark);

  prefs.putBool("limAlways", cfg.limiterAlways);
  prefs.putBool("launchEn", cfg.launchEnabled);
  prefs.putBool("limFullCut", cfg.limiterFullCut);
  prefs.putInt("limRPM", cfg.limiterRPM);
  prefs.putInt("launchRPM", cfg.launchRPM);
  prefs.putInt("limCut", cfg.limiterCut);
  prefs.putInt("limRet", cfg.limiterRetard);
  prefs.putInt("limDiv", cfg.limiterDiv);
  prefs.putInt("limSpeed", cfg.limiterMaxSpeed);

  prefs.putInt("pressIn", cfg.pressureInput);
  prefs.putInt("btnIn", cfg.buttonInput);
  prefs.putBool("whlSens", cfg.wheelSensor);
  prefs.putInt("spdScale", cfg.speedScale);
  prefs.putInt("snsPulse", cfg.sensorPulses);

  for (int i = 0; i < 6; i++) {
    char key[8];
    snprintf(key, sizeof(key), "g%d", i);
    prefs.putInt(key, cfg.cutTimeGear[i]);
  }

  prefs.end();
}

void resetConfig() {
  prefs.begin("quickshift", false);
  prefs.clear();
  prefs.end();

  cfg = cfgOptions();
  limiterState = cfg.limiterAlways ? PIT : (cfg.launchEnabled ? LAUNCH : OFF);
  limitingRPM = false;
  shiftingTrig = false;
}

static void handleRoot() {
  server.send_P(200, "text/html", DASHBOARD_HTML);
}

static void handleStatus() {
  char json[384];
  const char* qsState = shiftingTrig ? "CUTTING" : "ACTIVE";
  const char* lcState = (limiterState == LAUNCH && limitingRPM && shiftingTrig) ? "CUTTING" : (cfg.launchEnabled ? "STANDBY" : "OFF");
  const char* pitState = (cfg.limiterAlways || limiterState == PIT) ? ((limitingRPM && shiftingTrig) ? "CUTTING" : "ACTIVE") : "OFF";

  snprintf(json, sizeof(json),
    "{\"rpm\":%d,\"qsState\":\"%s\",\"lcState\":\"%s\",\"pitState\":\"%s\",\"fullCut\":%s,\"pressure\":%d,\"speed\":%.1f}",
    lastRPM, qsState, lcState, pitState, cfg.fullCut ? "true" : "false", pressureValue, lastWheelSpeed);

  server.send(200, "application/json", json);
}

static void handleTogglePit() {
  if (cfg.limiterAlways || limiterState == PIT) {
    cfg.limiterAlways = false;
    limiterState = cfg.launchEnabled ? LAUNCH : OFF;
    limitingRPM = false;
    shiftingTrig = false;
    cfg.fullCut = CFG_FULL_CUT_DEFAULT;
  } else {
    cfg.limiterAlways = true;
    limiterState = PIT;
  }
  saveConfig();
  handleStatus();
}

static void handleToggleLaunch() {
  cfg.launchEnabled = !cfg.launchEnabled;
  if (!cfg.launchEnabled && limiterState == LAUNCH) {
    limiterState = cfg.limiterAlways ? PIT : OFF;
    limitingRPM = false;
    shiftingTrig = false;
    cfg.fullCut = CFG_FULL_CUT_DEFAULT;
  } else if (cfg.launchEnabled && !cfg.limiterAlways) {
    limiterState = LAUNCH;
  }
  saveConfig();
  handleStatus();
}


static void handleGetConfig() {
  char json[768];
  snprintf(json, sizeof(json),
    "{\"retardLow\":%d,\"retardHigh\":%d,\"restore\":%d,\"minRPM\":%d,\"maxRPM\":%d,"
    "\"holdTimeLow\":%d,\"holdTimeHigh\":%d,\"deadTime\":%d,\"cutSens\":%d,\"cutHyst\":%d,"
    "\"fullCut\":%s,\"wastedSpark\":%d,\"limiterAlways\":%s,\"launchEnabled\":%s,\"limiterFullCut\":%s,"
    "\"limiterRPM\":%d,\"launchRPM\":%d,\"limiterCut\":%d,\"limiterRetard\":%d,"
    "\"limiterDiv\":%d,\"limiterMaxSpeed\":%d,\"pressureInput\":%d,\"buttonInput\":%d,"
    "\"wheelSensor\":%s,\"speedScale\":%d,\"sensorPulses\":%d,"
    "\"cutTimeGear\":[%d,%d,%d,%d,%d,%d]}",
    cfg.retardLow, cfg.retardHigh, cfg.restore, cfg.minRPM, cfg.maxRPM,
    cfg.holdTimeLow, cfg.holdTimeHigh, cfg.deadTime, cfg.cutSens, cfg.cutHyst,
    cfg.fullCut ? "true" : "false", cfg.wastedSpark,
    cfg.limiterAlways ? "true" : "false", cfg.launchEnabled ? "true" : "false", cfg.limiterFullCut ? "true" : "false",
    cfg.limiterRPM, cfg.launchRPM, cfg.limiterCut, cfg.limiterRetard,
    cfg.limiterDiv, cfg.limiterMaxSpeed, cfg.pressureInput, cfg.buttonInput,
    cfg.wheelSensor ? "true" : "false", cfg.speedScale, cfg.sensorPulses,
    cfg.cutTimeGear[0], cfg.cutTimeGear[1], cfg.cutTimeGear[2],
    cfg.cutTimeGear[3], cfg.cutTimeGear[4], cfg.cutTimeGear[5]
  );
  server.send(200, "application/json", json);
}

static void handlePostConfig() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"Missing body\"}");
    return;
  }
  String body = server.arg("plain");

  cfg.retardLow       = parseJsonInt(body, "retardLow", cfg.retardLow);
  cfg.retardHigh      = parseJsonInt(body, "retardHigh", cfg.retardHigh);
  cfg.restore         = parseJsonInt(body, "restore", cfg.restore);
  cfg.minRPM          = parseJsonInt(body, "minRPM", cfg.minRPM);
  cfg.maxRPM          = parseJsonInt(body, "maxRPM", cfg.maxRPM);
  cfg.holdTimeLow     = parseJsonInt(body, "holdTimeLow", cfg.holdTimeLow);
  cfg.holdTimeHigh    = parseJsonInt(body, "holdTimeHigh", cfg.holdTimeHigh);
  cfg.deadTime        = parseJsonInt(body, "deadTime", cfg.deadTime);
  cfg.cutSens         = parseJsonInt(body, "cutSens", cfg.cutSens);
  cfg.cutHyst         = parseJsonInt(body, "cutHyst", cfg.cutHyst);
  cfg.fullCut         = parseJsonBool(body, "fullCut", cfg.fullCut);
  cfg.wastedSpark     = parseJsonInt(body, "wastedSpark", cfg.wastedSpark);

  bool prevLimiterAlways = cfg.limiterAlways;
  cfg.limiterAlways   = parseJsonBool(body, "limiterAlways", cfg.limiterAlways);
  cfg.launchEnabled   = parseJsonBool(body, "launchEnabled", cfg.launchEnabled);
  if (!cfg.limiterAlways && limiterState == PIT) {
    limiterState = cfg.launchEnabled ? LAUNCH : OFF;
    limitingRPM = false;
    shiftingTrig = false;
    cfg.fullCut = CFG_FULL_CUT_DEFAULT;
  } else if (cfg.limiterAlways) {
    limiterState = PIT;
  }
  cfg.limiterFullCut  = parseJsonBool(body, "limiterFullCut", cfg.limiterFullCut);
  cfg.limiterRPM      = parseJsonInt(body, "limiterRPM", cfg.limiterRPM);
  cfg.launchRPM       = parseJsonInt(body, "launchRPM", cfg.launchRPM);
  cfg.limiterCut      = parseJsonInt(body, "limiterCut", cfg.limiterCut);
  cfg.limiterRetard   = parseJsonInt(body, "limiterRetard", cfg.limiterRetard);
  cfg.limiterDiv      = parseJsonInt(body, "limiterDiv", cfg.limiterDiv);
  cfg.limiterMaxSpeed = parseJsonInt(body, "limiterMaxSpeed", cfg.limiterMaxSpeed);

  cfg.pressureInput   = parseJsonInt(body, "pressureInput", cfg.pressureInput);
  cfg.buttonInput     = parseJsonInt(body, "buttonInput", cfg.buttonInput);
  cfg.wheelSensor     = parseJsonBool(body, "wheelSensor", cfg.wheelSensor);
  cfg.speedScale      = parseJsonInt(body, "speedScale", cfg.speedScale);
  cfg.sensorPulses    = parseJsonInt(body, "sensorPulses", cfg.sensorPulses);

  parseJsonGears(body, cfg.cutTimeGear);

  saveConfig();
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

static void handleReset() {
  resetConfig();
  handleGetConfig();
}

static void webServerTask(void *param) {
  while (true) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void webServerInit() {
  loadConfig();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("QuickShift-ECU", "12345678");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/config", HTTP_GET, handleGetConfig);
  server.on("/api/config", HTTP_POST, handlePostConfig);
  server.on("/api/toggle-pit", HTTP_POST, handleTogglePit);
  server.on("/api/toggle-lc", HTTP_POST, handleToggleLaunch);
  server.on("/api/reset", HTTP_POST, handleReset);
  server.begin();

  // Run web server on Core 0 so Core 1 is dedicated to time-critical ignition logic
  xTaskCreatePinnedToCore(webServerTask, "webServerTask", 4096, NULL, 1, &webTaskHandle, 0);
}

void webServerHandle() {
  if (webTaskHandle == NULL) {
    server.handleClient();
  }
}

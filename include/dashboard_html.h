#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H

#include <Arduino.h>

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>QuickPro Dashboard</title>
<link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&display=swap" rel="stylesheet">
<style>
*{
margin:0;
padding:0;
box-sizing:border-box;
font-family:'Orbitron',sans-serif;
}
body{
background:#050505;
display:flex;
justify-content:center;
padding:15px;
color:white;
}
.app{
width:100%;
max-width:430px;
background:#111;
border-radius:24px;
padding:18px;
border:2px solid #222;
box-shadow:0 0 30px rgba(255,0,0,0.2);
}
.header{
text-align:center;
margin-bottom:20px;
}
.logo{
font-size:34px;
font-weight:900;
}
.logo span{
color:#ff2a2a;
}
.sub{
font-size:11px;
color:#777;
margin-top:5px;
letter-spacing:1px;
}
/* RPM GAUGE */
.top{
display:flex;
justify-content:center;
margin-bottom:14px;
}
.gauge-box{
background:#181818;
border-radius:24px;
padding:20px;
display:flex;
justify-content:center;
align-items:center;
border:1px solid #2a2a2a;
width:100%;
height:230px;
}
.gauge{
position:relative;
width:180px;
height:180px;
border-radius:50%;
background:conic-gradient(
#ff2a2a 0deg,
#ff2a2a 70deg,
#ffaa00 70deg,
#ffaa00 145deg,
#222 145deg,
#222 360deg
);
display:flex;
justify-content:center;
align-items:center;
transition:background 0.1s ease;
}
.gauge::before{
content:'';
position:absolute;
width:140px;
height:140px;
background:#181818;
border-radius:50%;
}
.rpm-content{
position:relative;
z-index:2;
text-align:center;
}
.rpm{
font-size:38px;
font-weight:900;
letter-spacing:1px;
}
.rpm-label{
font-size:12px;
color:#777;
margin-top:6px;
}
/* INFO CARDS */
.info{
display:grid;
grid-template-columns:1fr 1fr;
gap:10px;
margin-top:12px;
}
.info-card{
background:#181818;
padding:14px;
border-radius:16px;
border:1px solid #2a2a2a;
}
.info-title{
font-size:10px;
color:#777;
margin-bottom:8px;
letter-spacing:0.5px;
}
.info-value{
font-size:16px;
font-weight:bold;
}
.green{color:#00ff88;}
.red{color:#ff2a2a;}
.yellow{color:#ffaa00;}
.blue{color:#00aaff;}
/* BUTTONS */
.main-btn{
width:100%;
padding:16px;
margin-top:14px;
border:none;
border-radius:18px;
background:#ff2a2a;
color:white;
font-size:14px;
font-weight:bold;
cursor:pointer;
transition:0.2s;
}
.main-btn:active{
transform:scale(0.98);
}
.menu-grid{
display:grid;
grid-template-columns:1fr 1fr;
gap:10px;
margin-top:14px;
}
.menu-btn{
background:#181818;
border:1px solid #333;
color:white;
padding:15px;
border-radius:16px;
font-size:12px;
font-weight:bold;
cursor:pointer;
transition:0.3s;
}
.menu-btn:hover,.menu-btn:active{
border-color:#ff2a2a;
background:#222;
}
/* STATUS */
.status{
margin-top:18px;
padding:14px;
background:#002b14;
border:1px solid #00aa55;
border-radius:16px;
text-align:center;
font-size:12px;
font-weight:bold;
color:#00ff88;
transition:0.3s;
}
.status.disconnected{
background:#2b0000;
border-color:#aa0000;
color:#ff4444;
}
.footer{
margin-top:16px;
text-align:center;
font-size:10px;
color:#666;
}
/* POPUPS */
.overlay{
position:fixed;
top:0;
left:0;
width:100%;
height:100%;
background:rgba(0,0,0,0.75);
display:none;
z-index:998;
backdrop-filter:blur(3px);
}
.popup{
position:fixed;
top:50%;
left:50%;
transform:translate(-50%,-50%);
width:90%;
max-width:380px;
max-height:85vh;
overflow-y:auto;
background:#181818;
border:2px solid #ff2a2a;
border-radius:24px;
padding:20px;
display:none;
z-index:999;
box-shadow:0 0 40px rgba(255,42,42,0.3);
}
.popup h2{
text-align:center;
margin-bottom:20px;
color:#ff2a2a;
font-size:18px;
}
.setting-box{
margin-bottom:16px;
}
.setting-box label{
display:flex;
justify-content:space-between;
align-items:center;
margin-bottom:8px;
font-size:12px;
color:#aaa;
}
.slider{
width:100%;
accent-color:#ff2a2a;
height:6px;
background:#333;
border-radius:3px;
cursor:pointer;
}
.number-input{
width:80px;
padding:8px;
border:1px solid #333;
border-radius:10px;
background:#111;
color:white;
text-align:center;
font-size:13px;
}
.toggle-row{
display:flex;
justify-content:space-between;
align-items:center;
padding:10px 0;
font-size:12px;
color:#ccc;
border-bottom:1px solid #282828;
}
.toggle-switch{
position:relative;
display:inline-block;
width:46px;
height:24px;
}
.toggle-switch input{
opacity:0;
width:0;
height:0;
}
.switch-slider{
position:absolute;
cursor:pointer;
top:0;left:0;right:0;bottom:0;
background-color:#333;
transition:.3s;
border-radius:24px;
}
.switch-slider:before{
position:absolute;
content:"";
height:18px;
width:18px;
left:3px;
bottom:3px;
background-color:white;
transition:.3s;
border-radius:50%;
}
input:checked + .switch-slider{
background-color:#ff2a2a;
}
input:checked + .switch-slider:before{
transform:translateX(22px);
}
.select-input{
padding:8px;
border-radius:10px;
background:#111;
color:white;
border:1px solid #333;
font-size:12px;
width:130px;
}
.close-btn{
width:100%;
padding:15px;
border:none;
border-radius:16px;
background:#ff2a2a;
color:white;
font-weight:bold;
cursor:pointer;
margin-top:14px;
font-size:13px;
}
.cancel-btn{
width:100%;
padding:10px;
border:1px solid #444;
border-radius:16px;
background:transparent;
color:#aaa;
font-weight:bold;
cursor:pointer;
margin-top:8px;
font-size:11px;
}
</style>
</head>
<body>
<div class="app">
<div class="header">
<div class="logo">QUICK<span>PRO</span></div>
<div class="sub">RACING QUICKSHIFTER ECU</div>
</div>

<!-- RPM GAUGE -->
<div class="top">
<div class="gauge-box">
<div class="gauge" id="gaugeElem">
<div class="rpm-content">
<div class="rpm" id="rpm">0</div>
<div class="rpm-label">RPM</div>
</div>
</div>
</div>
</div>

<!-- INFO CARDS -->
<div class="info">
<div class="info-card">
<div class="info-title">BATTERY</div>
<div class="info-value yellow" id="valBatt">13.6V</div>
</div>
<div class="info-card">
<div class="info-title">ENGINE TEMP</div>
<div class="info-value red" id="valTemp">92°C</div>
</div>
<div class="info-card">
<div class="info-title">QUICKSHIFTER</div>
<div class="info-value green" id="valQS">ACTIVE</div>
</div>
<div class="info-card">
<div class="info-title">LAUNCH CONTROL</div>
<div class="info-value blue" id="valLC">STANDBY</div>
</div>
<div class="info-card">
<div class="info-title">BACKFIRE SHIFT</div>
<div class="info-value yellow" id="valBF">ON</div>
</div>
<div class="info-card">
<div class="info-title">PIT LIMITER</div>
<div class="info-value red" id="valPIT">OFF</div>
</div>
</div>

<button class="main-btn" onclick="openPopup('QUICKSHIFTER SETUP')">
SETTINGS QUICKSHIFTER
</button>

<div class="menu-grid">
<button class="menu-btn" onclick="openPopup('LC SETUP')">RPM SETUP</button>
<button class="menu-btn" onclick="openPopup('PIT LIMITER')">PIT LIMITER</button>
<button class="menu-btn" onclick="openPopup('CUT OFF RPM')">CUT OFF TIME</button>
<button class="menu-btn" onclick="openPopup('SETTING')">SETTING</button>
</div>

<div class="status" id="connStatus">SYSTEM CONNECTED</div>
<div class="footer">QUICKPRO ECU RACING EDITION v2.5</div>
</div>

<div class="overlay" id="overlay" onclick="closePopup()"></div>

<div class="popup" id="popup">
<h2 id="popupTitle">MENU</h2>
<div id="popupContent"></div>
<button class="close-btn" onclick="saveSettings()">SAVE SETTINGS</button>
<button class="cancel-btn" onclick="closePopup()">CANCEL</button>
</div>

<script>
let currentConfig = {
  retardLow: 40,
  retardHigh: 60,
  restore: 20,
  minRPM: 2900,
  maxRPM: 12500,
  holdTimeLow: 49,
  holdTimeHigh: 72,
  deadTime: 300,
  cutSens: 1000,
  cutHyst: 500,
  fullCut: true,
  wastedSpark: 360,
  limiterAlways: false,
  limiterFullCut: false,
  limiterRPM: 3600,
  launchRPM: 2500,
  limiterCut: 10,
  limiterRetard: 30,
  limiterDiv: 1,
  limiterMaxSpeed: 15,
  pressureInput: 0,
  buttonInput: 0,
  wheelSensor: false,
  speedScale: 1528,
  sensorPulses: 70,
  cutTimeGear: [65, 60, 55, 50, 45, 40]
};

let activeMenu = "";

// Fetch initial config from ECU
function loadInitialConfig() {
  fetch('/api/config')
    .then(res => res.json())
    .then(data => {
      currentConfig = Object.assign(currentConfig, data);
      updateDisplayBadges();
    })
    .catch(err => console.log('Offline/sim config mode', err));
}
loadInitialConfig();

// Real-time telemetry poller
setInterval(() => {
  fetch('/api/status')
    .then(res => res.json())
    .then(data => {
      document.getElementById('rpm').innerText = data.rpm;
      updateGauge(data.rpm);

      if (data.battery !== undefined) {
        document.getElementById('valBatt').innerText = data.battery.toFixed(1) + "V";
      }
      if (data.temp !== undefined) {
        document.getElementById('valTemp').innerText = data.temp + "°C";
      }
      if (data.qsState) {
        const qsElem = document.getElementById('valQS');
        qsElem.innerText = data.qsState;
        qsElem.className = 'info-value ' + (data.qsState === 'CUTTING' ? 'yellow' : (data.qsState === 'ACTIVE' ? 'green' : 'red'));
      }
      if (data.lcState) {
        const lcElem = document.getElementById('valLC');
        lcElem.innerText = data.lcState;
        lcElem.className = 'info-value ' + (data.lcState === 'ACTIVE' ? 'green' : (data.lcState === 'STANDBY' ? 'blue' : 'red'));
      }
      if (data.pitState) {
        const pitElem = document.getElementById('valPIT');
        pitElem.innerText = data.pitState;
        pitElem.className = 'info-value ' + (data.pitState === 'ACTIVE' ? 'green' : (data.pitState === 'STANDBY' ? 'yellow' : 'red'));
      }
      if (data.fullCut !== undefined) {
        document.getElementById('valBF').innerText = data.fullCut ? "ON" : "OFF";
      }

      const st = document.getElementById('connStatus');
      st.innerText = "SYSTEM CONNECTED";
      st.className = "status";
    })
    .catch(() => {
      const st = document.getElementById('connStatus');
      st.innerText = "CONNECTING...";
      st.className = "status disconnected";
    });
}, 150);

function updateGauge(rpmVal) {
  let angle = (rpmVal / 14000) * 270;
  if (angle > 360) angle = 360;
  if (angle < 0) angle = 0;
  const gauge = document.getElementById('gaugeElem');
  gauge.style.background = `conic-gradient(#ff2a2a 0deg, #ffaa00 ${Math.min(angle, 140)}deg, #ff2a2a ${Math.min(angle, 270)}deg, #222 ${angle}deg, #222 360deg)`;
}

function updateDisplayBadges() {
  document.getElementById('valBF').innerText = currentConfig.fullCut ? "ON" : "OFF";
}

function createSliderBox(id, label, unit, min, max, val, step=1) {
  return `
  <div class="setting-box">
    <label>
      <span>${label}</span>
      <div style="display:flex;align-items:center;gap:6px;">
        <input type="number" id="input_${id}" value="${val}" min="${min}" max="${max}" step="${step}" class="number-input" oninput="document.getElementById('range_${id}').value=this.value">
        <span style="font-size:11px;color:#777;">${unit}</span>
      </div>
    </label>
    <input type="range" id="range_${id}" class="slider" min="${min}" max="${max}" step="${step}" value="${val}" oninput="document.getElementById('input_${id}').value=this.value">
  </div>
  `;
}

function createToggle(id, label, checked) {
  return `
  <div class="toggle-row">
    <span>${label}</span>
    <label class="toggle-switch">
      <input type="checkbox" id="toggle_${id}" ${checked ? 'checked' : ''}>
      <span class="switch-slider"></span>
    </label>
  </div>
  `;
}

function openPopup(menu) {
  activeMenu = menu;
  document.getElementById("popup").style.display = "block";
  document.getElementById("overlay").style.display = "block";
  document.getElementById("popupTitle").innerHTML = menu;

  let content = "";

  if (menu === "QUICKSHIFTER SETUP") {
    content = `
      ${createSliderBox("minRPM", "TRIGGER MIN RPM", "RPM", 1000, 15000, currentConfig.minRPM, 100)}
      ${createSliderBox("maxRPM", "TRIGGER MAX RPM", "RPM", 2000, 20000, currentConfig.maxRPM, 100)}
      ${createSliderBox("cutSens", "FORCE SENSITIVITY", "ADC", 100, 4000, currentConfig.cutSens, 50)}
      ${createSliderBox("cutHyst", "HYSTERESIS THRESHOLD", "ADC", 50, 2000, currentConfig.cutHyst, 25)}
      ${createSliderBox("holdTimeLow", "CUT TIME @ LOW RPM", "MS", 10, 200, currentConfig.holdTimeLow)}
      ${createSliderBox("holdTimeHigh", "CUT TIME @ HIGH RPM", "MS", 10, 200, currentConfig.holdTimeHigh)}
      ${createSliderBox("retardLow", "IGNITION RETARD LOW", "DEG", 0, 120, currentConfig.retardLow)}
      ${createSliderBox("retardHigh", "IGNITION RETARD HIGH", "DEG", 0, 120, currentConfig.retardHigh)}
      ${createSliderBox("deadTime", "DEAD TIME BETWEEN SHIFTS", "MS", 50, 1000, currentConfig.deadTime, 25)}
      ${createSliderBox("restore", "RESTORE RAMP RATE", "PULSES", 1, 100, currentConfig.restore)}
      ${createToggle("fullCut", "FULL IGNITION CUT", currentConfig.fullCut)}
    `;
  }
  else if (menu === "LC SETUP") {
    content = `
      ${createSliderBox("launchRPM", "LAUNCH CONTROL RPM", "RPM", 1500, 16000, currentConfig.launchRPM, 100)}
      ${createSliderBox("limiterMaxSpeed", "LAUNCH MAX SPEED", "KM/H", 0, 80, currentConfig.limiterMaxSpeed)}
      ${createSliderBox("limiterRetard", "LIMITER RETARD", "DEG", 0, 90, currentConfig.limiterRetard)}
      ${createSliderBox("limiterCut", "EXTRA CUT GAIN", "MS", 1, 100, currentConfig.limiterCut)}
    `;
  }
  else if (menu === "PIT LIMITER") {
    content = `
      ${createSliderBox("limiterRPM", "PIT LIMIT RPM", "RPM", 1500, 12000, currentConfig.limiterRPM, 100)}
      ${createSliderBox("limiterMaxSpeed", "PIT LIMIT SPEED", "KM/H", 10, 120, currentConfig.limiterMaxSpeed)}
      ${createToggle("limiterAlways", "ALWAYS ARMED (PIT MODE)", currentConfig.limiterAlways)}
      ${createToggle("limiterFullCut", "USE FULL CUT FOR LIMITER", currentConfig.limiterFullCut)}
    `;
  }
  else if (menu === "CUT OFF RPM") {
    const gears = currentConfig.cutTimeGear || [65, 60, 55, 50, 45, 40];
    content = `
      <div style="font-size:11px;color:#888;margin-bottom:12px;text-align:center;">Individual Gear Cut-off Windows</div>
      ${createSliderBox("gear1", "GEAR 1", "MS", 20, 120, gears[0])}
      ${createSliderBox("gear2", "GEAR 2", "MS", 20, 120, gears[1])}
      ${createSliderBox("gear3", "GEAR 3", "MS", 20, 120, gears[2])}
      ${createSliderBox("gear4", "GEAR 4", "MS", 20, 120, gears[3])}
      ${createSliderBox("gear5", "GEAR 5", "MS", 20, 120, gears[4])}
      ${createSliderBox("gear6", "GEAR 6", "MS", 20, 120, gears[5])}
    `;
  }
  else if (menu === "SETTING") {
    content = `
      <div class="setting-box">
        <label>
          <span>SENSOR INPUT PIN</span>
          <select id="select_pressureInput" class="select-input">
            <option value="0" ${currentConfig.pressureInput == 0 ? 'selected' : ''}>Piezo (Pin 13)</option>
            <option value="1" ${currentConfig.pressureInput == 1 ? 'selected' : ''}>Hall ADC1 (Pin 10)</option>
            <option value="2" ${currentConfig.pressureInput == 2 ? 'selected' : ''}>Hall ADC2 (Pin 11)</option>
          </select>
        </label>
      </div>
      <div class="setting-box">
        <label>
          <span>IGNITION TIMING SYSTEM</span>
          <select id="select_wastedSpark" class="select-input">
            <option value="360" ${currentConfig.wastedSpark == 360 ? 'selected' : ''}>Wasted Spark (360°)</option>
            <option value="720" ${currentConfig.wastedSpark == 720 ? 'selected' : ''}>Sequential (720°)</option>
          </select>
        </label>
      </div>
      ${createToggle("wheelSensor", "REAR WHEEL SPEED SENSOR", currentConfig.wheelSensor)}
      <button class="close-btn" style="background:#555;margin-top:14px;" onclick="resetDefaults()">RESET ALL SETTINGS</button>
      <div style="margin-top:20px;font-size:11px;color:#777;line-height:1.7;text-align:center;">
        QUICKPRO ECU RACING EDITION<br>
        FIRMWARE: v2.5 MODULAR<br>
        ESP32-S3 CORE ARCHITECTURE
      </div>
    `;
  }

  document.getElementById("popupContent").innerHTML = content;
}

function saveSettings() {
  // Read fields from active popup
  if (activeMenu === "QUICKSHIFTER SETUP") {
    currentConfig.minRPM = parseInt(document.getElementById("input_minRPM").value);
    currentConfig.maxRPM = parseInt(document.getElementById("input_maxRPM").value);
    currentConfig.cutSens = parseInt(document.getElementById("input_cutSens").value);
    currentConfig.cutHyst = parseInt(document.getElementById("input_cutHyst").value);
    currentConfig.holdTimeLow = parseInt(document.getElementById("input_holdTimeLow").value);
    currentConfig.holdTimeHigh = parseInt(document.getElementById("input_holdTimeHigh").value);
    currentConfig.retardLow = parseInt(document.getElementById("input_retardLow").value);
    currentConfig.retardHigh = parseInt(document.getElementById("input_retardHigh").value);
    currentConfig.deadTime = parseInt(document.getElementById("input_deadTime").value);
    currentConfig.restore = parseInt(document.getElementById("input_restore").value);
    currentConfig.fullCut = document.getElementById("toggle_fullCut").checked;
  }
  else if (activeMenu === "LC SETUP") {
    currentConfig.launchRPM = parseInt(document.getElementById("input_launchRPM").value);
    currentConfig.limiterMaxSpeed = parseInt(document.getElementById("input_limiterMaxSpeed").value);
    currentConfig.limiterRetard = parseInt(document.getElementById("input_limiterRetard").value);
    currentConfig.limiterCut = parseInt(document.getElementById("input_limiterCut").value);
  }
  else if (activeMenu === "PIT LIMITER") {
    currentConfig.limiterRPM = parseInt(document.getElementById("input_limiterRPM").value);
    currentConfig.limiterMaxSpeed = parseInt(document.getElementById("input_limiterMaxSpeed").value);
    currentConfig.limiterAlways = document.getElementById("toggle_limiterAlways").checked;
    currentConfig.limiterFullCut = document.getElementById("toggle_limiterFullCut").checked;
  }
  else if (activeMenu === "CUT OFF RPM") {
    currentConfig.cutTimeGear = [
      parseInt(document.getElementById("input_gear1").value),
      parseInt(document.getElementById("input_gear2").value),
      parseInt(document.getElementById("input_gear3").value),
      parseInt(document.getElementById("input_gear4").value),
      parseInt(document.getElementById("input_gear5").value),
      parseInt(document.getElementById("input_gear6").value)
    ];
  }
  else if (activeMenu === "SETTING") {
    currentConfig.pressureInput = parseInt(document.getElementById("select_pressureInput").value);
    currentConfig.wastedSpark = parseInt(document.getElementById("select_wastedSpark").value);
    currentConfig.wheelSensor = document.getElementById("toggle_wheelSensor").checked;
  }

  // Send update to ESP32
  fetch('/api/config', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(currentConfig)
  })
  .then(res => res.json())
  .then(() => {
    updateDisplayBadges();
    closePopup();
  })
  .catch(err => {
    console.error("Save error:", err);
    closePopup();
  });
}

function resetDefaults() {
  if (confirm("Reset all settings to factory default?")) {
    fetch('/api/reset', {method: 'POST'})
      .then(res => res.json())
      .then(data => {
        currentConfig = data;
        updateDisplayBadges();
        closePopup();
      });
  }
}

function closePopup() {
  document.getElementById("popup").style.display = "none";
  document.getElementById("overlay").style.display = "none";
}
</script>
</body>
</html>)rawliteral";

#endif // DASHBOARD_HTML_H

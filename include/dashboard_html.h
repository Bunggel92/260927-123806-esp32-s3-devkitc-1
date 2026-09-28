#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H

#include <Arduino.h>

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>QuickShift Dashboard</title>
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
min-height:100vh;
display:flex;
justify-content:center;
align-items:center;
padding:20px;
color:white;
}
.app{
width:100%;
max-width:430px;
background:#111;
border-radius:24px;
padding:20px;
border:2px solid #222;
box-shadow:0 0 40px rgba(255,0,0,0.18);
transition:max-width 0.3s ease, padding 0.3s ease;
}
.header{
text-align:center;
margin-bottom:20px;
}
.header-top{
display:flex;
flex-direction:column;
align-items:center;
}
.header-extra{
display:none;
}
.logo{
font-size:34px;
font-weight:900;
letter-spacing:1px;
}
.logo span{
color:#ff2a2a;
}
.sub{
font-size:11px;
color:#777;
margin-top:5px;
letter-spacing:1.5px;
}

/* DASHBOARD LAYOUT */
.dash-layout{
display:flex;
flex-direction:column;
}
.dash-left{
display:flex;
flex-direction:column;
}
.dash-right{
display:flex;
flex-direction:column;
}

/* RPM GAUGE */
.top{
display:flex;
justify-content:center;
margin-bottom:14px;
height:100%;
}
.gauge-box{
background:#181818;
border-radius:24px;
padding:24px 20px;
display:flex;
flex-direction:column;
justify-content:center;
align-items:center;
border:1px solid #2a2a2a;
width:100%;
height:100%;
min-height:240px;
position:relative;
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
letter-spacing:1px;
}

/* SHIFT FORCE METER */
.force-meter{
width:85%;
max-width:260px;
margin-top:16px;
display:flex;
flex-direction:column;
gap:6px;
}
.force-meta{
display:flex;
justify-content:space-between;
font-size:10px;
color:#777;
letter-spacing:0.5px;
}
.force-track{
width:100%;
height:6px;
background:#222;
border-radius:3px;
overflow:hidden;
}
.force-fill{
width:0%;
height:100%;
background:linear-gradient(90deg, #00ff88, #ffaa00, #ff2a2a);
border-radius:3px;
transition:width 0.1s linear;
}

/* INFO CARDS */
.info{
display:grid;
grid-template-columns:1fr 1fr;
gap:12px;
margin-top:12px;
}
.info-card{
background:#181818;
padding:16px 14px;
border-radius:18px;
border:1px solid #2a2a2a;
transition:all 0.2s ease;
}
.info-card:hover{
border-color:#3a3a3a;
}
.info-card.interactive{
cursor:pointer;
}
.info-card.interactive:hover{
border-color:#ff2a2a;
box-shadow:0 0 15px rgba(255,42,42,0.2);
transform:translateY(-2px);
}
.info-card.interactive:active{
transform:translateY(0);
}
.info-title{
font-size:10px;
color:#777;
margin-bottom:8px;
letter-spacing:0.8px;
}
.info-value{
font-size:17px;
font-weight:bold;
letter-spacing:0.5px;
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
letter-spacing:1px;
cursor:pointer;
transition:all 0.2s ease;
box-shadow:0 4px 15px rgba(255,42,42,0.25);
}
.main-btn:hover{
background:#ff4444;
box-shadow:0 6px 20px rgba(255,42,42,0.4);
}
.main-btn:active{
transform:scale(0.98);
}
.menu-grid{
display:grid;
grid-template-columns:1fr 1fr;
gap:12px;
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
letter-spacing:0.5px;
cursor:pointer;
transition:all 0.25s ease;
}
.menu-btn:hover{
border-color:#ff2a2a;
background:#222;
box-shadow:0 0 12px rgba(255,42,42,0.2);
}
.menu-btn:active{
transform:scale(0.98);
}

/* STATUS & FOOTER */
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
letter-spacing:1px;
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
letter-spacing:0.5px;
}

/* POPUPS */
.overlay{
position:fixed;
top:0;
left:0;
width:100%;
height:100%;
background:rgba(0,0,0,0.8);
display:none;
z-index:998;
backdrop-filter:blur(5px);
}
.popup{
position:fixed;
top:50%;
left:50%;
transform:translate(-50%,-50%);
width:92%;
max-width:400px;
max-height:86vh;
overflow-y:auto;
background:#161616;
border:2px solid #ff2a2a;
border-radius:24px;
padding:24px;
display:none;
z-index:999;
box-shadow:0 0 50px rgba(255,42,42,0.35);
}
.popup h2{
text-align:center;
margin-bottom:20px;
color:#ff2a2a;
font-size:18px;
letter-spacing:1px;
}
.popup-grid{
display:flex;
flex-direction:column;
gap:12px;
}
.setting-box{
margin-bottom:8px;
background:#1c1c1c;
padding:12px 14px;
border-radius:14px;
border:1px solid #2a2a2a;
}
.setting-box label{
display:flex;
justify-content:space-between;
align-items:center;
margin-bottom:8px;
font-size:11px;
color:#aaa;
letter-spacing:0.5px;
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
padding:7px;
border:1px solid #3a3a3a;
border-radius:8px;
background:#111;
color:white;
text-align:center;
font-size:13px;
font-weight:bold;
}
.toggle-row{
display:flex;
justify-content:space-between;
align-items:center;
padding:12px 14px;
font-size:12px;
color:#ccc;
background:#1c1c1c;
border-radius:14px;
border:1px solid #2a2a2a;
margin-bottom:8px;
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
padding:8px 10px;
border-radius:8px;
background:#111;
color:white;
border:1px solid #3a3a3a;
font-size:12px;
}
.popup-actions{
display:flex;
flex-direction:column;
gap:8px;
margin-top:16px;
}
.close-btn{
width:100%;
padding:14px;
border:none;
border-radius:14px;
background:#ff2a2a;
color:white;
font-weight:bold;
cursor:pointer;
font-size:13px;
letter-spacing:1px;
transition:0.2s;
}
.close-btn:hover{
background:#ff4444;
}
.cancel-btn{
width:100%;
padding:11px;
border:1px solid #444;
border-radius:14px;
background:transparent;
color:#aaa;
font-weight:bold;
cursor:pointer;
font-size:11px;
letter-spacing:1px;
transition:0.2s;
}
.cancel-btn:hover{
color:white;
border-color:#777;
}

.ota-box{
background:#141414;
border:1px solid #2a2a2a;
border-radius:16px;
padding:20px;
text-align:center;
}
.ota-label{
display:block;
font-size:12px;
color:#aaa;
margin-bottom:14px;
letter-spacing:1px;
}
.ota-input{
display:block;
width:100%;
padding:14px;
background:#1a1a1a;
border:1px dashed #444;
border-radius:12px;
color:#ddd;
font-size:13px;
cursor:pointer;
box-sizing:border-box;
}
.ota-progress{
margin-top:18px;
display:none;
}
.ota-track{
width:100%;
height:14px;
background:#111;
border-radius:8px;
overflow:hidden;
border:1px solid #333;
}
.ota-fill{
height:100%;
width:0%;
background:linear-gradient(90deg, #ff8800, #00ff88);
transition:width 0.2s ease;
}
.ota-msg{
font-size:12px;
color:#aaa;
margin-top:10px;
font-weight:bold;
letter-spacing:1px;
}

/* ========================================================
   DESKTOP & WIDESCREEN VIEW (Full Racing ECU Cockpit Suite)
   ======================================================== */
@media (min-width: 960px) {
  body{
    padding: 30px;
  }
  .app{
    max-width: 1240px;
    width: 95%;
    padding: 30px 36px;
    border-radius: 28px;
  }
  .header{
    display: flex;
    justify-content: space-between;
    align-items: center;
    text-align: left;
    margin-bottom: 24px;
    padding-bottom: 20px;
    border-bottom: 1px solid #222;
  }
  .header-top{
    align-items: flex-start;
  }
  .header-extra{
    display: flex;
    align-items: center;
    gap: 16px;
  }
  .header-badge{
    padding: 8px 16px;
    background:#181818;
    border: 1px solid #333;
    border-radius: 12px;
    font-size: 11px;
    color: #aaa;
    letter-spacing: 1px;
  }
  .header-badge span{
    color: #ff2a2a;
    font-weight: bold;
  }
  .logo{
    font-size: 38px;
  }
  .sub{
    font-size: 12px;
    margin-top: 4px;
  }

  .dash-layout{
    display: grid;
    grid-template-columns: 1fr 1.25fr;
    gap: 26px;
    align-items: stretch;
  }
  .gauge-box{
    min-height: 400px;
    padding: 36px;
    border-radius: 24px;
  }
  .gauge{
    width: 250px;
    height: 250px;
  }
  .gauge::before{
    width: 194px;
    height: 194px;
  }
  .rpm{
    font-size: 52px;
    letter-spacing: 2px;
  }
  .rpm-label{
    font-size: 15px;
    margin-top: 8px;
  }
  .force-meter{
    max-width: 320px;
    margin-top: 24px;
  }
  .force-meta{
    font-size: 11px;
  }
  .force-track{
    height: 8px;
  }

  .info{
    margin-top: 0;
    gap: 16px;
  }
  .info-card{
    padding: 22px 24px;
    border-radius: 20px;
  }
  .info-title{
    font-size: 12px;
    margin-bottom: 10px;
  }
  .info-value{
    font-size: 24px;
  }

  .main-btn{
    margin-top: 18px;
    padding: 20px;
    font-size: 16px;
    border-radius: 18px;
  }
  .menu-grid{
    margin-top: 16px;
    gap: 14px;
  }
  .menu-btn{
    padding: 18px;
    font-size: 13px;
    border-radius: 18px;
  }

  .status{
    margin-top: 24px;
    padding: 16px;
    font-size: 13px;
    border-radius: 18px;
  }
  .footer{
    margin-top: 18px;
    font-size: 11px;
  }

  /* Desktop Popup Modals: Dual-column grid */
  .popup{
    max-width: 720px;
    padding: 30px;
    border-radius: 24px;
  }
  .popup h2{
    font-size: 22px;
    margin-bottom: 24px;
  }
  .popup-grid{
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 14px;
  }
  .popup-actions{
    flex-direction: row;
    justify-content: flex-end;
    gap: 12px;
    margin-top: 22px;
  }
  .close-btn{
    width: auto;
    min-width: 180px;
    padding: 14px 28px;
  }
  .cancel-btn{
    width: auto;
    min-width: 120px;
    padding: 14px 24px;
  }
}

/* ========================================================
   TABLET / MID-SIZE LANDSCAPE
   ======================================================== */
@media (min-width: 640px) and (max-width: 959px) {
  .app{
    max-width: 780px;
    padding: 20px 24px;
  }
  .dash-layout{
    display: grid;
    grid-template-columns: 1fr 1.15fr;
    gap: 18px;
    align-items: stretch;
  }
  .gauge-box{
    min-height: 280px;
  }
  .gauge{
    width: 190px;
    height: 190px;
  }
  .gauge::before{
    width: 146px;
    height: 146px;
  }
  .rpm{
    font-size: 40px;
  }
  .info{
    margin-top: 0;
    gap: 10px;
  }
  .popup{
    max-width: 580px;
  }
}
</style>
</head>
<body>
<div class="app">
  <div class="header">
    <div class="header-top">
      <div class="logo">QUICK<span>SHIFT</span></div>
      <div class="sub">RACING QUICKSHIFTER ECU</div>
    </div>
    <div class="header-extra">
      <div class="header-badge">FIRMWARE: <span>v2.5 PRO</span></div>
      <div class="header-badge">DEVICE: <span>ESP32-S3</span></div>
    </div>
  </div>

  <div class="dash-layout">
    <!-- LEFT: RPM GAUGE & SENSOR METER -->
    <div class="dash-left">
      <div class="top">
        <div class="gauge-box">
          <div class="gauge" id="gaugeElem">
            <div class="rpm-content">
              <div class="rpm" id="rpm">0</div>
              <div class="rpm-label">RPM</div>
            </div>
          </div>
          <div class="force-meter">
            <div class="force-meta">
              <span>SHIFT FORCE</span>
              <span id="forceVal">0 ADC</span>
            </div>
            <div class="force-track">
              <div class="force-fill" id="forceFill"></div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- RIGHT: STATUS TILES & ACTION CONTROLS -->
    <div class="dash-right">
      <div class="info">
        <div class="info-card">
          <div class="info-title">QUICKSHIFTER</div>
          <div class="info-value green" id="valQS">ACTIVE</div>
        </div>
        <div class="info-card interactive" id="cardLC" onclick="toggleLaunchControl()" title="Click to toggle Launch Control">
          <div class="info-title">LAUNCH CONTROL</div>
          <div class="info-value blue" id="valLC">STANDBY</div>
        </div>
        <div class="info-card">
          <div class="info-title">BACKFIRE SHIFT</div>
          <div class="info-value yellow" id="valBF">ON</div>
        </div>
        <div class="info-card interactive" id="cardPIT" onclick="togglePitLimiter()" title="Click to toggle Pit Limiter">
          <div class="info-title">PIT LIMITER</div>
          <div class="info-value green" id="valPIT">ACTIVE</div>
        </div>
      </div>

      <button class="main-btn" onclick="openPopup('QUICKSHIFTER SETUP')">
        SETTINGS QUICKSHIFTER
      </button>

      <div class="menu-grid">
        <button class="menu-btn" onclick="openPopup('LC SETUP')">RPM SETUP</button>
        <button class="menu-btn" onclick="openPopup('PIT LIMITER')">PIT LIMITER</button>
        <button class="menu-btn" onclick="openPopup('CUT OFF RPM')">CUT OFF TIME</button>
        <button class="menu-btn" onclick="openPopup('SETTINGS')">SETTINGS</button>
      </div>
    </div>
  </div>

  <div class="status" id="connStatus">SYSTEM CONNECTED</div>
  <div class="footer">QUICKSHIFT ECU RACING EDITION v2.5</div>
</div>

<div class="overlay" id="overlay" onclick="closePopup()"></div>

<div class="popup" id="popup">
  <h2 id="popupTitle">MENU</h2>
  <div id="popupContent"></div>
  <div class="popup-actions" id="popupActions">
    <button class="cancel-btn" onclick="closePopup()">CANCEL</button>
    <button class="close-btn" onclick="saveSettings()">SAVE SETTINGS</button>
  </div>
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
  limiterAlways: true,
  launchEnabled: true,
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

      if (data.pressure !== undefined) {
        document.getElementById('forceVal').innerText = data.pressure + " ADC";
        let pct = Math.min(100, Math.max(0, (data.pressure / 4095) * 100));
        document.getElementById('forceFill').style.width = pct + "%";
      }

      if (data.qsState) {
        const qsElem = document.getElementById('valQS');
        qsElem.innerText = data.qsState;
        qsElem.className = 'info-value ' + (data.qsState === 'CUTTING' ? 'yellow' : (data.qsState === 'ACTIVE' ? 'green' : 'red'));
      }
      if (data.lcState) {
        const lcElem = document.getElementById('valLC');
        lcElem.innerText = data.lcState;
        lcElem.className = 'info-value ' + (
          (data.lcState === 'ACTIVE' || data.lcState === 'CUTTING') ? 'green' :
          (data.lcState === 'STANDBY' || data.lcState === 'ARMED' || data.lcState === 'ON') ? 'blue' : 'red'
        );
      }
      if (data.pitState) {
        const pitElem = document.getElementById('valPIT');
        pitElem.innerText = data.pitState;
        pitElem.className = 'info-value ' + (
          (data.pitState === 'ACTIVE' || data.pitState === 'CUTTING') ? 'green' :
          (data.pitState === 'STANDBY' || data.pitState === 'ARMED' || data.pitState === 'ON') ? 'yellow' : 'red'
        );
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

  const actions = document.getElementById("popupActions");
  if (actions) actions.style.display = "";

  let content = "";

  if (menu === "OTA UPDATE") {
    if (actions) actions.style.display = "none";
    content = `
      <div class="ota-box">
        <div class="ota-label">SELECT FIRMWARE BINARY (.bin)</div>
        <input type="file" id="otaFileInput" accept=".bin" class="ota-input">
        <div class="ota-progress" id="otaProgressBox">
          <div class="ota-track">
            <div class="ota-fill" id="otaBarFill"></div>
          </div>
          <div class="ota-msg" id="otaStatusText">0%</div>
        </div>
      </div>
      <div style="margin-top:20px;display:flex;flex-direction:column;gap:10px;">
        <button class="close-btn" id="otaBtnUpdate" style="background:#00aaff;width:100%;" onclick="startOtaUpdate()">UPDATE</button>
        <button class="cancel-btn" id="otaBtnCancel" style="width:100%;" onclick="openPopup('SETTINGS')">CANCEL</button>
      </div>
    `;
  }
  else if (menu === "QUICKSHIFTER SETUP") {
    content = `
      <div class="popup-grid">
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
      </div>
      ${createToggle("fullCut", "FULL IGNITION CUT (BACKFIRE)", currentConfig.fullCut)}
    `;
  }
  else if (menu === "LC SETUP") {
    content = `
      <div class="popup-grid">
        ${createSliderBox("launchRPM", "LAUNCH CONTROL RPM", "RPM", 1500, 16000, currentConfig.launchRPM, 100)}
        ${createSliderBox("limiterMaxSpeed", "LAUNCH MAX SPEED", "KM/H", 0, 80, currentConfig.limiterMaxSpeed)}
        ${createSliderBox("limiterRetard", "LIMITER RETARD", "DEG", 0, 90, currentConfig.limiterRetard)}
        ${createSliderBox("limiterCut", "EXTRA CUT GAIN", "MS", 1, 100, currentConfig.limiterCut)}
      </div>
      ${createToggle("launchEnabled", "ENABLE LAUNCH CONTROL (STANDBY)", currentConfig.launchEnabled)}
    `;
  }
  else if (menu === "PIT LIMITER") {
    content = `
      <div class="popup-grid">
        ${createSliderBox("limiterRPM", "PIT LIMIT RPM", "RPM", 1500, 12000, currentConfig.limiterRPM, 100)}
        ${createSliderBox("limiterMaxSpeed", "PIT LIMIT SPEED", "KM/H", 10, 120, currentConfig.limiterMaxSpeed)}
      </div>
      ${createToggle("limiterAlways", "ALWAYS ARMED (PIT MODE)", currentConfig.limiterAlways)}
      ${createToggle("limiterFullCut", "USE FULL CUT FOR LIMITER", currentConfig.limiterFullCut)}
    `;
  }
  else if (menu === "CUT OFF RPM") {
    const gears = currentConfig.cutTimeGear || [65, 60, 55, 50, 45, 40];
    content = `
      <div style="font-size:12px;color:#888;margin-bottom:12px;text-align:center;">Individual Gear Cut-off Windows</div>
      <div class="popup-grid">
        ${createSliderBox("gear1", "GEAR 1", "MS", 20, 120, gears[0])}
        ${createSliderBox("gear2", "GEAR 2", "MS", 20, 120, gears[1])}
        ${createSliderBox("gear3", "GEAR 3", "MS", 20, 120, gears[2])}
        ${createSliderBox("gear4", "GEAR 4", "MS", 20, 120, gears[3])}
        ${createSliderBox("gear5", "GEAR 5", "MS", 20, 120, gears[4])}
        ${createSliderBox("gear6", "GEAR 6", "MS", 20, 120, gears[5])}
      </div>
    `;
  }
  else if (menu === "SETTINGS" || menu === "SETTING") {
    content = `
      <div class="popup-grid">
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
            <span>IGNITION TIMING</span>
            <select id="select_wastedSpark" class="select-input">
              <option value="360" ${currentConfig.wastedSpark == 360 ? 'selected' : ''}>Wasted Spark (360°)</option>
              <option value="720" ${currentConfig.wastedSpark == 720 ? 'selected' : ''}>Sequential (720°)</option>
            </select>
          </label>
        </div>
      </div>
      ${createToggle("wheelSensor", "REAR WHEEL SPEED SENSOR", currentConfig.wheelSensor)}
      <button class="main-btn" style="background:#00aaff;margin-top:14px;color:#fff;" onclick="openPopup('OTA UPDATE')">FIRMWARE OTA UPDATE</button>
      <button class="close-btn" style="background:#444;margin-top:10px;" onclick="resetDefaults()">RESET ALL SETTINGS</button>
      <div style="margin-top:20px;font-size:11px;color:#777;line-height:1.7;text-align:center;">
        QUICKSHIFT RACING ECU SYSTEM<br>
        FIRMWARE: v2.5 PRO<br>
        PLATFORM: ESP32-S3 DUAL-CORE
      </div>
    `;
  }

  document.getElementById("popupContent").innerHTML = content;
}

function saveSettings() {
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
    currentConfig.launchEnabled = document.getElementById("toggle_launchEnabled").checked;
    const lcElem = document.getElementById('valLC');
    if (currentConfig.launchEnabled) {
      lcElem.innerText = "STANDBY";
      lcElem.className = "info-value blue";
    } else {
      lcElem.innerText = "OFF";
      lcElem.className = "info-value red";
    }
  }
  else if (activeMenu === "PIT LIMITER") {
    currentConfig.limiterRPM = parseInt(document.getElementById("input_limiterRPM").value);
    currentConfig.limiterMaxSpeed = parseInt(document.getElementById("input_limiterMaxSpeed").value);
    currentConfig.limiterAlways = document.getElementById("toggle_limiterAlways").checked;
    currentConfig.limiterFullCut = document.getElementById("toggle_limiterFullCut").checked;
    const pitElem = document.getElementById('valPIT');
    if (currentConfig.limiterAlways) {
      pitElem.innerText = "ACTIVE";
      pitElem.className = "info-value green";
    } else {
      pitElem.innerText = "OFF";
      pitElem.className = "info-value red";
    }
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
  else if (activeMenu === "SETTINGS" || activeMenu === "SETTING") {
    currentConfig.pressureInput = parseInt(document.getElementById("select_pressureInput").value);
    currentConfig.wastedSpark = parseInt(document.getElementById("select_wastedSpark").value);
    currentConfig.wheelSensor = document.getElementById("toggle_wheelSensor").checked;
  }

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

function startOtaUpdate() {
  const fileInput = document.getElementById("otaFileInput");
  if (!fileInput.files || fileInput.files.length === 0) {
    alert("Please select a firmware (.bin) file first!");
    return;
  }

  const file = fileInput.files[0];
  const progressBox = document.getElementById("otaProgressBox");
  const barFill = document.getElementById("otaBarFill");
  const statusText = document.getElementById("otaStatusText");
  const btnUpdate = document.getElementById("otaBtnUpdate");
  const btnCancel = document.getElementById("otaBtnCancel");

  progressBox.style.display = "block";
  btnUpdate.disabled = true;
  btnUpdate.style.opacity = "0.5";
  btnUpdate.style.cursor = "not-allowed";
  btnCancel.disabled = true;
  btnCancel.style.opacity = "0.5";
  btnCancel.style.cursor = "not-allowed";
  fileInput.disabled = true;

  statusText.innerText = "UPLOADING: 0%";
  barFill.style.width = "0%";

  const formData = new FormData();
  formData.append("update", file);

  const xhr = new XMLHttpRequest();
  xhr.open("POST", "/update", true);

  xhr.upload.addEventListener("progress", (e) => {
    if (e.lengthComputable) {
      const pct = Math.round((e.loaded / e.total) * 100);
      barFill.style.width = pct + "%";
      statusText.innerText = "UPLOADING: " + pct + "%";
      if (pct >= 100) {
        statusText.innerText = "FLASHING FIRMWARE...";
      }
    }
  });

  xhr.onload = function() {
    let success = false;
    try {
      const res = JSON.parse(xhr.responseText);
      if (res.status === "success") success = true;
    } catch (e) {
      if (xhr.status === 200) success = true;
    }

    if (xhr.status === 200 && success) {
      barFill.style.width = "100%";
      barFill.style.background = "#00ff88";
      let countdown = 6;
      statusText.innerHTML = `<span style="color:#00ff88;">UPDATE COMPLETE! REBOOTING (${countdown}s)...</span>`;
      const interval = setInterval(() => {
        countdown--;
        if (countdown > 0) {
          statusText.innerHTML = `<span style="color:#00ff88;">UPDATE COMPLETE! REBOOTING (${countdown}s)...</span>`;
        } else {
          clearInterval(interval);
          statusText.innerHTML = `<span style="color:#00ff88;">RECONNECTING...</span>`;
          window.location.reload();
        }
      }, 1000);
    } else {
      barFill.style.background = "#ff2a2a";
      statusText.innerHTML = `<span style="color:#ff2a2a;">UPDATE FAILED! PLEASE RETRY</span>`;
      btnUpdate.disabled = false;
      btnUpdate.style.opacity = "1";
      btnUpdate.style.cursor = "pointer";
      btnCancel.disabled = false;
      btnCancel.style.opacity = "1";
      btnCancel.style.cursor = "pointer";
      fileInput.disabled = false;
    }
  };

  xhr.onerror = function() {
    barFill.style.background = "#ff2a2a";
    statusText.innerHTML = `<span style="color:#ff2a2a;">UPDATE FAILED: CONNECTION LOST</span>`;
    btnUpdate.disabled = false;
    btnUpdate.style.opacity = "1";
    btnUpdate.style.cursor = "pointer";
    btnCancel.disabled = false;
    btnCancel.style.opacity = "1";
    btnCancel.style.cursor = "pointer";
    fileInput.disabled = false;
  };

  xhr.send(formData);
}

function togglePitLimiter() {
  fetch('/api/toggle-pit', {method: 'POST'})
    .then(res => res.json())
    .then(data => {
      if (data.pitState) {
        const pitElem = document.getElementById('valPIT');
        pitElem.innerText = data.pitState;
        pitElem.className = 'info-value ' + (
          (data.pitState === 'ACTIVE' || data.pitState === 'CUTTING') ? 'green' :
          (data.pitState === 'STANDBY' || data.pitState === 'ARMED' || data.pitState === 'ON') ? 'yellow' : 'red'
        );
      }
    })
    .catch(err => console.log('Toggle error', err));
}

function toggleLaunchControl() {
  fetch('/api/toggle-lc', {method: 'POST'})
    .then(res => res.json())
    .then(data => {
      if (data.lcState) {
        const lcElem = document.getElementById('valLC');
        lcElem.innerText = data.lcState;
        lcElem.className = 'info-value ' + (
          (data.lcState === 'ACTIVE' || data.lcState === 'CUTTING') ? 'green' :
          (data.lcState === 'STANDBY' || data.lcState === 'ARMED' || data.lcState === 'ON') ? 'blue' : 'red'
        );
      }
    })
    .catch(err => console.log('Toggle LC error', err));
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
  const actions = document.getElementById("popupActions");
  if (actions) actions.style.display = "";
}
</script>
</body>
</html>)rawliteral";

#endif // DASHBOARD_HTML_H

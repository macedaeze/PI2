#ifndef WEB_UI_H
#define WEB_UI_H

#include <Arduino.h>

const char MAIN_page[] PROGMEM = R"HTML(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Sensors & Servo</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:'Inter',sans-serif;background:#fff;color:#333;padding-bottom:140px}
header{background:linear-gradient(90deg,#7dd3fc,#a5b4fc,#f9a8d4);padding:16px;text-align:center;font-size:1.1rem;font-weight:600;color:#1e1e1e;box-shadow:0 3px 15px rgba(0,0,0,.1)}
footer{margin-top:40px;width:100%;background:linear-gradient(90deg,#86efac,#c084fc);padding:12px;position:fixed;bottom:0;left:0;text-align:center;font-size:.9rem;color:#1e1e1e;font-weight:500}
h1{margin-top:24px;text-align:center;font-size:1.7rem;background:linear-gradient(90deg,#0ea5e9,#c084fc);-webkit-background-clip:text;-webkit-text-fill-color:transparent}
h2{font-size:1.3rem;margin-bottom:8px}
.row{display:flex;flex-wrap:wrap;justify-content:center;gap:24px;margin-top:25px}
.card{width:300px;padding:18px;background:#fff;border-radius:16px;border:1px solid #e5e7eb;box-shadow:0 4px 16px rgba(0,0,0,.08);text-align:center}
.value{font-size:2rem;font-weight:700;margin:4px 0;color:#374151}
button{padding:10px 14px;border-radius:10px;border:0;cursor:pointer;font-weight:600;margin:6px;transition:opacity .2s}
button:hover{opacity:.8}
button:active{transform:scale(.95)}
.start{background:#0ea5e9;color:#fff}
.stop{background:#ef4444;color:#fff}
.center{background:#10b981;color:#fff}
.alert{font-weight:600;margin-top:6px}
.alert-ok{color:#10b981}
.alert-warn{color:#f97316}
.alert-danger{color:#dc2626}
.muted{color:#6b7280}
input[type=range]{width:100%;margin:8px 0}
</style>
</head>
<body>
<header>Ingeniería en Sistemas Espaciales — Proyecto Integrador 2 — 2do cuatrimestre 2025</header>
<h1>Panel de Control — ESP32</h1>
<div class="row">
<div class="card">
<h2>DS18B20</h2>
<p>Temperatura</p>
<p class="value"><span id="temp">--</span>°C</p>
</div>
<div class="card">
<h2>MQ135</h2>
<p>RAW</p>
<p class="value"><span id="raw">----</span></p>
<p>Voltaje</p>
<p class="value"><span id="volt">-.-</span> V</p>
<p id="gasAlert" class="alert muted">Esperando datos...</p>
<p id="gasStability" class="muted">Estado: midiendo...</p>
</div>
<div class="card">
<h2>Servo MG996R</h2>
<p>Ángulo actual: <b><span id="angle">90</span>°</b></p>
<p>Velocidad (°/s)</p>
<input id="speed" type="range" min="10" max="350" value="60" step="10">
<p><span id="speedv">60</span></p>
<button class="start" onclick="start()">Start</button>
<button class="stop" onclick="stop()">Stop</button>
<button class="center" onclick="center()">Center</button>
</div>
</div>
<footer>Team Pipinas — Ailin Ferrari · Emiliano Vitale · Ezequiel Maceda · Johanna Olivera</footer>
<script>
const el={temp:document.getElementById('temp'),raw:document.getElementById('raw'),volt:document.getElementById('volt'),angle:document.getElementById('angle'),gasAlert:document.getElementById('gasAlert'),gasStab:document.getElementById('gasStability'),speed:document.getElementById('speed'),speedv:document.getElementById('speedv')};
let lastVoltages=[],stableState="none",isTransition=false;
async function getJSON(u){try{let r=await fetch(u);return r.json()}catch(e){return null}}
function classifyRaw(v){if(v>=.5&&v<1.2)return"aire";if(v>=1.2&&v<2)return"respiracion";if(v>=2&&v<4)return"alcohol";return"fuera"}
function updateGasAlerts(v){lastVoltages.push(v);if(lastVoltages.length>10)lastVoltages.shift();let rawState=classifyRaw(v),isStable=false;if(lastVoltages.length>=5){let minV=Math.min(...lastVoltages),maxV=Math.max(...lastVoltages);if((maxV-minV)<.05)isStable=true}if(isStable){stableState=rawState;isTransition=false}else if(rawState!=stableState){isTransition=true}el.gasAlert.className='alert';if(isTransition){el.gasAlert.textContent="Volviendo a equilibrarse...";el.gasAlert.classList.add("muted");el.gasStab.textContent="Estado: variando...";return}if(stableState==="aire"){el.gasAlert.textContent="Aire limpio (0.5–1.2 V)";el.gasAlert.classList.add("alert-ok")}else if(stableState==="respiracion"){el.gasAlert.textContent="⚠ Respiración humana detectada (1.2–2.0 V)";el.gasAlert.classList.add("alert-warn")}else if(stableState==="alcohol"){el.gasAlert.textContent="⚠ Alcohol detectado (2.0–4.0 V)";el.gasAlert.classList.add("alert-danger")}else{el.gasAlert.textContent="Fuera de rango (0.5–4.0 V)";el.gasAlert.classList.add("muted")}el.gasStab.textContent=isStable?"Estado: estabilizado":"Estado: variando..."}
async function updateSensors(){let d=await getJSON('/temp');if(d&&d.ok)el.temp.innerText=d.temperature.toFixed(1);let g=await getJSON('/gas');if(g){el.raw.innerText=g.raw;el.volt.innerText=g.voltage.toFixed(2);updateGasAlerts(g.voltage)}}
async function refreshServo(){let s=await getJSON('/status');if(s)el.angle.innerText=s.angle}
el.speed.oninput=()=>el.speedv.innerText=el.speed.value;
async function start(){await fetch('/start?speed='+el.speed.value)}
async function stop(){await fetch('/stop')}
async function center(){await fetch('/set?angle=90')}
setInterval(updateSensors,1000);
setInterval(refreshServo,600);
</script>
</body>
</html>)HTML";

#endif // WEB_UI_H
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ESP32Servo.h>

#include <OneWire.h>
#include <DallasTemperature.h>

/* ------------------------------------------
Intento trabajar con constantes, así
tenés que cambiar todo una sola vez
------------------------------------------- */

/* ------------------------------------------
INICIO WIFI
------------------------------------------- */
const char* WIFI_SSID = "Nombre de la red, tal cual aparezca en la compu/celu";
const char* WIFI_PASS = "Contraseña";
/* ------------------------------------------
FIN WIFI
------------------------------------------- */


/* ------------------------------------------
INICIO SENSORES (DS18B20 + MQ135)
------------------------------------------- */
#define ONE_WIRE_BUS 4 // Pin definido para el sensor de temperatura
const int MQ135_PIN = 34; // Pin definido para el sensor de gases

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
/* ------------------------------------------
FIN SENSORES
------------------------------------------- */


/* ------------------------------------------
INICIO SERVOMOTOR
------------------------------------------- */
#define SERVO_PIN 22 // Pin definido para el servomotor
Servo servo;

int minUs = 600;
int maxUs = 2500;

volatile bool running = false;
int currentAngle = 90; // Centro
int minMax = 25; // Más menos del centro

// NUEVO: limitar servo a 90° ± 25°
int minAngle = currentAngle - minMax;
int maxAngle = currentAngle + minMax;

int speedDps = 60;
int stepDeg = 1;
int dir = +1;

unsigned long lastStepMs = 0;

void updateStepDeg() {
  if (speedDps <= 70)        stepDeg = 1;
  else if (speedDps <= 150)  stepDeg = 2;
  else if (speedDps <= 250)  stepDeg = 3;
  else                       stepDeg = 4;
}

unsigned long stepIntervalMs() {
  float ms = (1000.0 * stepDeg) / speedDps;
  return (ms < 20 ? 20 : ms);
}
/* ------------------------------------------
FIN SERVOMOTOR
------------------------------------------- */


/* ------------------------------------------
INICIO SERVIDOR WEB
------------------------------------------- */
WebServer server(80);
/* ------------------------------------------
FIN SERVIDOR WEB
------------------------------------------- */


/* ------------------------------------------
INICIO HTML UI
Vas a tener que agregar acá lo que sea que
agregues para que aparezca en la pantalla
------------------------------------------- */
const char MAIN_page[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Sensors & Servo</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
  body {
    font-family: 'Inter', sans-serif;
    background: #ffffff;
    margin: 0;
    padding: 0;
    padding-bottom: 140px;
    color: #333;
  }

  header {
    background: linear-gradient(90deg, #7dd3fc, #a5b4fc, #f9a8d4);
    padding: 16px;
    text-align: center;
    font-size: 1.1rem;
    font-weight:600;
    color: #1e1e1e;
    box-shadow: 0px 3px 15px rgba(0,0,0,0.1);
  }

  footer {
    margin-top: 40px;
    width: 100%;
    background: linear-gradient(90deg, #86efac, #c084fc);
    padding: 12px;
    position: fixed;
    bottom: 0;
    left: 0;
    text-align: center;
    font-size: 0.9rem;
    color: #1e1e1e;
    font-weight:500;
  }

  h1 {
    margin-top: 24px;
    text-align:center;
    font-size: 1.7rem;
    background: linear-gradient(90deg, #0ea5e9, #c084fc);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
  }

  .row {
    display:flex;
    flex-wrap:wrap;
    justify-content:center;
    gap:24px;
    margin-top:25px;
  }

  .card {
    width:300px;
    padding:18px;
    background:white;
    border-radius:16px;
    border:1px solid #e5e7eb;
    box-shadow:0px 4px 16px rgba(0,0,0,0.08);
    text-align:center;
  }

  .value {
    font-size:2rem;
    font-weight:bold;
    margin:4px 0;
    color:#374151;
  }

  button {
    padding:10px 14px;
    border-radius:10px;
    border:0;
    cursor:pointer;
    font-weight:600;
    margin:6px;
  }

  .start { background:#0ea5e9; color:white; }
  .stop { background:#ef4444; color:white; }
  .center { background:#10b981; color:white; }

  .alert { font-weight:600; margin-top:6px; }
  .alert-ok { color:#10b981; }
  .alert-warn { color:#f97316; }
  .alert-danger { color:#dc2626; }
  .muted { color:#6b7280; }
</style>

</head>

<body>

<header>
  Ingeniería en Sistemas Espaciales — Proyecto Integrador 2 — 2do cuatrimestre 2025
</header>

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

<footer>
  Team Pipinas — Ailin Ferrari · Emiliano Vitale · Ezequiel Maceda · Johanna Olivera
</footer>


<script>
async function getJSON(u){
  try{ let r=await fetch(u); return r.json(); }
  catch(e){ return null; }
}

const speed=document.getElementById('speed');
const speedv=document.getElementById('speedv');
const angleEl=document.getElementById('angle');
const gasAlertEl=document.getElementById('gasAlert');
const gasStabEl=document.getElementById('gasStability');

let lastVoltages=[];
let stableState="none";
let isTransition=false;

function classifyRaw(v){
  if (v>=0.5 && v<1.2) return "aire";
  if (v>=1.2 && v<2.0) return "respiracion";
  if (v>=2.0 && v<4.0) return "alcohol";
  return "fuera";
}

function updateGasAlerts(v){
  lastVoltages.push(v);
  if(lastVoltages.length>10) lastVoltages.shift();

  let rawState=classifyRaw(v);

  let isStable=false;
  if(lastVoltages.length>=5){
    let minV=Math.min(...lastVoltages);
    let maxV=Math.max(...lastVoltages);
    if((maxV-minV)<0.05) isStable=true;
  }

  if(isStable){
    stableState=rawState;
    isTransition=false;
  }
  else if(rawState!=stableState){
    isTransition=true;
  }

  gasAlertEl.classList.remove("alert-ok","alert-warn","alert-danger","muted");

  if(isTransition){
    gasAlertEl.textContent="Volviendo a equilibrarse...";
    gasAlertEl.classList.add("muted");
    gasStabEl.textContent="Estado: variando...";
    return;
  }

  if(stableState==="aire"){
    gasAlertEl.textContent="Aire limpio (0.5–1.2 V)";
    gasAlertEl.classList.add("alert-ok");
  }
  else if(stableState==="respiracion"){
    gasAlertEl.textContent="⚠ Respiración humana detectada (1.2–2.0 V)";
    gasAlertEl.classList.add("alert-warn");
  }
  else if(stableState==="alcohol"){
    gasAlertEl.textContent="⚠ Alcohol detectado (2.0–4.0 V)";
    gasAlertEl.classList.add("alert-danger");
  }
  else {
    gasAlertEl.textContent="Fuera de rango (0.5–4.0 V)";
    gasAlertEl.classList.add("muted");
  }

  gasStabEl.textContent = isStable ? "Estado: estabilizado" : "Estado: variando...";
}

async function updateSensors(){
  let d=await getJSON('/temp');
  if(d && d.ok){
    temp.innerText=d.temperature.toFixed(1);
  }

  let g=await getJSON('/gas');
  if(g){
    raw.innerText=g.raw;
    volt.innerText=g.voltage.toFixed(2);
    updateGasAlerts(g.voltage);
  }
}

async function refreshServo(){
  let s=await getJSON('/status');
  if(!s) return;
  angleEl.innerText=s.angle;
}

speed.oninput=()=> speedv.innerText=speed.value;
async function start(){ await fetch('/start?speed='+speed.value); }
async function stop(){  await fetch('/stop'); }
async function center(){await fetch('/set?angle=90');}

setInterval(updateSensors,1000);
setInterval(refreshServo,600);
</script>

</body>
</html>
)HTML";
/* ------------------------------------------
FIN HTML UI
------------------------------------------- */


/* ------------------------------------------
INICIO HANDLERS SENSORES
------------------------------------------- */
void handleRoot(){ server.send_P(200,"text/html",MAIN_page); }

void handleTemp() {
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);

  if (t == DEVICE_DISCONNECTED_C) {
    server.send(200,"application/json","{\"ok\":false}");
    return;
  }

  String json = "{\"ok\":true,\"temperature\":" + String(t,1) + "}";
  server.send(200,"application/json",json);
}

void handleGas(){
  int raw=analogRead(MQ135_PIN);
  float voltage=(3.3*raw)/4095.0;
  String json="{\"raw\":"+String(raw)+",\"voltage\":"+String(voltage,3)+"}";
  server.send(200,"application/json",json);
}
/* ------------------------------------------
FIN HANDLERS SENSORES
------------------------------------------- */


/* ------------------------------------------
INICIO HANDLERS SERVOMOTOR
------------------------------------------- */
void handleStart(){
  if(server.hasArg("speed")){
    speedDps=constrain(server.arg("speed").toInt(),10,350);
  }

  updateStepDeg();

  if(!servo.attached()) servo.attach(SERVO_PIN,minUs,maxUs);
  running=true;
  dir=+1;
  lastStepMs=millis();

  server.send(200,"text/plain","started");
}

void handleStop(){
  running=false;
  server.send(200,"text/plain","stopped");
}

void handleSet(){
  if(!server.hasArg("angle")){
    server.send(400,"text/plain","missing angle");
    return;
  }
  int a=constrain(server.arg("angle").toInt(),minAngle,maxAngle);
  running=false;

  if(!servo.attached()) servo.attach(SERVO_PIN,minUs,maxUs);

  currentAngle=a;
  servo.write(currentAngle);

  server.send(200,"text/plain","ok");
}

void handleStatus(){
  String json="{\"angle\":"+String(currentAngle)
             +",\"running\":"+(running?"true":"false")
             +",\"attached\":"+(servo.attached()?"true":"false")+"}";
  server.send(200,"application/json",json);
}
/* ------------------------------------------
FIN HANDLERS SERVOMOTOR
------------------------------------------- */


/* ------------------------------------------
INICIO WIFI SETUP
Vas a tener que abrir "Serial Monitor" para 
ver esto, es la lupita que ves en la esquina
superior derecha de la pantalla.
Cuando conecte, vas a poder entrar por:
1) http://esp-servo.local/
2) La IP que te aparezca, recomiendo esta
------------------------------------------- */
void connectWiFi(){
  Serial.println("\nConectando a WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PASS);

  unsigned long start=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-start<15000){
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if(WiFi.status()==WL_CONNECTED){
    Serial.print("Conectado! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("❌ No se pudo conectar a WiFi");
  }
}
/* ------------------------------------------
FIN WIFI SETUP
------------------------------------------- */


/* ------------------------------------------
INICIO SETUP
------------------------------------------- */
void setup(){
  Serial.begin(115200);
  delay(500);

  sensors.begin();
  pinMode(MQ135_PIN,INPUT);

  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN,minUs,maxUs);
  servo.write(currentAngle);

  connectWiFi();

  if(WiFi.status()==WL_CONNECTED) MDNS.begin("esp-demo");

  server.on("/",handleRoot);
  server.on("/temp",handleTemp);
  server.on("/gas",handleGas);
  server.on("/start",handleStart);
  server.on("/stop",handleStop);
  server.on("/set",handleSet);
  server.on("/status",handleStatus);

  server.begin();
  Serial.println("Servidor HTTP listo");
}
/* ------------------------------------------
FIN SETUP
------------------------------------------- */


/* ------------------------------------------
INICIO LOOP
------------------------------------------- */
void loop(){
  server.handleClient();

  if(running && servo.attached()){
    unsigned long now=millis();
    if(now-lastStepMs>=stepIntervalMs()){
      lastStepMs=now;

      int next=currentAngle + dir*stepDeg;

      if(next>=maxAngle){ next=maxAngle; dir=-1; }
      if(next<=minAngle){ next=minAngle; dir=+1; }

      currentAngle=next;
      servo.write(currentAngle);
    }
  }
}
/* ------------------------------------------
FIN LOOP
------------------------------------------- */

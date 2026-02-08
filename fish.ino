#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include <time.h>


const char* WIFI_SSID = "v4";
const char* WIFI_PASS = "danaioana4";


const int SERVO_PIN = 18;
const int TRIG_PIN  = 5;
const int ECHO_PIN  = 17;

const float LOW_FOOD_DISTANCE_CM = 6.0; 


const int CLOSED_DEG = 0;
const int OPEN_DEG   = 90;
const int HOLD_MS    = 800;


WebServer server(80);
Servo myServo;
Preferences prefs;


String feedTime = "08:00";
int lastFedYDay = -1;


float lastDistanceCm = -1;
String foodStatus = "Unknown";


String twoDigits(int v) { return (v < 10) ? ("0" + String(v)) : String(v); }

bool getTimeNow(struct tm &t) {
  return getLocalTime(&t, 50);
}

String nowHHMM() {
  struct tm t;
  if (!getTimeNow(t)) return "--:--";
  return twoDigits(t.tm_hour) + ":" + twoDigits(t.tm_min);
}


float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duration == 0) return -1;

  return (duration * 0.0343f) / 2.0f;
}

void updateFoodStatus() {
  lastDistanceCm = readDistanceCm();
  if (lastDistanceCm < 0) foodStatus = "Sensor error";
  else if (lastDistanceCm > LOW_FOOD_DISTANCE_CM) foodStatus = "Low food";
  else foodStatus = "OK";
}


void dispenseFood() {
  myServo.write(CLOSED_DEG);
  delay(HOLD_MS);

  myServo.write(OPEN_DEG);
  delay(HOLD_MS);

  myServo.write(CLOSED_DEG);
  delay(HOLD_MS);
}


void setupTime() {
  // Europe/Bucharest (EET/EEST)
  setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
  tzset();
  configTime(0, 0, "pool.ntp.org", "time.google.com", "time.nist.gov");
}


void maybeFeedOncePerDay() {
  struct tm t;
  if (!getTimeNow(t)) return;


  if (t.tm_yday == lastFedYDay) return;

  String hhmm = twoDigits(t.tm_hour) + ":" + twoDigits(t.tm_min);
  if (hhmm == feedTime) {
    dispenseFood();
    lastFedYDay = t.tm_yday;
  }
}


String pageHtml() {
  String html = R"HTML(
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Fish Feeder</title>
  <style>
    :root{
      --bg:#f4f6fb;
      --card:#ffffff;
      --text:#101828;
      --muted:#667085;
      --line:#e4e7ec;
      --accent:#2563eb;
      --ok:#16a34a;
      --bad:#dc2626;
    }
    *{box-sizing:border-box}
    body{
      margin:0;
      font-family: ui-sans-serif, system-ui, -apple-system, Segoe UI, Roboto, Arial;
      background: var(--bg);
      color: var(--text);
    }
    .wrap{max-width:760px; margin:0 auto; padding:20px;}
    header{margin-bottom:12px;}
    h1{font-size:22px; margin:0 0 4px;}
    .sub{font-size:12px; color:var(--muted); margin:0;}
    .card{
      background:var(--card);
      border:1px solid var(--line);
      border-radius:16px;
      padding:16px;
      box-shadow: 0 10px 25px rgba(16,24,40,.06);
      margin-bottom:12px;
    }
    .grid{
      display:grid;
      grid-template-columns: 1fr 1fr;
      gap:12px;
    }
    @media (max-width:720px){ .grid{grid-template-columns:1fr;} }
    .label{font-size:12px; color:var(--muted); margin-bottom:6px;}
    .value{font-size:22px; font-weight:900; letter-spacing:.2px;}
    .small{font-size:12px; color:var(--muted); margin-top:10px; line-height:1.45;}

    .status{
      display:inline-flex; align-items:center; gap:8px;
      padding:8px 12px; border-radius:999px;
      border:1px solid var(--line); background:#fafafa;
      font-weight:900; width:fit-content;
    }
    .dot{width:10px; height:10px; border-radius:999px; background:#98a2b3;}
    .ok{color:var(--ok);}
    .ok .dot{background:var(--ok);}
    .bad{color:var(--bad);}
    .bad .dot{background:var(--bad);}

    .divider{height:1px; background:var(--line); margin:12px 0;}

    /* custom time picker */
    .pickerRow{display:flex; align-items:flex-end; gap:10px; margin-top:8px;}
    .pickerBox{flex:1;}
    .pickerLabel{font-size:12px; color:var(--muted); margin-bottom:6px;}
    select{
      width:100%;
      padding:12px 12px;
      font-size:18px;
      font-weight:900;
      border-radius:14px;
      border:1px solid var(--line);
      background:#f9fafb;
      color:var(--text);
      outline:none;
    }
    select:focus{
      border-color: rgba(37,99,235,.55);
      box-shadow: 0 0 0 4px rgba(37,99,235,.12);
      background:#fff;
    }
    .colon{
      font-size:28px; font-weight:900; line-height:1;
      padding:0 2px 10px; opacity:.8;
    }

    .btnRow{display:flex; gap:10px; margin-top:12px;}
    button{
      flex:1;
      padding:12px 14px;
      font-size:15px;
      font-weight:900;
      border-radius:14px;
      border:1px solid var(--line);
      cursor:pointer;
    }
    .primary{
      background: var(--accent);
      color:#fff;
      border-color: rgba(37,99,235,.35);
    }
    .secondary{
      background:#fff;
      color:var(--text);
    }
  </style>
</head>
<body>
  <div class="wrap">
    <header>
      <h1>Fish Feeder</h1>
      <p class="sub">Set one feeding time per day. If distance &gt; 6 cm, it shows “Low food”.</p>
    </header>

    <div class="card">
      <div class="grid">
        <div>
          <div class="label">Current time</div>
          <div class="value" id="now">--:--</div>
        </div>
        <div>
          <div class="label">Feeding time (once/day)</div>
          <div class="value" id="ft">--:--</div>
        </div>
      </div>

      <div class="divider"></div>

      <div class="grid">
        <div>
          <div class="label">Distance</div>
          <div class="value"><span id="dist">--</span> cm</div>
        </div>
        <div>
          <div class="label">Food status</div>
          <div class="status" id="foodPill"><span class="dot"></span><span id="food">--</span></div>
        </div>
      </div>
    </div>

    <div class="card">
      <div class="label">Choose feeding time</div>

      <form action="/set" method="POST">
        <div class="pickerRow">
          <div class="pickerBox">
            <div class="pickerLabel">Hour</div>
            <select name="hour" id="hourSel"></select>
          </div>

          <div class="colon">:</div>

          <div class="pickerBox">
            <div class="pickerLabel">Minute</div>
            <select name="minute" id="minSel"></select>
          </div>
        </div>

        <div class="btnRow">
          <button class="primary" type="submit">Save</button>
          <button class="secondary" type="button" onclick="manualFeed()">Dispense now</button>
        </div>
      </form>

      <div class="small">Manual feed does not change the once/day schedule.</div>
    </div>
  </div>

<script>
function fillSelect(id, start, end, step){
  const sel = document.getElementById(id);
  sel.innerHTML = "";
  for(let v=start; v<=end; v+=step){
    const opt = document.createElement("option");
    opt.value = String(v).padStart(2,"0");
    opt.textContent = String(v).padStart(2,"0");
    sel.appendChild(opt);
  }
}
function setPickerFromFeedTime(feedTime){
  if(!feedTime || feedTime.length < 5) return;
  document.getElementById('hourSel').value = feedTime.slice(0,2);
  document.getElementById('minSel').value  = feedTime.slice(3,5);
}

fillSelect("hourSel", 0, 23, 1);
fillSelect("minSel", 0, 59, 1); // every minute (change to 5 if you want 0,5,10,...)

async function refresh(){
  const r = await fetch('/status');
  const s = await r.json();

  document.getElementById('now').textContent = s.now;
  document.getElementById('ft').textContent  = s.feed_time;
  document.getElementById('dist').textContent = s.distance_cm.toFixed(1);

  const pill = document.getElementById('foodPill');
  const food = document.getElementById('food');

  food.textContent = s.food_status;

  pill.classList.remove('ok','bad');
  if(s.food_status === "Low food") pill.classList.add('bad');
  else if(s.food_status === "OK") pill.classList.add('ok');

  setPickerFromFeedTime(s.feed_time);
}

async function manualFeed(){
  await fetch('/feed');
  alert('Dispensed!');
}

setInterval(refresh, 1000);
refresh();
</script>

</body>
</html>
)HTML";
  return html;
}


void handleRoot() {
  server.send(200, "text/html", pageHtml());
}

void handleStatus() {
  updateFoodStatus();
  float dist = (lastDistanceCm < 0) ? 0 : lastDistanceCm;

  String json = "{";
  json += "\"now\":\"" + nowHHMM() + "\",";
  json += "\"feed_time\":\"" + feedTime + "\",";
  json += "\"distance_cm\":" + String(dist) + ",";
  json += "\"food_status\":\"" + foodStatus + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleSet() {
  if (!server.hasArg("hour") || !server.hasArg("minute")) {
    server.send(400, "text/plain", "Missing hour/minute");
    return;
  }

  String hh = server.arg("hour");
  String mm = server.arg("minute");

  if (hh.length() == 1) hh = "0" + hh;
  if (mm.length() == 1) mm = "0" + mm;

  feedTime = hh + ":" + mm;
  prefs.putString("feedTime", feedTime);

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleManualFeed() {
  dispenseFood();
  server.send(200, "text/plain", "OK");
}


void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2500);


  prefs.begin("feeder", false);
  feedTime = prefs.getString("feedTime", feedTime);


  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  setupTime();


  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/set", HTTP_POST, handleSet);
  server.on("/feed", handleManualFeed);
  server.begin();

  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
  maybeFeedOncePerDay();
  delay(10);
}

// Import required libraries
#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO true

// Set number of relays or leds
#define NUM_RELAYS  5

// Assign each GPIO to a relay (each port a relay or led)
int relayGPIOs[NUM_RELAYS] = {5, 4, 14, 12, 13};
uint8_t LED_Pin = D4;     // declare LED pin on NodeMCU Dev Kit used for wifi_connectivity check


const char* ssid = "abc";//ssid of our wifi
const char* password = "987654321"; //password or my network

const char* PARAM_INPUT_1 = "relay";  //setting parameter we are going to use
const char* PARAM_INPUT_2 = "state";

// Creating AsyncWebServer object on port 80
AsyncWebServer server(80);

//Server code
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center;}
    h2 {font-size: 4.0rem; background-color:#ffcccc}
    p {font-size: 4.0rem;}
    body {max-width: 700px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 150px; height: 75px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ffccff; border-radius: 15px}
    .slider:before {position: absolute; content: ""; height: 60px; width: 60px; left: 8px; bottom: 8px; background-color: #4d194d; -webkit-transition: .4s; transition: .4s; border-radius: 75px}
    input:checked+.slider {background-color: #e60073}
    input:checked+.slider:before {-webkit-transform: translateX(75px); -ms-transform: translateX(75px); transform: translateX(75px)}
  </style>
</head>
<body>
  <h2>WiFi Connectivity on NodeMCU</h2><h4> Project: Computer Networks <br></h4>
<h3> <i> 200779 Izza Naeem <br> 200785 M. Ahmad Mehmood </i> <br> <br></h3>
  <h4>Relay #1 - GPIO 2</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="1"><span class="slider"></span></label><h4>Relay #2 - GPIO 4</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="2"><span class="slider"></span></label><h4>Relay #3 - GPIO 14</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="3"><span class="slider"></span></label><h4>Relay #4 - GPIO 12</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="4"><span class="slider"></span></label><h4>Relay #5 - GPIO 13</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="5"><span class="slider"></span></label>
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    for(int i=1; i<=NUM_RELAYS; i++){
      String relayStateValue = relayState(i);
      buttons+= "<h4>Relay #" + String(i) + " - GPIO " + relayGPIOs[i-1] + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    }
    return buttons;
  }
  return String();
}

String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "";
    }
    else {
      return "checked";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "checked";
    }
    else {
      return "";
    }
  }
  return "";
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);

  pinMode(LED_Pin, OUTPUT);   // Initialize the LED pin as an output (internal LED)

  // Set all relays to off when the program starts - if set to Normally Open (NO), the relay is off when you set the relay to HIGH
  for(int i=1; i<=NUM_RELAYS; i++){
    pinMode(relayGPIOs[i-1], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i-1], HIGH);
    }
    else{
        pinMode(LED_Pin, OUTPUT);

      digitalWrite(relayGPIOs[i-1], LOW);
    }
  }
  
  // Connecting to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    digitalWrite(LED_Pin, LOW); // Turn the LED on
  delay(500);                // Wait for a second
  digitalWrite(LED_Pin, HIGH);// Turn the LED off                // Wait for a second
  }
  //  if (WiFi.status() == WL_CONNECTED) {
  //  digitalWrite(LED_Pin, LOW); // Turn the LED on
  // }
 

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      if(RELAY_NO){
        Serial.print("NO ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
      }
      else{
        Serial.print("NC ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  server.begin();
}
  
void loop() {

}
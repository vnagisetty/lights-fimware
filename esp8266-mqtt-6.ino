/*
 * 5/13/21: adding json formatting
 */
#include "secrets8266.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "libb64/cdecode.h"
}
#include <ArduinoJson.h>
/* send following message in topic
 * inTopic
 * {
"status": 0,
"deviceId": "A1",
"otherInfo": "None"
}
 */

int LED = 5; // Assign LED pin i.e: D1 on NodeMCU
int GPIO4 = 4; // Assign LED pin i.e: D2 on NodeMCU

unsigned long lastPublish;
int msgCount;
int light_status;
int msg_in;
int x;

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(9600);
  x=0;
  light_status=0;
  msg_in=65;
 

  Serial.println("setup x:");
  Serial.println(x);
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());
  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  
  pinMode(LED, OUTPUT);
  pinMode(GPIO4, OUTPUT);
 
 Serial.println("Blink..example..setup");
 setCurrentTime();  
// Serial.println("certificatePemCrt="); 
 // Serial.println(certificatePemCrt); 
 
  uint8_t binaryCert[certificatePemCrt.length() * 3 / 4];
  int len = b64decode(certificatePemCrt, binaryCert);
  wiFiClient.setCertificate(binaryCert, len);
  
  uint8_t binaryPrivate[privatePemKey.length() * 3 / 4];
  len = b64decode(privatePemKey, binaryPrivate);
  wiFiClient.setPrivateKey(binaryPrivate, len);

  uint8_t binaryCA[caPemCrt.length() * 3 / 4];
  len = b64decode(caPemCrt, binaryCA);
  wiFiClient.setCACert(binaryCA, len);
   
}


// the loop function runs over and over again forever
void loop() {
  
  Serial.println("Blink..example..loop");

  Serial.print("In loop light status:");
  Serial.println(light_status);

  if (x>0){ x=0;
    Serial.println("Received message...");
    if (light_status==1){ digitalWrite(GPIO4, LOW);
        digitalWrite(LED, LOW);
        Serial.println("RELAY ON...");
      }
     else {digitalWrite(GPIO4, HIGH);
      digitalWrite(LED, HIGH);
      Serial.println("RELAY OFF...");
      }
  }
 /* Serial.print("msg_in: ");  
   Serial.println(msg_in);
  Serial.print("light_status:");  
  Serial.println(light_status);  
  Serial.print("In loop x:");
  Serial.println(x);
  */
  
  /*
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  digitalWrite(LED, LOW); // turn the LED on
  digitalWrite(GPIO4, LOW); // turn the LED on
  delay(5100);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  digitalWrite(LED, HIGH); // turn the LED off
  digitalWrite(GPIO4, HIGH); // turn the LED on
  delay(5100);                      // Wait for two seconds (to demonstrate the active low LED)
*/
   pubSubCheckConnect();
/*
  if (x==1){ msg_in=0; x=0;
        Serial.print("turning light on.. ");  
         
      if (light_status=1){
      digitalWrite(GPIO4, HIGH);
      digitalWrite(LED, LOW);
      }
     else { 
      digitalWrite(GPIO4, LOW);
      digitalWrite(LED, HIGH);
    }
  }
  */
  
  if (millis() - lastPublish > 1000) {
    String msg = String("Hello from ESP8266: ") + ++msgCount;
    StaticJsonDocument<200> doc;
    doc["time"] =millis() ;
    doc["deviceId"]="A1";
    doc["status"]=1;
    doc["otherInfo"]="Nothing for now";
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client
    //pubSubClient.publish("outTopic", msg.c_str());
    //pubSubClient.publish("outTopic", jsonBuffer);
    //Serial.print("Published: "); 
    //Serial.println(jsonBuffer);
    lastPublish = millis();
  }
  
  delay(1000);
}

//void toJason(String abels[],String values[]){
/*
void toJson(){
   StaticJsonDocument<200> doc;
  doc["time"] =millis() ;
  //doc["sensor_a0"] = analogRead(0);
  doc["deviceId"]='A1';
  doc["status"]=1;
  doc["otherInfo"]='Nothing for now';
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
}
*/
void msgReceived(char* topic, byte* payload, unsigned int length) {
  msg_in++;

  ++x;
  Serial.print("In msg x:");
  Serial.println(x);
  
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  char  payload_str[length];
  for (int i = 0; i < length; i++) {
    
    Serial.print((char)payload[i]);
    payload_str[i] = (char)payload[i];
    
  }
 /* for (int i = 0; i < length; i++){
  Serial.print(payload_str[i]);
  }
*/
//    Serial.println("incoming: " + topic + " - " + payload_str);

   StaticJsonDocument<200> doc;
   deserializeJson(doc, payload_str);
   const char* deviceId = doc["deviceId"];
    
   const char* otherInfo = doc["otherInfo"];
   int status=doc["command"]; // | 0;
 /*  
  Serial.print("deviceId: ");    
   Serial.println(  deviceId); 
   Serial.print("otherInfo:"); 
  Serial.println(otherInfo); 
  Serial.print("status:");  
  Serial.println(status);    
   
   //cloud_flag = strdup(save);
   
   Serial.print("light_status:");  
   Serial.println(light_status);  
   Serial.print("msg_in: ");  
   Serial.println(msg_in);  
  */
  light_status = 0; 
  light_status = status; 
  
}
void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    //pubSubClient.subscribe("inTopic");
    pubSubClient.subscribe("web2device");
  }
  pubSubClient.loop();
}

int b64decode(String b64Text, uint8_t* output) {
  base64_decodestate s;
  base64_init_decodestate(&s);
  int cnt = base64_decode_block(b64Text.c_str(), b64Text.length(), (char*)output, &s);
  return cnt;
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}

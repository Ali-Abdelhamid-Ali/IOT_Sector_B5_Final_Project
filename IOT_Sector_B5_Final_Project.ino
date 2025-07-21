#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char ssid[] = "A";
const char pass[] = "12345670";

const char broker[] = "broker.emqx.io";
const int port = 1883;

#define IR_PIN D5 
#define LED_PIN D6
#define TIM_TH 1000

WiFiClient espClient;
PubSubClient client(espClient);

char object[15];
unsigned long last_pup_time = 0;

enum Mode { AUTO, MANUAL_ON, MANUAL_OFF };
Mode currentMode = AUTO;

unsigned long manualStartTime = 0;
const unsigned long MANUAL_DURATION = 500; 

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "/sectorb5/AliAbdelhamid/led") {
    if (message == "on") {
      currentMode = MANUAL_ON;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Manual ON: LED ON");
    } else if (message == "off") {
      currentMode = MANUAL_OFF;
      digitalWrite(LED_PIN, LOW);
      manualStartTime = millis();
      Serial.println("Manual OFF: LED OFF for 5 seconds");
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(IR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  client.setServer(broker, port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("sectorb5_AliAbdelhamid_IR")) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }

  client.subscribe("/sectorb5/AliAbdelhamid/led");
  last_pup_time = millis();
}

void loop() {
  client.loop();

  
  if (currentMode == MANUAL_OFF && millis() - manualStartTime >= MANUAL_DURATION) {
    currentMode = AUTO;
    Serial.println("Returned to AUTO mode after 0.5 sec OFF");
  }

  int ir_value = digitalRead(IR_PIN);  

  if (currentMode == AUTO) {
    if (ir_value == LOW) {
      strcpy(object, "object");
      digitalWrite(LED_PIN, HIGH);
      Serial.println("AUTO: Object detected → LED ON");
    } else {
      strcpy(object, "no object");
      digitalWrite(LED_PIN, LOW);
      Serial.println("AUTO: No object → LED OFF");
    }
  } else if (currentMode == MANUAL_ON) {
    strcpy(object, "manual on");
  } else if (currentMode == MANUAL_OFF) {
    strcpy(object, "manual off");
  }

  if (millis() - last_pup_time >= TIM_TH) {
    client.publish("/sectorb5/AliAbdelhamid/IR", object);
    last_pup_time = millis();
  }
}




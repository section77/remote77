// für ESP32, funktioniert nicht mit ESP8266
#include <WiFi.h>
#include <esp_deep_sleep.h>
#include "Esp.h"
#include <PubSubClient.h>
#include "credentials.h"

const bool dry_run = true;

// external wakeup
#define BUTTON_PIN_BITMASK 0x608000000 // 2^27 + 2^33 + 2^34

// analog in
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

//board
const int ledPin = 2;      
const int wartezeit = 150;
int ledState = HIGH;         // the current state of the output pin

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  // set initial LED state
  digitalWrite(ledPin, ledState);

  setup_wifi();

  // mqtt verbinden
  client.setServer(mqtt_server, 1883);
  if (!client.connected()) {
    reconnect();
  }

  print_wakeup_reason();
    // analog in
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  char buffer[7];         //the ASCII of the integer will be stored in this char array
  itoa(sensorValue,buffer,10); //(integer, yourBuffer, base)
  client.publish(mqtt_pub_topic_batterie, buffer);

  Serial.println("jetzt erst mal schlafen...");
  esp_deep_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(3600000000); // time in µs: 3600 * 1000 * 1000 -> 1h
  esp_deep_sleep_start();
}

void setup_wifi() {

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print(device_name);
  Serial.println(" ist startklar");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clientname, mqtt_user, mqtt_password)) {
      // Once connected, publish an announcement...
      strcat (msg, device_name);
      strcat (msg, " connected");
      Serial.println(msg);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
  }
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }

  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
  if (wakeupBit & GPIO_SEL_27) {
    // GPIO 27 woke up
    // Geraet 1, Funktion 0
    Serial.println("27 war's!");
    if (!dry_run) {
      client.publish(mqtt_pub_topic_geraet_1, "0");
    }
    Serial.println("geraet_1 -> 0");
    for (int i=1; i<3 ; i++) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      delay(wartezeit);
    }
  }

  else if (wakeupBit & GPIO_SEL_33) {
    // GPIO 33 woke up
    // Geraet 1, Funktion 1
    Serial.println("33 war's!");
    if (!dry_run) {
      client.publish(mqtt_pub_topic_geraet_1, "1");
    }
    Serial.println("geraet_1 -> 1");
    for (int i=1; i<5 ; i++) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      delay(wartezeit);
    }
  } 

  else if (wakeupBit & GPIO_SEL_34) {
    // GPIO 34 woke up
    // Licht an
    Serial.println("34 war's!");
    client.publish(mqtt_pub_topic_licht, "1");
    Serial.println("Licht an");
    for (int i=1; i<7 ; i++) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      delay(wartezeit);
    }
  } 
}

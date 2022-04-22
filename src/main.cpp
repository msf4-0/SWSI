#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h> // MQTT client
#include <ArduinoJson.h> // JSON serializer
#include <SPI.h> // SPI Interface
#include "RotaryEncoder.h"
#include "WeighModule.h"
#include "LedRGB.h"
#include "Led.h"


//***** GD2 WEIGHING SCALE ******//
//** Written by : Navaneeth Nair **//
//** Last Modified : 20/4/22
//** Firmware Version 0.9.1 **//

// Parameters settings
// Load Cell

// WiFI & MQTT
const char* id = "gdWS1"; // Unique Scale ID
const char* ssid = "RFID_MSF4.0_2.4";
const char* password = "$HRDC2k19";
const char* mqtt_server = "192.168.0.164";

// PWM output settings
const int ledFreq = 5000;
const int res = 8;
const float brightness = 0.1;
// PWM channels
const int rChan = 9;
const int gChan = 10;
const int bChan = 11;

// Pin Definitions
// ID = RFID, EP = E-paper, HX = HX711 Amplifier, ENC = Rotary Encoder
#define MOSI_SPI 14
#define MISO_SPI 12
#define SCK_SPI 13

#define SS_ID 32
#define RST_ID 33

#define SS_EP 15
#define DC_EP 27
#define RST_EP 26
#define BUSY_EP 25

#define DOUT_HX 19
#define SCK_HX 18

#define SCK_ENC 21
#define DT_ENC 22
#define SW_ENC 23

#define LED1_R 16
#define LED1_G 17
#define LED1_B 5
#define LED2_Y 4

#define BATT 35
#define USB5V 34

// Initialize objects
WiFiClient espClient;
PubSubClient client(espClient);
MFRC522 rfid(SS_ID, RST_ID);
WeighModule module(BATT, USB5V, id, ssid, password, mqtt_server, rfid);
RotaryEncoder knob(SCK_ENC,DT_ENC,SW_ENC);
LedRGB ledrgb (LED1_R, LED1_G, LED1_B, res, ledFreq, rChan, bChan, gChan, brightness);
Led led2(LED2_Y);
GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290::HEIGHT> display(GxEPD2_290_T94(SS_EP, DC_EP, RST_EP, BUSY_EP));

// Interrupts
volatile byte dir = 0;

// Function prototypes
void callback(char* topic, byte* payload, unsigned int length);
void batteryLED();
void reconnect();
void setup_wifi();
void updatePage(int page);
void knob_handler();
void IRAM_ATTR SCK_ISR();
void IRAM_ATTR SW_ISR();

void setup() {
  Serial.begin(115200);
  Serial.println("Initiating");
  module.init(DOUT_HX, SCK_HX);
  module.batteryInit();
  batteryLED();
  // Reinitialize SPI
  SPI.end();
  SPI.begin(SCK_SPI, MISO_SPI, MOSI_SPI);
  display.init(115200); // EPD Init
  display.setRotation(3);  // Set orientation.
  display.setTextWrap(false);
  delay(500);
  // Connect to WiFi
  updatePage(0);
  setup_wifi();
  // Connect to Node-Red
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  updatePage(3);
  reconnect();
  // Localize
  updatePage(4);
  rfid.PCD_Init(); // Initialize MFRC522 RFID Reader
  updatePage(5);
  if (module.localize()) { // Check if card is detected
    ledrgb.color("yellow");
    updatePage(7);
    ledrgb.color("clear");
    delay(500);
    batteryLED();
  }
  else {
    ledrgb.color("red");
    updatePage(6);
    ledrgb.color("clear");
    delay(500);
    batteryLED();
  }
  // Attach knob interrupts
  attachInterrupt(digitalPinToInterrupt(SCK_ENC), SCK_ISR, CHANGE);
  // Change to front page
  module.setPage(1);
  updatePage(0);
}

void loop() {
  if (!client.connected()) reconnect(); // Reconnect to MQTT server 
  client.loop(); // Handle buffer

  module.update();
  batteryLED();
  if (module.isLow()) led2.on();
  else led2.off();
  knob_handler(); // Handle knob controls
  
}

void callback(char* topic, byte* payload, unsigned int length) { // Node-Red Client callback function
  // Update display to major changes
  StaticJsonDocument<256> JsonIn;
  deserializeJson(JsonIn, payload, length);
  ledrgb.color("cyan");
  char tmpItem [15];
  char tmpInfo [15];
  char tmpStation [15];
  strcpy(tmpItem, JsonIn["Item"]);
  strcpy(tmpInfo, JsonIn["Info"]);
  strcpy(tmpStation, JsonIn["Station"]);
  // Update module data
  module.setConv(JsonIn["Conversion"]);
  module.setThres(JsonIn["Threshold"]);
  module.setPoll(JsonIn["PollRate"]);
  module.setSleep(JsonIn["SleepFlag"]);
  module.setSleepTime(JsonIn["SleepCounter"]);

  StaticJsonDocument<256> JsonOut;
  JsonOut["Device"] = module.getID();
  JsonOut["Weight"] = module.getMass();
  JsonOut["Qty"] = module.getQty();
  JsonOut["Battery"] = module.getBatt();
  JsonOut["TagID"] = module.getTag();

  char JSONmessageBuffer[100];
  serializeJson(JsonOut,JSONmessageBuffer);
  JsonOut.clear();
  char pub[20];
  const char* subtopic = "/main";
  strcpy(pub, id);
  strcat(pub, subtopic);
  client.publish(pub, JSONmessageBuffer);
  JSONmessageBuffer[0] = '\0';
  pub[0] = '\0';

  module.setItem(tmpItem);
  module.setInfo(tmpInfo);
  module.setStation(tmpStation);
  if (module.getPage() == 1) updatePage(0);
}

void batteryLED() {
  if (module.readChg()) ledrgb.color("blue");
  else {
    if (module.getBatt() < 20) {
      if (module.getBatt() < 2) ESP.deepSleep(0); // Set to deep sleep when power is low
      ledrgb.color("red");
    }
    else ledrgb.color("green");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect(id)) {
      //module.setPage(0);
      //updatePage(4);
      // Subscribe
      char sub[20];
      const char* subtopic = "/output";
      strcpy(sub, module.getID());
      strcat(sub, subtopic);
      client.subscribe(sub);
      //module.setPage(1);
      //updatePage(0);
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {
  delay(10);
  // Attempt to connect to WiFi network
  WiFi.begin(module.getSSID(), module.getPass());
  updatePage(1);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  updatePage(2);
}

void updatePage(int page) {
  module.updateDisplay(display, page);
}

void knob_handler() {
  const bool sw = knob.isPressed();
  if (dir == 1) {
    updatePage(1);
    delay(50);
    dir = 0;
  }
  else if (dir == 2) {
    updatePage(2);
    delay(50);
    dir = 0;
  }
  else if (sw) updatePage(3);
}

void IRAM_ATTR SCK_ISR() {
  if (digitalRead(SCK_ENC) != digitalRead(DT_ENC)) dir = 1;
  else dir = 2;
}
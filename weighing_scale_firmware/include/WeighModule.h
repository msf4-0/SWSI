#ifndef WEIGHMODULE_H
#define WEIGHMODULE_H

#define ENABLE_GxEPD2_GFX 1 // Enable pointer support for E-paper object

// Declare libraries
#include "HX711.h" // Load Cell Amplifier
#include <Arduino.h> // Arduino standard library
#include <MFRC522.h> // RFID
#include <GxEPD2_BW.h>  // E-paper Black & White Display
// #include <EEPROM.h>
#include <Preferences.h>

// Font libraries
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

class WeighModule {
  private:
    // EEPROM
    byte CALIB_ADDR = 0x01;
    byte MASS_MEM_ADDR = 0x02;
    Preferences preferences;
    // EEPROM eeprom;
    // Battery
    byte battPin;
    byte chgPin;
    float battLevel;
    float batt_buffer;
    byte batt_buffer_cnt;
    const byte batt_buffer_threshold = 20;
    // Scale
    HX711 scale;
    int mass;
    int refillMass;
    float calib_factor = 1331.61;
    float conversion = 2.2;
    int offset;
    int qty;
    float refMass = 100; // Random
    bool calib_opt = 0;
    // Inventory manage
    bool autoOrder = 1;
    int threshold = -2;
    unsigned long lowTime;
    const unsigned long lowTimeout = 10000;
    bool lowLatch = 0;

    // General
    char item [15];
    char info [15];
    char station [15];
    int tag;
    bool sleepFlag;
    unsigned long long sleepTime;
    // RFID
    MFRC522 rfid;
    MFRC522::MIFARE_Key key;
    MFRC522::StatusCode status;
    // EPD
    GxEPD2_GFX* epdPtr;
    byte page;
    unsigned int mainIndex;
    unsigned int scroll;
    float polling_rate = 1;
    unsigned long lastUpt;
    unsigned long menuTime;
    unsigned long dashTime;
    unsigned long calibTime;
    const unsigned long menuTimeout = 25000;
    const unsigned long dashTimeout = 20000;
    const unsigned long calibInt = 1000;
    // General - Fixed
    const char* id;
    const char* ssid;
    const char* password;
    const char* mqtt_server;

  public:
    // Constructor
    WeighModule(byte bat, byte chg, const char* id, const char* ssid, const char* pass, const char* mqtt, MFRC522 &rfid);

    // Getters
    int getMass();
    float getBatt();
    float getCalib();
    int getQty();
    const char* getID();
    const char* getSSID();
    const char* getPass();
    const char* getMQTT();
    int getTag();
    byte getPage();

    // Setters
    void setCalib(float calib);
    void setQty(int qty);
    void setItem(const char* item);
    void setInfo(const char* info);
    void setStation(const char* station);
    void setPage(byte page);
    void setConv(float conv);
    void setThres(int thres);
    void setPoll(float poll);
    void setSleep(bool sleepFlag);
    void setSleepTime(unsigned long sleepTime);

    // Functions
    void init(byte dout, byte sck);
    // Battery
    float readBatt();
    int readChg();
    void batteryInit();
    void batteryStat();
    // Scale
    void calibScale();
    void readScale();
    bool isLow();
    // RFID
    bool localize();
    bool readRFID();
    // E-paper
    void clear(GxEPD2_GFX& disp, bool full=0);
    void updateDisplay(GxEPD2_GFX& disp, int state);
    void update();
};
#endif

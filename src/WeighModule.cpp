#include "WeighModule.h"

// Constructor
WeighModule::WeighModule(byte bat, byte chg, const char* id, const char* ssid, const char* pass, const char* mqtt, MFRC522 &rfid) {
  this->battPin = bat;
  this->chgPin = chg;
  this->id = id;
  this->ssid = ssid;
  this->password = pass;
  this->mqtt_server = mqtt;
  this->rfid = rfid;


  pinMode(battPin, INPUT);
  pinMode(chgPin, INPUT);
  strcpy(item, "Init in");
  strcpy(info,"Node-Red!");
  strcpy(station,"No allocation");
  tag = 1;
  mass = 0.0;
  qty = 0;
}

// Getter methods
float WeighModule::getBatt() {
  return this->battLevel;
}
int WeighModule::getMass() {
  return this->mass;
}
float WeighModule::getCalib() {
  return this->calib_factor;
}
int WeighModule::getQty() {
  return this->qty;
}
int WeighModule::getTag() {
  return this->tag;
}
const char* WeighModule::getID() {
  return this->id;
}
const char* WeighModule::getSSID() {
  return this->ssid;
}
const char* WeighModule::getPass() {
  return this->password;
}
const char* WeighModule::getMQTT() {
  return this->mqtt_server;
}
byte WeighModule::getPage() {
  return this->page;
}

// Setter methods
void WeighModule::setCalib(float calib) {
  this->calib_factor = calib;
}
void WeighModule::setQty(int qty) {
  this->qty = qty;
}
void WeighModule::setItem(const char* item) {
  strcpy(this->item, item);
}
void WeighModule::setInfo(const char* info) {
  strcpy(this->info, info);
}
void WeighModule::setStation(const char* station) {
  strcpy(this->station, station);

}
void WeighModule::setPage(byte page) {
  this->page = page;
}
void WeighModule::setConv(float conv) {
  this->conversion = conv;
}
void WeighModule::setThres(int thres) {
  this->threshold = thres;
}
void WeighModule::setPoll(float poll) {
  if (poll < 2.5) this->polling_rate = 0.5;
  else this->polling_rate = poll - 2;
}

void WeighModule::setSleep(bool sleepFlag) {
  this->sleepFlag = sleepFlag;
}
void WeighModule::setSleepTime(unsigned long sleepTime) {
  this->sleepTime = sleepTime;
}


// Functions
void WeighModule::init(byte dout, byte sck) {
  preferences.begin("scale",false);

  if (preferences.getFloat("calib",0) == 0) {
    preferences.putFloat("calib",calib_factor);
  }
  else calib_factor = preferences.getFloat("calib",0);
  
  if (preferences.getFloat("offset",0) == 0) {
    offset = 0;
  }
  else offset = preferences.getFloat("offset",0);
  preferences.end();
  scale.begin(dout, sck);
  // scale.tare();
  scale.set_offset(offset);
  scale.set_scale(calib_factor);
  // Serial.println(scale.get_offset());
}

float WeighModule::readBatt() {
  return analogRead(battPin);
}

int WeighModule::readChg() {
  return digitalRead(chgPin);
}

bool WeighModule::isLow() {
  return lowLatch;
}

void WeighModule::batteryInit() {
  for (byte i = 0; i < 5; i++ ) {
    battLevel += map(readBatt(), 3100.0, 4095.0, 0, 100);
  }
  battLevel /= 5;
}

void WeighModule::batteryStat() {
  const float battRaw = map(readBatt(), 3100.0, 4095.0, 0, 100);
  batt_buffer += battRaw;
  batt_buffer_cnt++;

  if (batt_buffer_cnt == batt_buffer_threshold) {
    battLevel = batt_buffer/batt_buffer_threshold;
    batt_buffer_cnt = 0;
    batt_buffer = 0;
  }
}

void WeighModule::readScale() {
  const float masstmp = scale.get_units(10);
  mass = (int) masstmp;
  if (mass < 0) mass = 0;
  qty = mass/conversion;
}

bool WeighModule::localize() {
  bool successRead = 0;
  const unsigned long init_time = millis();
  unsigned long elapsed = 0;
  const unsigned long rfid_timeout = 5000;
  while (!successRead) {
    successRead = readRFID();
    elapsed = millis() - init_time;

    if (elapsed > rfid_timeout)
      return 0;
  }
  return 1;
}

bool WeighModule::readRFID() { // get RFID
  // Getting ready for Reading PICCs
  if ( ! rfid.PICC_IsNewCardPresent())  //If a new PICC placed to RFID reader continue
    return 0;
  if ( ! rfid.PICC_ReadCardSerial())  //Since a PICC placed get Serial and continue
    return 0;

  // String tagID = "";
  // char tagStr[32];
  byte readCard[4];
  tag = 0;
  for ( uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    readCard[i] = rfid.uid.uidByte[i];
    tag += readCard[i];
    //tagID.concat(String(rfid.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  // tagID.toUpperCase();
  // tagID.toCharArray(tagStr, 10);
  rfid.PICC_HaltA(); // Stop reading
  return 1;
}

void WeighModule::update() {
  if (sleepFlag == 1) {
    // preferences.begin("scale",false);
    // preferences.putFloat("offset",offset);
    // preferences.end();
    esp_sleep_enable_timer_wakeup(1000*sleepTime + 180000000);
    esp_deep_sleep_start();
  }
  batteryStat();
  //if (!autoOrder) lowTime = millis();
  switch(page) {
    case 1:
    {
      // if (millis() - dashTime > dashTimeout) {
      //   updateDisplay(*epdPtr, 0);
      //   dashTime = millis();
      // }
      // else if (millis() - lastUpt > polling_rate*1000) {
      if (millis() - lastUpt > polling_rate*1000) {
        
        scale.power_up();
        readScale();
        updateDisplay(*epdPtr, 4);
        if (lowLatch == 0) {
          if (autoOrder == 0) lowTime = millis();
          if (qty < threshold) {
            if (millis() - lowTime < lowTimeout) {
              qty = threshold + 1;
              mass = qty*conversion;
            }
            else {
              lowLatch = 1;
            }
          }
          else lowTime = millis();
        }
        else {
          if (qty > threshold) {
            mass = refillMass;
            qty = mass/conversion;
          }
          else refillMass = mass;
        }
        lastUpt = millis();
        scale.power_down();
      }
    }
    break;
    case 3:
    {
      if (millis() - calibTime > calibInt) updateDisplay(*epdPtr, 4);
    }
    break;
    default:
    {
      if (millis() - menuTime > menuTimeout) {
        page = 1;
        updateDisplay(*epdPtr , 0);
        scroll = 0;
        mainIndex = 0;
      }
    }
    break;
  }
}


void WeighModule::updateDisplay(GxEPD2_GFX& disp, int state) {
  disp.setTextColor(GxEPD_BLACK);  // Set color for text
  disp.setFont(&FreeMono9pt7b);
  epdPtr = &disp;
  switch(page) {
    case 0: // Init page
    {
      switch(state) {
        case 0:
          {
            disp.setFont(&FreeMonoBold12pt7b);  // Set font
            disp.setFullWindow();
            disp.firstPage();
            do
            {
              disp.fillScreen(GxEPD_WHITE);  // Clear previous graphics to start over to print new things.
              disp.setCursor(10, 20);  // Set the position to start printing text (x,y)
              disp.println("Initializing");  // Print some texth
            }
            while (disp.nextPage());
          }
          break;

        case 1:
        {
          disp.setPartialWindow(10,35,300,200);
          disp.firstPage();
          do
          {
            disp.fillScreen(GxEPD_WHITE);
            disp.setFont(&FreeMonoBold12pt7b);
            disp.setCursor(10,50);
            disp.println("Connecting to WiFi:");
            disp.setCursor(10, 70);
            disp.setFont(&FreeMonoBold9pt7b);
            disp.println(ssid);
          }
          while (disp.nextPage());

          delay(1000);
        }
            break;
        case 2:
        {
          disp.setPartialWindow(10,35,300,15);
          disp.setFont(&FreeMonoBold12pt7b);
          clear(disp);
          disp.firstPage();
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(10,50);
            disp.println("Connected!:");
          }
          while (disp.nextPage());

          delay(1000);
        }
        break;

        case 3:
        {
          disp.setPartialWindow(5,35,300,50);
          disp.setFont(&FreeMonoBold12pt7b);
          clear(disp);
          disp.firstPage();
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(5,50);
            disp.println("Connecting 2 Node-Red");
          }
          while (disp.nextPage());

          delay(1000);
        }
            break;
        case 4:
        {
          disp.setPartialWindow(5,35,300,50);
          disp.setFont(&FreeMonoBold12pt7b);
          clear(disp);
          disp.firstPage();
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(5,50);
            disp.println("Connected 2 Node-Red");
          }
          while (disp.nextPage());
        }
        break;

        case 5:
        {
          disp.setPartialWindow(5,35,300,50);
          disp.setFont(&FreeMonoBold12pt7b);
          clear(disp);
          disp.firstPage();
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(5,50);
            disp.println("Localizing...");
          }
          while (disp.nextPage());
        }
        break;
        case 6:
        {
          disp.setPartialWindow(5,35,300,50);
          disp.setFont(&FreeMonoBold12pt7b);
          clear(disp);
          disp.firstPage();
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(5,50);
            disp.println("No tag!");
          }
          while (disp.nextPage());
        }
        break;
        case 7:
        {
          disp.setPartialWindow(5,35,300,50);
          disp.setFont(&FreeMonoBold12pt7b);
          clear(disp);
          disp.firstPage();
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(5,50);
            disp.println("Localized!");
          }
          while (disp.nextPage());
        }
        break;
      }
    }
    break;
    case 1: // Dashboard
    {
      switch(state) {
        case 0:
        {
          disp.setFullWindow();
          disp.setFont(&FreeMonoBold12pt7b);
          disp.firstPage();
          scale.power_up();
          readScale();

          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(10, 20);
            disp.println(item);

            disp.setCursor(10, 50);
            disp.println(info);

            disp.setCursor(10, 90);
            disp.println(mass);

            disp.setCursor(90, 90);
            disp.println("g");

            disp.setCursor(10, 120);
            disp.println(qty);
            disp.setCursor(80, 120);
            disp.println("pcs");

            disp.setFont(&FreeMonoBold9pt7b);
            disp.setCursor(145, 50);
            disp.println(station);

            disp.setFont(&FreeMonoBold12pt7b);
            disp.setCursor(145, 90);
            disp.println("Status:");

            disp.setCursor(260, 90);
            if (lowLatch == 1) disp.println("Lo");
            else disp.println("Ok");

            // Draw line:
            disp.drawLine(140,10, 140,110,   GxEPD_BLACK);  // Draw line (x0,y0,x1,y1,color)

            disp.drawRect(247, 9, 3, 12, GxEPD_BLACK);
            disp.drawRect(250, 5, 40, 20, GxEPD_BLACK);

            if (battLevel > 75) disp.fillRect(253, 7, 10, 16, GxEPD_BLACK);
            if (battLevel > 40) disp.fillRect(265, 7, 10, 16, GxEPD_BLACK);
            if (battLevel > 5) disp.fillRect(277, 7, 10, 16, GxEPD_BLACK);
            disp.setCursor(170, 20);
            disp.println((int) battLevel);
            disp.setCursor(230, 20);
            disp.println("%");
          }
          while (disp.nextPage());
          lastUpt = millis();
          dashTime = millis();
        }
          break;
        case 1: // CW
          break;
        case 2: //CCW
          break;
        case 3: // SW
        {
          page = 2;
          updateDisplay(disp, 0);
          scale.power_up();
        }
          break;
        case 4: // Update dashboard
        {
          disp.setPartialWindow(10,60,60,70);
          disp.setFont(&FreeMonoBold12pt7b);
          disp.firstPage();
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.setCursor(10,90);
            disp.println(mass);
            disp.setCursor(10, 120);
            disp.println(qty);

          }
          while (disp.nextPage());
        }
        break;
        case 5: // Update low status
        {
          disp.setPartialWindow(265, 85, 25, 30);
          disp.setFont(&FreeMonoBold12pt7b);
          disp.firstPage();
          do {
            disp.setCursor(270, 90);
            if (lowLatch) disp.println("Lo");
            else disp.println("Ok");
          }
          while (disp.nextPage());
        }
      }
    }
    break;
    case 2: // Main Menu
    {
      if (mainIndex > 3) scroll = 1;
      else scroll = 0;
      int offset;
      switch(state) {
        case 0: // First print
        {
          disp.setFullWindow();
          disp.setFont(&FreeMonoBold12pt7b);
          disp.firstPage();
          if (scroll == 0) {
            do {
              disp.fillScreen(GxEPD_WHITE);
              disp.setCursor(20, 30);
              disp.println("Zero Scale");

              disp.setCursor(20, 60);
              disp.println("Auto Order");

              disp.setCursor(220, 60);
              if (autoOrder) disp.println("A/ ");
              else disp.println(" /M");

              disp.setCursor(20, 90);
              disp.println("Order");

              disp.setCursor(20, 120);
              disp.println("Restock");

              if (mainIndex == 3) disp.fillTriangle(5,125, 15,115, 5,105, GxEPD_BLACK);
              else disp.fillTriangle(5,35, 15,25, 5,15, GxEPD_BLACK);

            }
            while (disp.nextPage());
          }
          else {
            do {
              disp.fillScreen(GxEPD_WHITE);
              disp.setCursor(20, 30);
              disp.println("Relocalize");

              disp.setCursor(20, 60);
              disp.println("Calibrate Scale");

              disp.setCursor(20, 90);
              disp.println("Back");

              if (mainIndex == 6) disp.fillTriangle(5,95, 15,85, 5,75, GxEPD_BLACK);
              else disp.fillTriangle(5,35, 15,25, 5,15, GxEPD_BLACK);

            }
            while (disp.nextPage());
          }
        }
          break;

        case 1: // CW
        {
          if (mainIndex < 6) mainIndex += 1;
          else mainIndex = 0;
          if ((scroll == 0 && mainIndex > 3) || (scroll == 1 && mainIndex < 4)) {
            updateDisplay(disp, 0);
            break;
          }
          disp.setPartialWindow(5,5,15,120);
          disp.firstPage();
          offset = (mainIndex - 4*scroll)*30;
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.fillTriangle(5,35 + offset, 15,25 + offset, 5,15 + offset, GxEPD_BLACK);
          }
          while (disp.nextPage());
        }
        break;

        case 2: // CCW
        {
          if (mainIndex > 0) mainIndex -= 1;
          else mainIndex = 6;
          if ((scroll == 0 && mainIndex > 3) || (scroll == 1 && mainIndex < 4)) {
            updateDisplay(disp, 0);
            break;
          }
          disp.setPartialWindow(5,5,15,120);
          disp.firstPage();
          offset = (mainIndex - 4*scroll)*30;
          do {
            disp.fillScreen(GxEPD_WHITE);
            disp.fillTriangle(5,35 + offset, 15,25 + offset, 5,15 + offset, GxEPD_BLACK);
          }
          while (disp.nextPage());
        }
        break;

        case 3: // SW
        {
          switch (mainIndex) {
            case 0: // Zero Scale
            {
              scale.tare();
              offset = scale.get_offset();
              preferences.begin("scale",false);
              preferences.putFloat("offset",offset);
              preferences.end();
              disp.setPartialWindow(250,10,50,30);
              disp.setFont(&FreeMonoBold12pt7b);
              disp.firstPage();
              do {
                disp.drawLine(250,20,255,30,GxEPD_BLACK);
                disp.drawLine(255,30,290,10,GxEPD_BLACK);
              }
              while (disp.nextPage());
              delay(500);
              clear(disp);
            }
            break;
            case 1: // Auto Order A/M
            {
              autoOrder = !autoOrder;
              disp.setPartialWindow(210,45,60,30);
              disp.setFont(&FreeMonoBold12pt7b);
              disp.firstPage();
              do {
                disp.fillScreen(GxEPD_WHITE);
                disp.setCursor(220, 60);
                if (autoOrder) disp.println("A/ ");
                else disp.println(" /M");
              }
              while (disp.nextPage());
            }
            break;
            case 2:  // Manual Order
            {
              if (lowLatch == 0) {
                if (qty < threshold) lowLatch = 1;
              }
            }
            break;
            case 3: // Restock
            {
              readScale();
              if (qty > threshold && lowLatch == 1) {
                lowLatch = 0;
                disp.setPartialWindow(250,90,50,30);
                disp.setFont(&FreeMonoBold12pt7b);
                disp.firstPage();
                do {
                  disp.drawLine( 250, 100, 255, 110, GxEPD_BLACK );
                  disp.drawLine( 255, 110, 290, 90, GxEPD_BLACK );
                }
                while (disp.nextPage());
                delay(500);
                clear(disp);
              }
              else {
                disp.setPartialWindow(250,90,50,30);
                disp.setFont(&FreeMonoBold12pt7b);
                disp.firstPage();
                do {
                  disp.drawLine(255, 90, 290, 110, GxEPD_BLACK);
                  disp.drawLine(255, 110, 290, 90, GxEPD_BLACK);
                }
                while (disp.nextPage());
                delay(500);
                clear(disp);
              }
            }
            break;
            case 4: // Relocalize
            {
              const bool isLoc = localize();
              if (isLoc) {
                disp.setPartialWindow(250,10,50,30);
                disp.setFont(&FreeMonoBold12pt7b);
                disp.firstPage();
                do {
                  disp.drawLine(250,20,255,30,GxEPD_BLACK);
                  disp.drawLine(255,30,290,10,GxEPD_BLACK);
                }
                while (disp.nextPage());
                delay(500);
                clear(disp);
              }
              else {
                disp.setPartialWindow(250,10,50,30);
                disp.setFont(&FreeMonoBold12pt7b);
                disp.firstPage();
                do {
                  disp.drawLine(255,10,290,30,GxEPD_BLACK);
                  disp.drawLine(255,30,290,10,GxEPD_BLACK);
                }
                while (disp.nextPage());
                delay(500);
                clear(disp);
              }
            }
            break;
            case 5: // Calibrate Scale
            {
              page = 3;
              calibTime = millis();
              updateDisplay(disp, 0);
            }
            break;
            case 6: // Back
            {
              page = 1;
              updateDisplay(disp, 0);
              scroll = 0;
              mainIndex = 0;
            }
            break;
          }
        }
        break;
      }
      menuTime = millis();
    }
    break;
    case 3: // Sub options - Calibrate scale
      {
        switch (state) {
          case 0: // Init
          {
            disp.setFullWindow();
            disp.setFont(&FreeMono12pt7b);
            disp.firstPage();
              do {
                disp.fillScreen(GxEPD_WHITE);
                disp.setCursor(10, 20);
                disp.println("Ref mass:");

                disp.setCursor(170, 20);
                disp.println(refMass);
                disp.setCursor(260, 20);
                disp.println("g");

                disp.setCursor(20, 70);
                disp.println("Cur mass: ");
                disp.setCursor(180, 70);
                disp.println(mass);

                disp.setCursor(20, 120);
                disp.println("Back");
                disp.setCursor(180, 120);
                disp.println("Confirm");
                disp.fillTriangle(105,125, 100,115, 105,105, GxEPD_BLACK);
              }
              while (disp.nextPage());
          }
          break;
          case 1: // CW
          {
            calib_opt = !calib_opt;
            disp.setPartialWindow(100, 90, 70, 35);
            disp.setFont(&FreeMono12pt7b);
            disp.firstPage();
            do {
              disp.fillScreen(GxEPD_WHITE);
              if (calib_opt == 0) disp.fillTriangle(105,125, 100,115, 105,105, GxEPD_BLACK);
              else disp.fillTriangle(150,125, 155,115, 150,105, GxEPD_BLACK);
            }
            while (disp.nextPage());
          }
          break;
          case 2: // CCW
          {
            calib_opt = !calib_opt;
            disp.setPartialWindow(100, 90, 70, 35);
            disp.setFont(&FreeMono12pt7b);
            disp.firstPage();
            do {
              disp.fillScreen(GxEPD_WHITE);
              if (calib_opt == 0) disp.fillTriangle(105,125, 100,115, 105,105, GxEPD_BLACK);
              else disp.fillTriangle(150,125, 155,115, 150,105, GxEPD_BLACK);
            }
            while (disp.nextPage());
          }
          break;
          case 3: // SW
          {
            if (calib_opt == 1) {
              scale.set_scale(scale.get_units(10)/refMass);
              calib_factor = scale.get_scale();
              preferences.begin("scale",false);
              preferences.putFloat("calib",calib_factor);
              preferences.end();
            }
            page = 2;
            updateDisplay(disp, 0);
          }
          break;
          case 4: // Update Mass
          {
            disp.setPartialWindow(170, 60, 50, 30);
            disp.setFont(&FreeMono12pt7b);
            disp.firstPage();
            readScale();
            do {
              disp.fillScreen(GxEPD_WHITE);
              disp.setCursor(180, 70);
              disp.println(mass);
            }
            while (disp.nextPage());
            calibTime = millis();
          }
        }
      }
      break;
  }

}

void WeighModule::clear(GxEPD2_GFX& disp, bool full) {
  if (full) disp.setPartialWindow(0,0,290,150);
  disp.firstPage();
  do {
    disp.fillScreen(GxEPD_BLACK);
  }
  while (disp.nextPage());

  disp.firstPage();
  do {
    disp.fillScreen(GxEPD_WHITE);
  }
  while (disp.nextPage());

}

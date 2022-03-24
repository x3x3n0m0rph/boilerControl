#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <math.h>
#include "defines.h"
//#include "microsmooth/microsmooth.h"
//#include "microsmooth/microsmooth.cpp"

typedef enum {
  GSM_OK, 
  GSM_CME,
  GSM_ERR,
  GSM_TIMEOUT,
  GSM_UNDEF
} GSM_ANSWER_STATUS;

struct GSM_ANSWER {
  byte status;
  String data;
};

LiquidCrystal_I2C         lcd(0x27, 16, 2);
SoftwareSerial            gsm(5, 6);

void setup() {
  Serial.begin(9600);
  
  initPins();
  initLCD();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("INIT...");

  
  gsm.begin(9600);
  lcd.setCursor(2, 1);
  lcd.print("UART OK");
  sleepMillis(500);
  GSM_reset();
  lcd.setCursor(2, 1);
  lcd.print("GSM RESET...");
  lcd.setCursor(2, 1);
  
  sleepMillis(GSM_CMD_TIMEOUT*2);
  if(GSM_ready()) {
    lcd.print("GSM INIT OK ");
  }
  else {
    lcd.print("GSM INIT FAIL");
  }
  sleepMillis(500);
}

void loop() {
  
  updateStatus();
  redrawLCD();

  if(timing > 150) {
      buildJson();
      auto _seq = seq;
      do {
        _seq = seq;
        seq = sendSequence(seq);
      } while(seq - _seq == 10 && seq != TX_SEQ_REQ_OK);
      if(seq == TX_SEQ_REQ_OK) {
        seq = TX_SEQ_NULL;
        timing = 0;
      }
  }
  sleepMillis(REFRESH_TIMEOUT);
  timing++;
  

  
}

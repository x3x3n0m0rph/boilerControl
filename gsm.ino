
inline bool GSM_ready() {
  return GSM_command_OK("AT");
}

uint8_t GSM_registration() {
  auto res = GSM_command("AT+CGREG?", "+CGREG: ");
  if (res.status == GSM_OK) {
    return res.data.charAt(res.data.indexOf(",") + 1);
  }
  else return 4;
}

int8_t GSM_signal_level() {
  auto res = GSM_command("AT+CSQ", "+CSQ: ");
  if (res.status == GSM_OK) {
    return res.data.substring(0, res.data.indexOf(",")).toInt();
  }
  else return 99;
}

bool GSM_GPRS_setup() {
  return GSM_command_OK(GSM_GPRS_SETUP_1) && GSM_command_OK(GSM_GPRS_SETUP_2);
}

bool GSM_GPRS_init() {
  return GSM_command_OK(GSM_GPRS_SETUP_3);
}

bool GSM_GPRS_ready() {
  auto res = GSM_command("AT+SAPBR=2,1", "+SAPBR: ");
  if (res.status == GSM_OK) {
    return res.data.charAt(res.data.indexOf(",") + 1) == '1';
  }
  else return false;
}

void GSM_reset() {
  digitalWrite(DPIN_SIM800RST, HIGH);
  delay(200);
  digitalWrite(DPIN_SIM800RST, LOW);
  seq = TX_SEQ_NULL;
}

inline GSM_ANSWER GSM_command(char cmd) {
  return GSM_command(cmd, GSM_CMD_TIMEOUT);
}
inline GSM_ANSWER GSM_command(const char* cmd, long timeout) {
  return __GSM_command(cmd, timeout, "OK");
}

inline GSM_ANSWER GSM_command(const char* cmd, const char* awaitFor) {
  return __GSM_command(cmd, GSM_CMD_TIMEOUT, awaitFor);
}

inline bool GSM_command_OK(const char* cmd) {
  return GSM_command(cmd, GSM_CMD_TIMEOUT).status == GSM_OK;
}

inline bool GSM_command_OK(const char* cmd, long timeout) {
  return GSM_command(cmd, timeout).status == GSM_OK;
}

GSM_ANSWER __GSM_command(const char*  cmd, long timeout, const char* awaitFor) {
  gsm.println(cmd);
  Serial.println(cmd);
  String answer = "";
  GSM_ANSWER _answer;
  uint64_t _timeout = millis() + timeout;

  while (millis() < _timeout) {
    while (!gsm.available() && millis() < _timeout) {};
    if (gsm.available()) {
      answer = gsm.readString();
      if (answer.endsWith("OK\r\n")) {
        _answer.status = GSM_OK;
        if (awaitFor == "OK") return _answer;
      }
      else if (answer.endsWith("ERROR\r\n")) {
        _answer.status = GSM_ERR;
      }
      else if (answer.indexOf("+CME ERROR: ") > -1) {
        _answer.status = GSM_CME;
        _answer.data = answer.substring(answer.indexOf("+CME ERROR: " + 12));
        return _answer;
      }
      else {
        _answer.status = GSM_UNDEF;
      }

      int ind = answer.indexOf(awaitFor);
      if (ind > -1) {
        _answer.status = GSM_OK;
        _answer.data = answer.substring(ind + strlen(awaitFor));
        return _answer;
      }
      else {
        continue;
      }
    }
  }
  _answer.status = GSM_TIMEOUT;
  Serial.println(_answer.data);
  return _answer;
}

uint8_t sendSequence(uint8_t state) {
  sleepMillis(750);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("TRANSMISSION..");
  switch (state) {
    case TX_SEQ_NULL:
      if (GSM_ready()) {
          lcd.setCursor(1, 1);
          lcd.print("SIM READY");
        return TX_SEQ_AT_OK;
      }
      else return TX_SEQ_NULL;
      break;
      
    case TX_SEQ_AT_OK:
      if (GSM_signal_level() != 99 && GSM_registration() == '1') {
          lcd.setCursor(1, 1);
          lcd.print("GSM OK");
        return TX_SEQ_SIGNAL_OK;
      }
      else {
          lcd.setCursor(1, 1);
          lcd.print("GSM FAIL");
        return TX_SEQ_AT_OK;
      }
      break;
      
    case TX_SEQ_SIGNAL_OK:
      if (GSM_GPRS_ready()) {
          lcd.setCursor(1, 0);
          lcd.print("GPRS READY");
        return TX_SEQ_GPRS_OK;
      }
      if (!GSM_GPRS_setup()) {
          lcd.setCursor(1, 1);
          lcd.print("GPRS FAIL");
        return TX_SEQ_SIGNAL_OK;
      }
      lcd.setCursor(1, 1);
      lcd.print("GPRS INIT...");
      for (int i = 0; i < 10; i++) {
        if (GSM_GPRS_init()) {

          break;
        }
        else sleepMillis(GSM_GPRS_TIMEOUT);
      }
      GSMGPRS = GSM_GPRS_ready();
      if(GSMGPRS) {
          lcd.setCursor(1, 1);
          lcd.print("            ");
          lcd.setCursor(1, 1);
          lcd.print("GPRS OK");
        return TX_SEQ_GPRS_OK;
      }
      else {
          lcd.setCursor(1, 1);
          lcd.print("GPRS FAIL");
        return TX_SEQ_SIGNAL_OK;
      }
    break;
      
    case TX_SEQ_GPRS_OK:
      if (!GSM_command_OK(GSM_HTTP_INIT)) {
        GSM_command_OK(GSM_HTTP_TERM);
                  lcd.setCursor(1, 1);
          lcd.print("HTTP FAIL");
        return TX_SEQ_GPRS_OK;
      }
          lcd.setCursor(1, 1);
          lcd.print("HTTP OK");
      if (GSM_command_OK(json_data.c_str(), 20000)) {
        lcd.setCursor(1, 1);
        lcd.print("HTTP INIT OK");
        return TX_SEQ_SESS_OK;
      }
      else {
        lcd.setCursor(1, 1);
        lcd.print("HTTP INIT FAIL");
        GSM_command_OK(GSM_HTTP_TERM);
        return TX_SEQ_GPRS_OK;
      }
      break;
      
    case TX_SEQ_SESS_OK:
      for (int i = 0; i < 5; i++) {

        auto response = __GSM_command(GSM_HTTP_SUBMIT, 30000, "+HTTPACTION: ");
        if(response.status == GSM_OK) {
          String code = response.data.substring(response.data.indexOf(",")+1, 5);
          lcd.setCursor(1, 1);
          lcd.print("RESULT: ");
          lcd.setCursor(10, 1);
          if (code == "202") {
            GSM_command_OK(GSM_HTTP_TERM);
            lcd.print("OK");
            return TX_SEQ_REQ_OK;
          }
          else if (code == "601") {
             GSM_reset();
             return TX_SEQ_NULL;
          }
          else lcd.print(code);
        }
      }
      //failed 5 times
      GSM_command_OK(GSM_HTTP_TERM);
      return TX_SEQ_GPRS_OK;
      break;
    default:
      return state;
      break;
  }
}

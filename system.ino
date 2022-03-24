inline void initPins() {
  pinMode(DPIN_SIM800RST, OUTPUT);
  pinMode(DPIN_POWER_AC, INPUT_PULLUP);
  pinMode(DPIN_IGN, INPUT_PULLUP);
  pinMode(DPIN_PR_CLK, OUTPUT);
  pinMode(DPIN_PR_SHLD, OUTPUT);
  pinMode(DPIN_PR_QH, INPUT);

  pinMode(APIN_BATTERY, INPUT);
  pinMode(APIN_TEMP_WATER, INPUT);
  pinMode(APIN_TEMP_AIR, INPUT);

  digitalWrite(APIN_BATTERY, LOW);
  digitalWrite(APIN_TEMP_WATER, LOW);
  digitalWrite(APIN_TEMP_AIR, LOW);
}

int medianRead(uint8_t pin, int times) {
  long readings = 0;
  
  for(int i = 0; i < times; i++) 
    readings += analogRead(pin);
  return readings / times;
}

void updateStatus() {
  __flame = digitalRead(DPIN_IGN) == LOW;
  bool __power_act = digitalRead(DPIN_POWER_AC) == LOW; 
  if(__power_act != __power_ac) timing = 1000;
  __power_ac = __power_act;
  __t_water = readTemp(APIN_TEMP_WATER); //formula
  sleepMillis(10);

  __t_air = readTemp(APIN_TEMP_AIR); //formula
  sleepMillis(10);

  __u_batt = (readRealVoltage(APIN_BATTERY) / DC_VM_BATT_R2) * (DC_VM_BATT_R1 + DC_VM_BATT_R2); //formula
  sleepMillis(10);

  __p_water = 0; ///ALGORITHM

  GSMSignalLevel = -113 + GSM_signal_level() * 2;
  GSMRegLevel = GSM_registration();
  if(GSMGPRS) GSMGPRS = GSM_GPRS_ready();
}

float readRealVoltage(uint8_t pin) {
  return (AREF_VOLT / AREF_STEPS) * medianRead(pin, 3);
}

float readTemp(uint8_t pin) {
  float real_u = readRealVoltage(pin);
  float term_r_ln = log((TERM_PULLD_RES * (AREF_VOLT - real_u)) / real_u);
  return (1 / (TERM_A + TERM_B * term_r_ln + TERM_C * pow(term_r_ln, 3))) - TERM_K_DELTA;
}

void sleepMillisOrEvent(uint64_t timout, bool(*expr)()) {
  uint64_t _timeout = millis() + timout;
  while (millis() < _timeout && !expr()) {};
}

void sleepMillis(uint64_t timout) {
  sleepMillisOrEvent(timout, []() {return false;} );
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void buildJson() {
  //{"dev":"144","wt":1.0,"at":1.0,"wp":1.0,"ub":1.0,"ac":1,"db":1,"ign":1}
  json_data = GSM_HTTP_REQUEST;
  json_data.concat(___DEVICE_UID); //fix to compile time
  json_data.concat("%22,%22wt%22:");
  json_data.concat(s__t_water);
  json_data.concat(",%22at%22:");
  json_data.concat(s__t_air);
  json_data.concat(",%22wp%22:");
  json_data.concat(s__p_water);
  json_data.concat(",%22ub%22:");
  json_data.concat(s__u_batt);
  json_data.concat(",%22ac%22:");
  json_data.concat(__power_ac ? "1" : "0");
  json_data.concat(",%22db%22:");
  json_data.concat(GSMSignalLevel);
  json_data.concat(",%22ign%22:");
  json_data.concat(__flame ? "1" : "0");
  json_data.concat("}\"");
}

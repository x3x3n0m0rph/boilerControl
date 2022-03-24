
inline void initLCD() {
  lcd.init();
  lcd.backlight();
}

void redrawLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  s__t_water = String(__t_water, 1);
  if (__t_water < 0.0) lcd.print("LOW");
  else if (__t_water >= 100.0) lcd.print("HIGH");
  else lcd.print(s__t_water);

  lcd.setCursor(5, 0);
  s__p_water = String(__p_water, 1);
  if (__p_water < 0.0) lcd.print("LOW");
  else if (__p_water >= 10.0) lcd.print("HIGH");
  else lcd.print(s__p_water);

  lcd.setCursor(9, 0);
  if (__power_ac) lcd.print("220");
  else lcd.print(" NO");

  lcd.setCursor(13, 0);
  if (__flame) lcd.print("IGN");
  else lcd.print("   ");

  lcd.setCursor(0, 1);
  s__t_air = String(__t_air, 1);
  if (__t_air < 0.0) lcd.print("LOW");
  else if (__t_air >= 100.0) lcd.print("HIGH");
  else lcd.print(s__t_air);

  lcd.setCursor(5, 1);
  s__u_batt = String(__u_batt, 1);
  if (__u_batt < 0.0) lcd.print("LOW");
  else if (__u_batt >= 100.0) lcd.print("HIGH");
  else lcd.print(s__u_batt);

  lcd.setCursor(10, 1);
  if (GSMSignalLevel > 0) lcd.print("ERR");
  else lcd.print(GSMSignalLevel);

  lcd.setCursor(14, 1);
  if (GSMGPRS) lcd.print('G');
  else lcd.print(' ');

  lcd.setCursor(15, 1);
  switch (GSMRegLevel) {
    case '1': //home
      lcd.print('H');
      break;
    case '2': //searchin
      lcd.print('S');
      break;
    case '5': //roaming
      lcd.print('R');
      break;
    default: //error
      lcd.print('E');
      break;
  }
}

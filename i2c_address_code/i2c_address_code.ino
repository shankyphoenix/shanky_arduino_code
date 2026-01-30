//sketch created by Akshay Joseph
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{

  lcd.begin();


  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Ryan sdfg sdfgsdfg sdfgdsfg sdfg ");
}

void loop()
{
    lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Ryan ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Jordon ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Jeston");
  delay(1000);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Shanky");
  delay(1000);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Rianna");
  delay(1000);
}

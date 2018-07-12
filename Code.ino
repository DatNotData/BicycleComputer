#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define HALL_PIN 2 // pin of the magnetic (hall effect) sensor
unsigned long hallPrevOn = 0; // last time the sensor was reading ON

#define  WHEEL_CIRCUMFERENCE 0.0020447 // circumference of wheel in km
double speed = 0; // speep in km/h
double distance = 0; // distance trvelled\

void updateLcdInfo() {
  // display current speed
  if (speed < 1000) {
    lcd.setCursor(2, 0);
    if (speed < 100)
      lcd.print('0');
    if (speed < 10)
      lcd.print('0');
    lcd.print(speed, 3);
    lcd.print(" km/h");
  }

  // display distance travelled
  if (distance < 1000) {
    lcd.setCursor(0, 1);
    int distanceDigit =  distance < 1 ? 4 : 4 - int(log10(distance));
    lcd.print(distance, distanceDigit - 1);
    lcd.print("km");
  }

  // display time
  unsigned long milliseconds = millis();
  if (milliseconds < 86400000) {
    lcd.setCursor(8, 1);
    int hours = int(milliseconds / 3600000);
    if (hours < 10)
      lcd.print('0');
    lcd.print(hours);
    lcd.print(':');

    int minutes = (milliseconds - (hours * 3600000)) / 60000;
    if (minutes < 10)
      lcd.print('0');
    lcd.print(minutes);
    lcd.print(":");

    int seconds = (milliseconds - (minutes * 60000)) / 1000;
    if (seconds < 10)
      lcd.print('0');
    lcd.print(seconds);
  }
}

void interruptRoutine() {
  unsigned long hallPrevOnDif = millis() - hallPrevOn;
  hallPrevOn = millis();

  speed = WHEEL_CIRCUMFERENCE / hallPrevOnDif * 3600000;
  distance += WHEEL_CIRCUMFERENCE;
}

void setup() {
  lcd.begin();
  lcd.backlight();

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), interruptRoutine, FALLING);
}

void loop() {
  updateLcdInfo();
}

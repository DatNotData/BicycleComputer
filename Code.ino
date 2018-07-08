#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define HALL_PIN A2 // pin of the magnetic (hall effect) sensor
bool hallState = false; // state of the magnetic sensor
unsigned long hallPrevOn = 0; // last time the sensor was reading ON
bool hallPrevState = false; // last state of the sensor

#define  WHEEL_CIRCUMFERENCE 0.002 // circumference of wheel in km
double speed = 0; // speep in km/h
double distance = 0; // distance trvelled\

bool hallRead() {
  int iter = 5; // number of values to read
  int sum = 0;
  for (int i = 0; i < iter; i++) {
    sum += analogRead(HALL_PIN) < 100;
  }
  return (sum / iter) > 0.5;
}

void updateLcdInfo() {
  // display current speed
  lcd.setCursor(2, 0);
  if (speed < 100)
    lcd.print('0');
  if (speed < 10)
    lcd.print('0');
  lcd.print(speed, 3);
  lcd.print(" km/h ");

  lcd.setCursor(0, 1);
  // display distance travelled
  int distanceDigit =  distance < 1 ? 4 : 4 - int(log10(distance));
  lcd.print(distance, distanceDigit - 1);
  lcd.print("km ");

  // display time
  unsigned long milliseconds = millis();
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

void setup() {
  lcd.begin();
  lcd.backlight();

  Serial.begin(9600);
}

void loop() {
  hallPrevState = hallState;
  hallState = hallRead();

  if (hallState == true && hallPrevState == false) {
    unsigned long hallPrevOnDif = millis() - hallPrevOn;
    hallPrevOn = millis();

    speed = WHEEL_CIRCUMFERENCE / hallPrevOnDif * 3600000;
    distance += WHEEL_CIRCUMFERENCE;
  }

  updateLcdInfo();
}

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define HALL_PIN A2 // pin of the magnetic (hall effect) sensor
bool hallState = false; // state of the magnetic sensor
unsigned long hallPrevOn = 0; // last time the sensor was reading ON
bool hallPrevState = false; // last state of the sensor
unsigned long hallPrevOnDif = 0; // difference between the previous on time and now

float wheelC = 2; // circumference of wheel in meters
double speed = 0; // speep in km/h
double distance = 0; // distance trvelled\

#define DIGIT_SPEED 3
#define DIGIT_DISTANCE 4
#define DIGIT_MINUTES 3

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
  lcd.setCursor(0, 0);
  int speedDigit = speed < 1 ? DIGIT_SPEED - 1 : DIGIT_SPEED - log10(speed);
  lcd.print(speed, speedDigit);
  lcd.print("km/h ");

  int distanceDigit = distance < 1 ? DIGIT_DISTANCE - 1 : DIGIT_DISTANCE - log10(distance);
  lcd.print(distance, distanceDigit);
  lcd.print("km");

  // display distance travelled
  lcd.setCursor(0, 1);
  int minutes = round(millis() / 60000);
  for (int i = 0; i < (minutes < 10 ? DIGIT_MINUTES - 1 : DIGIT_MINUTES - (log10(minutes) + 1)); i++)
    lcd.print('0');
  lcd.print(minutes);
  lcd.print("m");

  // display time in minutes and seconds only
  // no hours, minutes dont "reset" at each hour
  //int seconds = (millis()-(minutes*60000))/1000;
  int seconds = 0;
  if (seconds < 10)
    lcd.print('0');
  lcd.print(seconds);
  lcd.print("s ");
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
    hallPrevOnDif = millis() - hallPrevOn;
    hallPrevOn = millis();

    speed = wheelC / hallPrevOnDif * 3600;
    distance += wheelC / 1000;
  }

  updateLcdInfo();
}

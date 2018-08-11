#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


/*

    A3144 hall effect sensor pinout

          -------------
          \-----------/
           |    |    |
          5V   GND  DOUT
           |         |
           |<10k ohm>|
                ^
       add pull up resistor

*/

#define HALL_PIN 2            // pin of the magnetic (hall effect) sensor
unsigned long hallPrevOn = 0; // last time the sensor was reading ON
unsigned long timeDelta = 0;  // current time - previous time the sensor was reading ON

#define  WHEEL_CIRCUMFERENCE 0.0020447 // circumference of wheel in km
#define HALL_TIMEOUT 7200              // time out before the displayed speed is reset to 0 (in millis)
double distance = 0;                   // distance trvelled

bool updateRequest = false; // this variable will be set to TRUE when the lcd and speed value needs to be updated

void setup() {
  lcd.begin();
  lcd.backlight();

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(HALL_PIN), magnetEventRoutine, FALLING);

  // set timer interrupt to request screen refresh
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 31250; // frequency : 16 000 000 Hz / 256 / framerate Hz
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void loop() {
  if (updateRequest)
    update();
}

void magnetEventRoutine() { // routine when the magnet pass
  unsigned long currentMillis = millis();
  timeDelta = currentMillis - hallPrevOn;
  hallPrevOn = currentMillis;
  distance += WHEEL_CIRCUMFERENCE; // add to distance
}

void update() {
  unsigned long milliseconds = millis();

  double speed = 0; // current speed

  // display current speed
  if (milliseconds - hallPrevOn < 7200 && timeDelta != 0) // check timeout and time delta
    speed = WHEEL_CIRCUMFERENCE / timeDelta * 3600000;

  lcd.setCursor(2, 0);
  if (speed < 100)
    lcd.print('0');
  if (speed < 10)
    lcd.print('0');
  lcd.print(speed, 3);
  lcd.print(" km/h");

  // display distance travelled
  lcd.setCursor(0, 1);
  int distanceDigit =  distance < 1 ? 4 : 4 - int(log10(distance));
  lcd.print(distance, distanceDigit - 1);
  lcd.print("km");

  // display time
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

  updateRequest = false;
}

ISR(TIMER1_COMPA_vect) { // refresh screen and speed
  updateRequest = true;
}

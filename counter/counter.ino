
int maxPeople = 5; // maximum number of people allowed before the alarm goes off
int sensitivity = 5; //lower values will make it more sensitive and higher values will make it less sensitive
//---------------------------------------------------

#include <LiquidCrystal_I2C.h>                                                         
#include <Wire.h>
#include <NewPing.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
int lastLCD = millis();

int currentPeople = 0;

int buzzer = 8;

//int sensor1[] = {2, 3};
//int sensor2[] = {10, 11};
NewPing sensor[2] = {
  NewPing(3, 2, 130),
  NewPing(11, 10, 130)
};
float sensor1Initial;
float sensor2Initial;

String msg = "";
char inbyte;
String sequence = "";
int timeoutCounter = 0;
bool processing = false;

void setup() {
  //Setup code
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(buzzer, OUTPUT);
  delay(500);

  sensor1Initial = 0;
  sensor2Initial = 0;
  for (int i = 0; i < 10; i++) {
    sensor1Initial += sensor[0].ping_cm();
    delay(100);
    sensor2Initial += sensor[1].ping_cm();
    delay(100);
  }
  sensor1Initial /= 10;
  sensor2Initial /= 10;
  lcd.setCursor(0,0);
  lcd.print("Testing");
  lcd.setCursor(0,1);
  lcd.print("Initial 1: ");
  lcd.print(sensor1Initial);
  lcd.setCursor(0,2);
  lcd.print("Initial 2: ");
  lcd.print(sensor2Initial);
  delay(2000);
  lcd.clear();
}

void loop() {
  msg = "";
  if (Serial.available()) {
    inbyte = Serial.read();
    msg += inbyte;
    delay(300);
  }
  while (Serial.available()) {
    inbyte = Serial.read();
    msg += inbyte;
  }
  if (msg == "query") {
    Serial.print("Current number of people: ");
    Serial.println(currentPeople);
  }
  
  //Read ultrasonic sensors
  float sensor1Val = sensor[0].ping_cm();
  float sensor2Val = sensor[1].ping_cm();

  //Process the data
//  if(processing) {
//    if (sensor1Val >= sensor1Initial - 50 && sequence.charAt(2) != '1') {
//      sequence += "1";
//    } 
//    if (sensor2Val >= sensor2Initial - 50 && sequence.charAt(2) != '2') {
//      sequence += "2";
//    }
//  }
//  else {
    if (sensor1Val < sensor1Initial - 50 && sequence.charAt(0) != '1') {
      sequence += "1";
    }
    if (sensor2Val < sensor2Initial - 50 && sequence.charAt(0) != '2') {
      sequence += "2";
    }
//  }

  lcd.setCursor(0, 0);
  lcd.print("Seq: ");
  lcd.print(sequence);
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.print("Current People: ");
  lcd.print(currentPeople);
  lcd.setCursor(0, 2);
  lcd.print("Processing: ");
  lcd.print(processing);
  lcd.setCursor(0,3);
  lcd.print("1: ");
  lcd.print(sensor1Val);
  lcd.print(" 2: ");
  lcd.print(sensor2Val);
  lcd.print("  ");
  
  if (sequence.length() >= 2) {
    if (sequence.equals("12")) {
      currentPeople++;
      while(sensor2Val < sensor2Initial - 50);
    } else if (sequence.equals("21") && currentPeople > 0) {
      currentPeople--;
      while(sensor1Val < sensor1Initial - 50);
    }
    sequence = "";
  }
  
  //Resets the sequence if it is invalid or timeouts
  if (sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 200) {
    sequence = "";
  }
  if (sequence.length() == 1) { //
    timeoutCounter++;
  } else {
    timeoutCounter = 0;
  }

  //If the number of people is too high, trigger the buzzer
//  if (currentPeople > maxPeople) {
//    tone(buzzer, 1700);
//  } else {
//    noTone(buzzer);
//  }

}

//Returns the distance of the ultrasonic sensor that is passed in
//a[0] = echo, a[1] = trig
int measureDistance(int a[]) {
  pinMode(a[1], OUTPUT);
  digitalWrite(a[1], LOW);
  delayMicroseconds(2);
  digitalWrite(a[1], HIGH);
  delayMicroseconds(10);
  digitalWrite(a[1], LOW);
  pinMode(a[0], INPUT);
  long duration = pulseIn(a[0], HIGH, 100000);
  return duration / 29 / 2;
}

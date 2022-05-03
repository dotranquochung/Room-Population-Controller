
int maxPeople; // maximum number of people allowed before the alarm goes off
int sensitivity = 5; //lower values will make it more sensitive and higher values will make it less sensitive
//---------------------------------------------------

#include <LiquidCrystal_I2C.h>                                                         
#include <Wire.h>
//#include <NewPing.h>
#include "pitches.h"

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
int lastLCD = millis();

int currentPeople = 0;

float area;
float areaPerPerson;
String areaString;
String capString;
//char *charBuffer;

int buzzer = 8;

int sensor1[] = {2, 3};
int sensor2[] = {9, 10};
//NewPing sensor[2] = {
//  NewPing(3, 2, 130),
//  NewPing(10, 9, 130)
//};
float sensor1Initial;
float sensor2Initial;

String msg = "";
char inbyte;
String sequence = "";
int timeoutCounter = 0;
unsigned long lastEntered;
unsigned long done;

void setup() {
  //Setup code
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(buzzer, OUTPUT);
  delay(500);

  sensor1Initial = 0;
  sensor2Initial = 0;
//  for (int i = 0; i < 10; i++) {
//    sensor1Initial += sensor[0].ping_cm();
//    delay(100);
//    sensor2Initial += sensor[1].ping_cm();
//    delay(100);
//  }
//  sensor1Initial /= 10;
//  sensor2Initial /= 10;
  lcd.setCursor(0,0);
  lcd.print("Testing");
//  lcd.setCursor(0,1);
//  lcd.print("Initial 1: ");
//  lcd.print(sensor1Initial);
//  lcd.setCursor(0,2);
//  lcd.print("Initial 2: ");
//  lcd.print(sensor2Initial);
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
  else if (msg.substring(0,4) == "area") {
    areaString = msg.substring(5);
//    *charBuffer = (char *)malloc(areaString.length() * sizeof(char));
//    areaString.toCharArray(*charBuffer, areaString.length() + 1);
//    area = atof(*charBuffer);
    area = areaString.toFloat();
    Serial.println(area);
    if (area != 0 && areaPerPerson != 0) {
      maxPeople = (int)(area/areaPerPerson);
      Serial.println(maxPeople);
    }
  }
  else if (msg.substring(0,3) == "cap") {
    capString = msg.substring(4);
//    *charBuffer = (char *)malloc(capString.length() * sizeof(char));
//    capString.toCharArray(*charBuffer, capString.length() + 1);
//    areaPerPerson = atof(*charBuffer);
    areaPerPerson = capString.toFloat();
    Serial.println(areaPerPerson);
    if (area != 0 && areaPerPerson != 0) {
      maxPeople = (int)(area/areaPerPerson);
      Serial.println(maxPeople);
    }
  }
  
  //Read ultrasonic sensors
  float sensor1Val = measureDistance(sensor1);
  float sensor2Val = measureDistance(sensor2);

  if (millis() - done >= 600) {
    if (sensor1Val < 50 && sequence.charAt(0) != '1') {
      sequence += "1";
      lastEntered = millis();
    }
    if (sensor2Val < 50 && sequence.charAt(0) != '2') {
      sequence += "2";
      lastEntered = millis();
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Seq: ");
  lcd.print(sequence);
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.print("Current People: ");
  lcd.print(currentPeople);
  lcd.setCursor(0, 2);
  lcd.print("1: ");
  lcd.print(sensor1Val);
  lcd.setCursor(0, 3);
  lcd.print(" 2: ");
  lcd.print(sensor2Val);
  lcd.print("  ");
  
  if (sequence.length() >= 2) {
    if (sequence.equals("12")) {
      currentPeople++;
      getIn();
    } else if (sequence.equals("21") && currentPeople > 0) {
      currentPeople--;
      getOut();
    }
    done = millis();
    sequence = "";
  }

  if(millis() - lastEntered > 700)
    sequence = "";
  
  //Resets the sequence if it is invalid or timeouts
  if (sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 200) {
    sequence = "";
  }
  if (sequence.length() >= 1) { //
    timeoutCounter++;
  } else {
    timeoutCounter = 0;
  }

  //If the number of people is too high, trigger the buzzer
  if (maxPeople > 0 && currentPeople > maxPeople) {
    tone(buzzer, 1700);
  } else {
    noTone(buzzer);
  }

}

void getIn() {
  tone(buzzer, NOTE_E4, 250);
  delay(250*1.3);
  tone(buzzer, NOTE_C4, 250);
  delay(250*1.3);
  noTone(buzzer);
}

void getOut() {
  tone(buzzer, NOTE_C4, 250);
  delay(250*1.3);
  tone(buzzer, NOTE_E4, 250);
  delay(250*1.3);
  noTone(buzzer);
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

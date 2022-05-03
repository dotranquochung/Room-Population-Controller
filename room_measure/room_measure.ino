
/*!
 * @File  DFRobot_Iraser_Sensor.ino
 * @brief  In this example, the infrared laser ranging sensor is used to measure the distance, and the sensor data is processed to obtain the measured distance
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence  The MIT License (MIT)
 * @author  [liunian](nian.liu@dfrobot.com)
 * @version  V1.0
 * @date  2020-08-13
 */
#include <SoftwareSerial.h>
#include <Servo.h>
SoftwareSerial mySerial(9,10);
char buff[4]={0x80,0x06,0x03,0x77};
unsigned char data[11]={0};

Servo myservo;
int pos = 0;    // variable to store the servo position

String msg = "";
char inbyte;

float side_1 = 0;
float side_2 = 0;
float side_3 = 0;

float area = 0;
void setup()
{
 myservo.attach(5);  // attaches the servo on pin 9 to the servo object
 backToDefault();
 Serial.begin(115200);
 mySerial.begin(9600);
}

void loop()
{
  msg = "";
  side_1 = 0;
  side_2 = 0;
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
    backToDefault();
    side_1 = measure();
    turnleft();
    side_2 = measure();
    backToDefault();
    Serial.println("Side 1: " + String(side_1));
    Serial.println("Side 2: " + String(side_2));
    area = side_1 * side_2;
    Serial.println("Area: " + String(area));
  }
}

void turnleft() {
  for (int i = 0; i < 8; i++) {
    myservo.write(0);
    delay(250);
  }
}


void backToDefault() {
  for (int i = 0; i < 8; i++) {
    myservo.write(105);
    delay(250);
  }
}


float measure() {
  float distance=0;
  int iter = 0;
  mySerial.print(buff);
  while(iter < 20)
  {
    if(mySerial.available()>0)//Determine whether there is data to read on the serial 
    {
      delay(50);
      for(int i=0;i<11;i++)
      {
        data[i]=mySerial.read();
      }
      unsigned char Check=0;
      for(int i=0;i<10;i++)
      {
        Check=Check+data[i];
      }
      Check=~Check+1;
      if(data[10]==Check)
      {
        if(data[3]=='E'&&data[4]=='R'&&data[5]=='R')
        {
          Serial.println("Out of range");
        }
        else
        {
          distance=0;
          distance=(data[3]-0x30)*100+(data[4]-0x30)*10+(data[5]-0x30)*1+(data[7]-0x30)*0.1+(data[8]-0x30)*0.01+(data[9]-0x30)*0.001;
          Serial.print("Distance = ");
          Serial.print(distance,3);
          Serial.println(" M");
        }
      }
      else
      {
        Serial.println("Invalid Data!");
      }
    }
    delay(20);
    iter++;
  }
  return distance;
}

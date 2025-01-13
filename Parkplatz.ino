#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>
#include <Servo.h>


Servo mein_servo;
const int amountBays = 4;
int occupied = 0;
int change;
int lastIter[amountBays] = {1}; // Sensor values
long timer;
const String firstLine = "Freie Parkzone";
String displayText;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int t1            = 2000;
const int steueradresse = 11;
const int sensorEntrance = 2; //pinmode
const int sensorExit = 3; //pinmode
int sensors[amountBays] = {4,5,6,7}; // Sensor addresses/ pins
const int lights = 8;


void setup() {
  //pinMode(taster, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  mein_servo.attach(steueradresse);
  pinMode(13,OUTPUT); // ToDo Schrankensteuerung
  pinMode(sensorEntrance,INPUT);
  for (int pin:sensors){
    pinMode(pin,INPUT);
  }
  pinMode(lights, OUTPUT);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  displayText = "";
  occupied = 0;

  /*Check Parking bays*/
  for (int i = 0 ; i < amountBays; i++) {
    change = lastIter[i] - digitalRead(sensors[i]); // 1: wird belegt, -1: wird frei
    if (change == -1){
      digitalWrite(lights, HIGH);//To Do lights on, wenn parkplatz frei lighter ein. 
      timer = millis();
    }
    //Fetch free Bays
    occupied = digitalRead(sensors[i]);
    switch(occupied){
      case 0:
        // Parkplatz belegt.
        occupied++;
        break;
      case 1:
        // Parkplatz frei.
        displayText += char(i+49);// 49 ist ein eins
        displayText += ", ";
        break;
    }
    lastIter[i] = digitalRead(sensors[i]);
  }

  /*Entrance und Exit control*/
  if ((!digitalRead(sensorEntrance) && (amountBays-occupied > 0)) || !digitalRead(sensorExit))// Fahrzeug ist Da! es wird aufgezählt!
  {
    mein_servo.write(90);// ToDo Schrankensteuerung
    timer = millis();
    digitalWrite(lights, HIGH);// To do :Light on
  }
  else{
    mein_servo.write(0); // toDO Schrankensteuerung
  }

  /*LCD Text*/
  if (amountBays - occupied == 0){
    firstLine = "Nicht Frei";
  }
  lcd.print(firstLine);
  lcd.setCursor(0,1);// Cursor auf 2. zeile 
  lcd.print(displayText);

  /*time-of for Lighting*/
  if (millis() - timer > 10*1000){//licht bleibt 10sec an
    digitalWrite(lights, LOW);//To do: light off
  }
  delay(1000); 
}

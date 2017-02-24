/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */
#include <EEPROM.h>
#include "Tlc5940.h"

int addr = 0;
int speedMin = 5;//00
int speedStep = 5;
int speedMax = 100;
int currentSpeed = 0;
int portIndex = 0;
int moveMent = 0;// none = 0, left to right = 1, right to left= 2, pattern = 3
byte powerToggle = LOW;

byte states[] = { LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW };
String  inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{

   //Tlc.clear(); Tlc.update();delay(75);

  Serial.begin(9600);
  Serial1.begin(9600);
  readStates();

  if(currentSpeed == 0 || currentSpeed == 255){
    currentSpeed = speedMin;
    writeSpeed();
  }

  if(moveMent > 3 || moveMent == 255){
    moveMent = 0; 
    writemoveMent();
  }
  
  Tlc.init();
 
}
void writeStates()
{
  for(int i=0;i<12;i++)
  {
    EEPROM.update(i, states[i]);
    delay(100);
  }
  // speed
   EEPROM.update(12, currentSpeed);
   delay(100);
  // moment
   EEPROM.update(13, moveMent); 
   delay(100);
   
}
void readStates()
{
 for(int i=0;i<12;i++)
  {
     states[i] = EEPROM.read(i);
     states[i] == 255 ? HIGH : LOW;
    delay(100);
  }
  // speed
   currentSpeed = EEPROM.read(12);
   delay(100);
  // moment
   moveMent = EEPROM.read(13); 
   delay(100);
   powerToggle = LOW;
}
void printMem()
{
  for(int i = 0; i< 14;i++)
  {
    byte value = EEPROM.read(i);

    Serial.print(i);
    Serial.print("\t");
    Serial.print(value, DEC);
    Serial.println();

  }
}
void allOn()
{
      for(int i = 0;i< 12;i++){
          states[i] = HIGH;
          //digitalWrite(ports[i], states[i]);
          Tlc.set(i, 4095);
      }  
      Tlc.update();delay(75);
      moveMent = 0;
      writeStates();
}
void allOff()
{
      for(int i = 0;i< 12;i++){
          states[i] = LOW;
         // digitalWrite(ports[i], states[i]);
          Tlc.set(i, 0);
      }  
      Tlc.update();delay(75);
       writeStates();
}
void restSpeed()
{
    currentSpeed = speedMin;
}
void incrementSpeed(){

    if(currentSpeed < speedMax)
      currentSpeed += speedStep;

    writeSpeed();
    Serial.write("speed:");
    Serial.println(currentSpeed, DEC);
}
void decrementSpeed(){

    if(currentSpeed > speedMin)
      currentSpeed -= speedStep;

    writeSpeed();
    Serial.write("speed:");
    Serial.println(currentSpeed, DEC);
}
void writeSpeed()
{
  EEPROM.update(12, currentSpeed);
}
void writemoveMent()
{
  EEPROM.update(13, moveMent);
}
void loop() {

  serialEvent(); //call the function
  // print the string when a newline arrives:
  if (stringComplete) {

    inputString.trim();
    if(inputString != "4294967295")
      Serial.println(inputString);
    
    // clear the string:
    //if(inputString=="553617527")
    //  toggleState(0);
      
    inputString = "";
    stringComplete = false;
  }
  movement();
}
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  //stringComplete = true;
}
void incrementMovement()
{

  moveMent++;
  moveMent = moveMent > 3 ? 0 : moveMent;

  writemoveMent();
  
  Serial.write("moveMent:");
  Serial.println(moveMent, DEC);
}
void movement()
{
    
  if(moveMent > 0)
  {
    Serial.write("moveMent:");
    Serial.println(moveMent, DEC);
    Serial.write("speed:");
    Serial.println(currentSpeed, DEC);
    Serial.write("port:");
    if(moveMent == 1)
    {
      portIndex++;
      portIndex = portIndex > 11 ? 0 : portIndex;
    }
    if(moveMent == 2)
    {
      portIndex--;
      portIndex = portIndex < 0 ? 11 : portIndex;
    }
    
    delay(currentSpeed*100);
  }
  
}
void toggleState(int idx)
{

  states[idx] = states[idx] == HIGH ? LOW : HIGH;
  EEPROM.update(idx, states[idx]);
 // digitalWrite(ports[idx], states[idx]);
  Tlc.set(idx, states[idx] == HIGH ? 4095 : 0);
  Tlc.update();delay(75);

  //Serial.print(idx);
  Serial.println("toggleState");
}

void handle(int recv)
{
     
     switch(recv)
     {
        case -30601:// 1
          Serial.println("1");
           toggleState(0);
          break;
        case 18615://2
        Serial.println("2");
         toggleState(1);
        break;
        case -14281://3
        Serial.println("3");
         toggleState(2);
        break;
        case 10455://4
        Serial.println("4");
         toggleState(3);
        break;
        case -22441://5
        Serial.println("5");
         toggleState(4);
        break;
        case 26775://6
        Serial.println("6");
         toggleState(5);
        break;
        case -6121://7
        Serial.println("7");
         toggleState(6);
        break;
        case 6375://8
        Serial.println("8");
         toggleState(7);
        break;
        case -26521://9
        Serial.println("9");
         toggleState(8);
        break;
        case 30855://refresh
        Serial.println("refresh");
         toggleState(9);
        break;
        case 2295://0
        Serial.println("0");
         toggleState(10);
        break;
        case -28561:// fav
        Serial.println("favoriet");
         toggleState(11);
              break;               
        case 22695:
          Serial.println("menu");
        break;
        case 14535:
         Serial.println("Exit");
        break;
        case -10201:
        Serial.println("Epg");
        break;
        case -18361:
        Serial.println("Info");
         printMem();
        break;        
        case -2041:
        Serial.println("OK");
          moveMent = 0;       
        break;
        case 255:
        Serial.println("UP");
            incrementMovement();
        break;
        case 16575:
        Serial.println("RIGHT");
        break;
        case -32641:
        Serial.println("DOWN");
        break;
        case -16321:
        Serial.println("LEFT");
        break;
        case -5611:
        Serial.println("VOL UP");
        decrementSpeed();
        break;
        case 6885:
        Serial.println("VOL DOWN");
        incrementSpeed();
        
        break;
        case 8415:
        Serial.println("music");
        break;
        case 31365:
        Serial.println("sat");
        break;
        case -21931:
        Serial.println("Pup");
              decrementSpeed();
        break;
        case 27285:
        Serial.println("Pdown");
         incrementSpeed();
 
        break;
        case 20655:
           Serial.println("Power");
           if(powerToggle == LOW)
           {
                powerToggle = HIGH;
                for(int i = 0;i< 12;i++){
                   // states[i] = LOW;
                   moveMent = 0;
                   currentSpeed = speedMin;
                   Tlc.clear();
                }     
                Tlc.update();     delay(75);        
           }
           else
           {
              readStates();
              
           }
          // allOn();
        break;        
        case 12495:
           Serial.println("no speaker");
           allOn();
        break;
        default:
          if(recv != -1)
            Serial.println(recv);   

         }
     }



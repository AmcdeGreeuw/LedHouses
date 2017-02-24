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

    Tlc.init();
   Tlc.clear(); Tlc.update();delay(75);

  Serial.begin(9600);
  readStates();

  if(currentSpeed == 0 || currentSpeed == 255){
    currentSpeed = speedMin;
    writeSpeed();
  }

  if(moveMent > 3 || moveMent == 255){
    moveMent = 0; 
    writemoveMent();
  }
  
  
 
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
/*
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
*/
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

    uint32_t value = inputString.toInt();
    
    if(inputString != "4294967295"){
      Serial.println(inputString);
      handle(value);
    }
    
    // clear the string:
    //if(inputString=="553617527")
    //  toggleState(0);
      
    inputString = "";
    stringComplete = false;
  }
  //movement();
}
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
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

  if(idx < 12)
  {
      states[idx] = states[idx] == HIGH ? LOW : HIGH;
      EEPROM.update(idx, states[idx]);
      Tlc.set(idx, states[idx] == HIGH ? 4095 : 0);
      Tlc.update();delay(75);
  }
  else{
      
      
  }
  Serial.print("toggleState:");  Serial.println(idx);
}

void handle(uint32_t recv)
{     
     switch(recv)
     {
        case  553617527:  toggleState(0);break; //  1 
        case  553601207:  toggleState(1);break; //  2
        case  553633847:  toggleState(2);break; //  3
        case  553593047:  toggleState(3);break; //  4
        case  553625687:  toggleState(4);break; //  5
        case  553609367:  toggleState(5);break; //  6
        case  553642007:  toggleState(6);break; //  7
        case  553588967:  toggleState(7);break; //  8 
        case  553621607:  toggleState(8);break; //  9
        case  553613447:  toggleState(9);break; //  refresh
        case  553584887:  toggleState(10);break; // 0
        case  553619567:  toggleState(11);break; // FAV
        case  553605287:  toggleState(12);break; // menu
        case  553597127:  toggleState(13);break; // exit
        case  553637927:  toggleState(14);break; // epg
        case  553629767:  toggleState(15);break; // i
        case  553582847:  toggleState(16);break; // navUP
        case  553599167:  toggleState(17);break; // navRight
        case  553615487:  toggleState(18);break; // navDown
        case  553631807:  toggleState(19);break; // nacLeft
        case  553646087:  toggleState(20);break; // navOK
        case  553642517:  toggleState(21);break; // V+
        case  553589477:  toggleState(22);break; // V-
        case  553591007:  toggleState(23);break; // tel/muz
        case  553613957:  toggleState(24);break; // sat
        case  553626197:  toggleState(25);break; // P+
        case  553609877:  toggleState(26);break; // P-
        case  553635887:  toggleState(27);break; // F1
        case  553627727:  toggleState(28);break; // F2
        case  553611407:  toggleState(29);break; // F3
        case  553644047:  toggleState(30);break; // F4
        case  553623647:  toggleState(31);break; // vol?
        case  553639967:  toggleState(32);break; // align_bottom
        case  553640477:  toggleState(33);break; // text
        case  553607327:  toggleState(34);break; // pauze
        case  553632317:  toggleState(35);break; // UHF
        case  553586927:  toggleState(36);break; // sat
        case  553583357:  toggleState(37);break; // M1
        case  553599677:  toggleState(38);break; // N/P
        case  553591517:  toggleState(39);break; // Timer
        case  553603247:  toggleState(40);break; // POWER
        case  553595087:  toggleState(41);break; // Mute
        default:
            Serial.println(recv);   

         }
         
}



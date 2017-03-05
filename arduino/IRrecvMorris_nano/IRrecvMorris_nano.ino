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
int speedMin = 1;//00
int speedStep = 5;
int speedMax = 255;
byte maxluminous = 256;
int currentSpeed = 0;
int portIndex = 0;
int moveMent = 0;// none = 0, left to right = 1, right to left= 2, pattern = 3
byte powerToggle = LOW;
byte luminous = 4; // off 
int currentLuminous = 2048;  //50%
byte const speedaddress = 16;
byte const luminousaddress = 18;
byte const movementaddress = 17;
uint32_t lastvalue = 0;
boolean binit = true;
byte repeat = 0;
byte repeatmax = 2;

byte states[] = { LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW };
String  inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{

  Tlc.init();
  Tlc.clear(); Tlc.update();delay(75);

  Serial.begin(9600);
  startTest();
  
  readStates();

//defaults
  if(currentSpeed == 0 || currentSpeed == 255){
    currentSpeed = speedMin;
    writeSpeed();
  }

  if(moveMent > 4 || moveMent == 255){
    moveMent = 0; 
    writemoveMent();
  }
 // end defaults
 binit = false; 
}



void loop() {

  serialEvent(); //call the function
  // print the string when a newline arrives:
  if (stringComplete) {

    inputString.trim();

    uint32_t value = inputString.toInt();
    
    if(inputString != "4294967295"){
       repeat = 0;
     // Serial.println(inputString);
      lastvalue = value;
      handle(value);
    }
    else
    {
      repeat++;
      if(repeat==repeatmax)
      {
         repeat = 0;
         handle(lastvalue);
      }
      
    }
          
    inputString = "";
    stringComplete = false;
  }
  movement();
}

void writeStates()
{
  for(int i=0;i<16;i++)
  {
    EEPROM.update(i, states[i]);
    delay(50);
  }

   EEPROM.update(speedaddress, currentSpeed);
   delay(50);

   EEPROM.update(movementaddress, moveMent); 
   delay(50);

   EEPROM.update(luminousaddress, luminous); 
   delay(50);
   
}
void readStates()
{
 for(int i=0;i<16;i++)
  {
     states[i] = EEPROM.read(i);
     states[i] == 255 ? HIGH : LOW;
    delay(50);
  }

   currentSpeed = EEPROM.read(speedaddress);
   delay(50);
   
   moveMent = EEPROM.read(movementaddress); 
   delay(50);

   luminous = EEPROM.read(luminousaddress); 
   delay(50);
   
   setLuminous();
   powerToggle = LOW;
}

void setLuminous()
{
   currentLuminous = (luminous * 16) -1;
        for(int i = 0;i< 16;i++){
          if(states[i] == HIGH)
            Tlc.set(i, currentLuminous);
          else
            Tlc.set(i, 0);
      }  
      Tlc.update();delay(75);
}
void powerBlink(){

  if(binit) return;
  allOn(4095);
  delay(200);
  clearLeds();
  delay(200);
  allOn(4095);
  delay(200);
  clearLeds();
  delay(200);
 readStates();
  
}


void startTest(){

  allOn(1095);
  return;
  allOn(4095);
  delay(1000);
    allOn(3095);
  delay(1000);
    allOn(2095);
  delay(1000);
    allOn(1095);
  delay(1000);
    allOn(595);
  delay(1000);
    allOn(4095);
  delay(1000);
    allOn(4095);
  delay(1000);
    allOn(4095);
  delay(1000);
  clearLeds();
}

void toggleAllOn()
{
    int counthigh = 0;
    // check what dominates 
    for(int i=0;i<16;i++)
    {
      if(states[i]==HIGH)
        counthigh++;
    }

    if(counthigh > 8)
      allOn();
     else
      allOff();
     
}
void allOn(int v)
{
  for(int i=0;i<16;i++){
      Tlc.set(i, v);     
  }
   Tlc.update();delay(75);

}
void clearLeds()
{
   Tlc.clear(); Tlc.update();delay(75);
}
void togglePower()
{
    powerToggle = powerToggle == LOW ? HIGH : LOW;
    if(powerToggle==HIGH)
    {
      moveMent = 0;
      clearLeds();
    }
    else
    {
      readStates();
    } 
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
      for(int i = 0;i< 16;i++){
          states[i] = HIGH;
          Tlc.set(i, currentLuminous);
      }  
      Tlc.update();delay(75);
      writeStates();
}
void allOff()
{
      for(int i = 0;i< 16;i++){
          states[i] = LOW;
          Tlc.set(i, 0);
      }  
      Tlc.update();delay(75);
       writeStates();
}
void resetSpeed()
{
    currentSpeed = speedMin;
}
void incrementSpeed(){

    currentSpeed -= speedStep;
    if(currentSpeed < 0)
    {
      currentSpeed = speedMin;
    }

    writeSpeed();
    movement();
   // Serial.write("speed:");
   // Serial.println(currentSpeed, DEC);
}

void decrementSpeed(){

    currentSpeed += speedStep;
    if(currentSpeed > speedMax)
    {
      currentSpeed = speedMax;
    }
    writeSpeed();
    movement();
   // Serial.write("speed:");
   // Serial.println(currentSpeed, DEC);
}
void incrementLuminous()
{

    luminous++;
    if(luminous>255){
      luminous=255;
    }

    writeLuminous();
    setLuminous();
  
}
void decrementLuminous()
{

    luminous--;
    if(luminous<1)
    {
      luminous=1;
    }
    writeLuminous();
    setLuminous();
  
}
void writeSpeed()
{
  EEPROM.update(speedaddress, currentSpeed);
}
void writemoveMent()
{
  EEPROM.update(movementaddress, moveMent);
}
void writeLuminous()
{
  EEPROM.update(luminousaddress, luminous);
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

void movement()
{
    
  if(moveMent > 0)
  {
    
    switch(moveMent)
    {
        default:
        case 0:
        break;
        case 1:
            // omhoog
        break;
        case 2:
          states[portIndex] = LOW;
          portIndex++;
          portIndex = portIndex > 15 ? 0 : portIndex;   
          states[portIndex] = HIGH;
        break;
        case 3:
            // down
        break;
        case 4:
          states[portIndex] = LOW;
          portIndex--;
          portIndex = portIndex < 0 ? 15 : portIndex;
          states[portIndex] = HIGH;
        break;      
    }        
    setLuminous();


    delay(currentSpeed );
    }
  
}

void resetAllSettings()
{
  moveMent = 0;// none
  portIndex = 0;//
  currentSpeed = speedMin;
  writeStates();
}
void toggleState(int idx)
{

  if(idx < 16)
  {
      states[idx] = states[idx] == HIGH ? LOW : HIGH;
      EEPROM.update(idx, states[idx]);
      Tlc.set(idx, states[idx] == HIGH ? currentLuminous : 0);
      Tlc.update();delay(75);
  }
 
  //Serial.print("toggleState:");  Serial.println(idx);
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
        case  553582847:  moveMent = 1;writemoveMent();break; // navUP
        case  553599167:  moveMent = 2;writemoveMent();break; // navRight
        case  553615487:  moveMent = 3;writemoveMent();break; // navDown
        case  553631807:  moveMent = 4;writemoveMent();break; // nacLeft
        case  553646087:  resetAllSettings();break; // navOK
        case  553642517:  incrementLuminous();break; // V+
        case  553589477:  decrementLuminous();break; // V-
        case  553591007:  toggleState(23);break; // tel/muz
        case  553613957:  toggleState(24);break; // sat
        case  553626197:  incrementSpeed();break; // P+
        case  553609877:  decrementSpeed();break; // P-
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
        case  553603247:  togglePower();break; // POWER
        case  553595087:  toggleState(0);break; // Mute
        default:
            //Serial.println(recv);   
            break;
         }
         
}



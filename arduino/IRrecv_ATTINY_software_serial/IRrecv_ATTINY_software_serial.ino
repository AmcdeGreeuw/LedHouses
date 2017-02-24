/*
 * tiny_IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <tiny_IRremote.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 4); // RX, TX

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  mySerial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    mySerial.println(results.value);
    irrecv.resume(); // Receive the next value
  }
}

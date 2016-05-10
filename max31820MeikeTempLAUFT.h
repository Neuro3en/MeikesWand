// full sketch by niq_ro from http://nicuflorica.blogspot.com
// or http://www.tehnic.go.ro
// or http://arduinotehniq.blogspot.com/

#include <Wire.h> // for i2c protocol

#include <OneWire.h>
// http://www.pjrc.com/teensy/td_libs_OneWire.html
// http://milesburton.com/Dallas_Temperature_Control_Library
OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)  

byte grad[8] = {
  B01110,
  B10001,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000,
};

int ics =0; //count number of sensor

void setup(){
    Serial.begin(9600);
}

void loop(){
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
    if ( !ds.search(addr)) {
   
  Serial.println(" ");    
    ds.reset_search();
    ics=0;
    return;
  }
  
ics++; 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end    44
  
  delay(800);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }

//  for( i = 0; i < 8; i++) {
//      Serial.print(' ');
//      Serial.print(addr[i], HEX);
//  }
//Serial.println(' ');
  if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
      return;
  }

  celsius = (float)raw / 16.0;

  Serial.print(celsius);
  Serial.print("  ");
  
  
 // delay(800);
}

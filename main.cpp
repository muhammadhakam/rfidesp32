#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <WiFiMulti.h>
#define indexCode 2
#define en 4
#define PICC_0 ;
#define capasitors 15
#define pulsePin 5
#define buz 13

long sumLengnt = 0;
long ignore = 0;
long diff = 0;
long timeP = 0;
long PeriodeBuzz = 0;

Servo Gate;
int RX3 = 2;
int TX3 = 4;

WiFiMulti WiFimul;
#define WIFI_SSID "ABA Workplace"
#define WIFI_Password "aba12345678"

byte type[2] = {1,2};
byte code[indexCode] = {0x21,0xFF};

bool valid = true;
bool started = true;
bool pass = false;
String respond;

uint8_t SingleRead[5] = {0x04, 0x00, 0x50,0xB8,0xFA};
uint8_t MultiRead[10] = {0x07, 0x00, 0x33, 0x01, 0x02, 0x00, 0x00, 0xFA};
uint8_t ReadActiveMode[11] = {0x0a, 0x00, 0x35, 0x01, 0x0101110, 0x05, 0x00, 0x04, 0x10, 0x00, 0xFA};
uint8_t SetPower[6] = {0x05, 0x00, 0x2F, 0x20, 0x00, 0xFA};
uint8_t SetWiegand[9] = {0x08, 0x00, 0x34, 0x0000, 0x30, 0x10,0x15, 0x00, 0xFA };
uint8_t GetUserUID[5] = {0x04, 0x00, 0x50, 0x00, 0xFA };
int data;

void setup() {
  Serial2.begin(115200);
  Serial.begin(115200);
  Serial2.write(SetPower, 6);
  Serial2.write(SetWiegand, 9);
  Serial2.write(ReadActiveMode, 11);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFimul.addAP( WIFI_SSID, WIFI_Password);
  while (WiFimul.run() != WL_CONNECTED)
  {
    delay(100);
  }
  Serial.println("Connected");
}

bool matchType(uint8_t data){
  for(int i = 0;i < 2;i++){
    if(data == type[i]) {
      respond = "00";
      respond += String(data,HEX)+ '-';
      return true;
    }
  }
  return false;
}

bool matchCode(uint8_t data){
  for(int i = 0;i < indexCode;i++){
    if(data == code[i]) {
      respond += String(code[i],HEX) + '-';
      return true;
    }
  }
  return false;
}


void loop() {
  //CRC 
  int crc;
  uint16_t paramL=0;
  uint8_t data =0;
  digitalWrite(LED_BUILTIN, WiFi.status()==WL_CONNECTED);
  while(Serial2.available()){
    Serial2.write(GetUserUID, 5);
    data = Serial2.read();
    if(data == 0x04){
      pass = data == 0x04 ? true:false;
      data = Serial2.read();
      crc = data;
      pass = matchType(data);
      data = Serial2.read();
      crc += data;
      pass = matchCode(data);
      if(pass){
        paramL = Serial2.read();
        crc+=paramL;
        paramL = paramL << 8;
        paramL += Serial2.read();
        crc+=paramL;
        respond += String(paramL,HEX)+'-';
        for(int i=0;i<paramL;i++){
          data = Serial2.read();
          respond += String(data,HEX)+'-';
          crc+=data;
        }
        crc &= 0xFF;
        data = Serial2.read();
        if(crc == data){
          respond += String(crc,HEX)+'-';
          data = Serial2.read();
          respond += String(data,HEX);

        }
        Serial.println(respond);
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
    //kasi akses fungsi mySQL//
    if (respond = data)
    {
      Serial.println("UID terdaftar");
      Gate.write(90);
      delay(3000);  //Ganti metal Sensor//
      Gate.write(0); 

    } if (WiFi.status() == WL_CONNECT_FAILED) {
      String UID[] = {respond};
      delay(1000);
      Gate.write(90);
      delay(3000); //Ganti metal Sensor//
      Gate.write(0);
      if (WiFi.status() == WL_CONNECTED) {
        // Kirim ke mySQL //
      }
    }
    
  }}
}


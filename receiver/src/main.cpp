/*
  Universidade Federal do ABC
  Mestrado em Engenharia da Informação
  Santo André - 2022

  Aluno: Caio Vinícius Ribeiro da Silva
  Professor: Doutor Carlos Eduardo Capovilla


  // 7C:9E:BD:5A:EE:44 (ESP 32 LORA)
  // 28:6C:07:A1:B2:E6 (D1 mini Pro)
  // 84:f3:eb:c9:cf:16 (nodeMCU)

*/

#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include "ESPNowW.h"

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED);


typedef struct struct_message
{
  int ID;
  // string
  char a[32];
  
  // lux or random float
  float b;

  // voltage battery or 3.14
  float c;

  // current battery or true
  float d;

  // voltage D1 
  float e; 
} struct_message;

// Create a struct_message called myData
struct_message myData;

// char nome [32];
String nome;
float lux, vbat, ibat, vd1;
float nrandom, pi, bin;


void displayPositions()
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Mestrado - Caio");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Internet of Things");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Teste");
  display.display();
}

void showVbattery()
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 0, "Mestrado - Caio");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(0, 10, "Lum: " + String(lux) + " lux"); 
  display.drawString(0, 20, "VBAT: " + String(vbat) + " V");
  display.drawString(0, 30, "IBAT: " + String(ibat) + " mA");
  display.drawString(0, 40, "VD1: " + String(vd1) + " V");
  display.display();
}

void showD1()
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 0, "Mestrado - Caio");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  display.drawString(0, 20, "VD1: " + String(vbat) + " V");
  display.drawString(0, 30, "ID1: " + String(ibat) + " mA");
  display.drawString(0, 40, "N: " + String(nrandom));
  display.display();
}

void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{

  // char macStr[18];
  // Serial.print("Packet received from: ");
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.println(macStr);

  memcpy(&myData, data, sizeof(myData));

  // Serial.printf("Board ID %u: %u bytes\n", myData.ID, data_len);

  if (myData.ID == 1)
  {
    nome = myData.a;
    nrandom = myData.b;
    pi = myData.c;
    bin = myData.d;

    Serial.print("ID: ");
    Serial.println(myData.ID);

    Serial.print("String: ");
    Serial.println(nome);

    Serial.print("Numero: ");
    Serial.println(nrandom);

    Serial.print("pi: ");
    Serial.println(pi);

    Serial.print("True: ");
    Serial.println(bin);
  }

  if (myData.ID == 2)
  {
    lux = myData.b;
    vbat = myData.c;
    ibat = myData.d;
    vd1 = myData.e;


    // Serial.print("ID: ");
    // Serial.println(myData.ID);

    // Serial.print("Lux: ");
    // Serial.println(lux);

    // Serial.print("VIN: ");
    // Serial.println(vin);

    // Serial.print("IIN: ");
    // Serial.println(iin);
  }

}

void setup()
{
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());

  // ESPNow.set_mac(sender_mac);
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.reg_recv_cb(onRecv);



}

void loop() 
{
  // showVbattery();
  showD1();

}
/*
  Universidade Federal do ABC
  Mestrado em Engenharia da Informação
  Santo André - 2022

  Aluno: Caio Vinícius Ribeiro da Silva
  Professor: Doutor Carlos Eduardo Capovilla


  Hardware: D1 mini Pro

*/

// Station - ESP8266 - send data

// 7C:9E:BD:5A:EE:44 (ESP 32 LORA)
// 28:6C:07:A1:B2:E6 (D1 mini Pro)


// 1 - Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ESPNowW.h"
#include <Wire.h>

// 2 - Variables

uint8_t receiver_mac[] = {0x7C, 0x9E, 0xBD, 0x5A, 0xEE, 0x44};

#define BOARD_ID 1 

typedef struct struct_message
{
  int ID;
  char nome[32];
  float lux;
  float vIn;
  float iIn;
  float vBat;

} struct_message;

struct_message myData;

// Time to Deepleep
const int SleepTimeSeconds = 15;

void conexao()
{
  const char *ssid = "";
  const char *password = "";

  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.print("WiFi Conectada - Endereco IP do D1 Mini Pro ");
  Serial.println(WiFi.localIP());
}



void sendData()
{

  myData.ID = BOARD_ID;
  strcpy(myData.nome, "UFABC"); //a
  myData.lux = random(1, 20);   //b
  myData.vIn = 3.14;            //c
  myData.iIn = true;            //d

  ESPNow.send_message(receiver_mac, (uint8_t *)&myData, sizeof(myData));

}

void setup()
{
  // ------ Serial config
  Serial.begin(9600);

  pinMode(D0, WAKEUP_PULLUP);

  // ------ ESP-NOW config
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.add_peer(receiver_mac, BOARD_ID);


  // DeepSleep
  // ESP.deepSleep(SleepTimeSeconds * 1000000);

  // Send data
  sendData();


  ESP.deepSleep(20e6);
  // ESP.deepSleep(5 * 1000000, RF_DISABLED);
  // ESP.deepSleep(20e6);
  // ESP.deepSleepInstant(10e6, RF_DISABLED);
  // ESP.deepSleepInstant(10e6);
}

void loop() 
{

}
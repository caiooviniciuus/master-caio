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

char ssid[20] = "Mestrado";
char password[20] = "ufabcaio";

// 2 - Variables

uint8_t receiver_mac[] = {0x7C, 0x9E, 0xBD, 0x5A, 0xEE, 0x44};

#define BOARD_ID 1 

typedef struct struct_message
{
  int ID;
  char a[32];
  float b;
  float c;
  float d;
  float e;

} struct_message;

struct_message myData;

// Time to Deepleep
const int SleepTimeSeconds = 15;

// esp_now_peer_info_t slave;
// int32_t RSSI;

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

// void scanSlave()
// {
//   int8_t scanResults = WiFi.scanNetworks();

//   bool scanFound = 0;
//   memset(&slave, 0, sizeof(slave));

//   if (scanResults == 0)
//   {
//     Serial.println("Não foi encontrado nenhum dispositivo no modo AP");
//   }
//   else
//   {
//     RSSI = WiFi.RSSI();
//   }
// }

void sendData()
{

  myData.ID = BOARD_ID;
  strcpy(myData.a, "UFABC");  //a
  myData.b = random(1, 20);   //b
  myData.c = 3.14;            //c
  myData.d = true;            //d

  ESPNow.send_message(receiver_mac, (uint8_t *)&myData, sizeof(myData));

}

void setup()
{
  // ------ Serial config
  Serial.begin(9600);

  pinMode(D0, WAKEUP_PULLUP);

  // ------ ESP-NOW config
  // antes estava STA
  WiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 1, false, 4);
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.add_peer(receiver_mac, BOARD_ID);


  // comentei aqui, para não entrar no deepsleep
  // Send data
  // sendData();

  // ESP.deepSleepMax();
  // ESP.deepSleep(20e6);
  // 20 ms (20e6)..., tentar alimentar com 2,5V no SENAI...
  // fazer o teste sem o modo sleep para ver o consumo...
}

void loop() 
{
  delay(1500);
  sendData();
  // Serial.print("ESP32 Board MAC Address:  ");
  // Serial.println(WiFi.macAddress());
  // depois preciso fazer o teste de consumo desabilitando o deepsleep...
  // fiz, fica 80mA full time
}

// o que preciso fazer:

/*
1.descobrir como faz o RSSI 
2.pensar em enviar pacotes de dados...

*/
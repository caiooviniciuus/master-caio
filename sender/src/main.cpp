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
#include <espnow.h>


const char *ssid = "Mestrado";
const char *password = "ufabcaio";

// 2 - Variables

uint8_t receiver_mac[] = {0x7C, 0x9E, 0xBD, 0x5A, 0xEE, 0x44};

#define BOARD_ID 1

float i = 0;

typedef struct struct_message
{
  int ID;
  float rssi;
  double pacote1;
  // double pacote2;
  float transmissao;
  float vd1;
  float id1;
  float vbat;
  float lux;

} struct_message;

struct_message myData;

// Time to Deepleep
const int SleepTimeSeconds = 15;


void conexao()
{
  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.print("WiFi Conectada");
}

void sendData()
{

  myData.ID = BOARD_ID;
  myData.transmissao = i;

  i++;


  if (i > 10000)
  {
    i = 0;
  }

  int32_t rssi = WiFi.RSSI();
  myData.rssi = rssi;





  myData.pacote1 = 100100100100;
  // myData.pacote2 = 100100100100;

  Serial.print("Tamanho da mensagem: ");
  Serial.println(sizeof(myData));

  // Serial.print("RSSI: ");
  // Serial.println(myData.rssi);

  // // envio do pacote de 10 bytes
  // int32_t encNow = 100;
  // myData.encoder = encNow;
  // myData.encoder_zero = 0;

  // Serial.print("Tamanho mensagem: ");
  // Serial.println(l);

  // Serial.print("Pacote1 enviado: ");
  // Serial.println(myData.encoder);
  // Serial.print("Pacote2 enviado: ");
  // Serial.println(myData.encoder_zero);

  // Serial.print("Dado enviado: ");
  // Serial.println(i);

  // delay(2000);

  // aqui envia a mensagem...
  ESPNow.send_message(receiver_mac, (uint8_t *)&myData, sizeof(myData));

  // int result;
  // result = ESPNow.send_message(receiver_mac, (uint8_t *)&myData, sizeof(myData));

  // esp_err_t result = ESPNow.send_message(receiver_mac, (uint8_t *)&myData, sizeof(myData));

  // int result;

  // result = esp_now_send(receiver_mac, (uint8_t *)&myData, sizeof(myData));

  // Serial.print("Retorno ESPNOW: ");
  // Serial.println(result);
}

// void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
//   Serial.println("Status:");
//   Serial.println(sendStatus);
// }

void setup()
{

  // ------ Serial config
  Serial.begin(9600);

  pinMode(D0, WAKEUP_PULLUP);

  // ------ ESP-NOW config
  // antes estava STA
  WiFi.mode(WIFI_STA);

  // WiFi.mode(WIFI_AP);
  // WiFi.softAP(ssid, password, 1, false, 4);

  WiFi.disconnect();

  ESPNow.init();
  ESPNow.add_peer(receiver_mac, BOARD_ID);

  // esp_now_register_send_cb(onSent);

  // conexao();

  // comentei aqui, para não entrar no deepsleep
  // sendData();

  // ESP.deepSleepMax();
  // ESP.deepSleep(20e6);

  // 20 ms (20e6)..., tentar alimentar com 2,5V no SENAI...
  // fazer o teste sem o modo sleep para ver o consumo...
}

void loop()
{

  // 2022-4-16 - Comentei para ver se conseguia melhorar o desempenho energético
  // 2022-4-16 - voltei o código anterior, não mudou o consumo essa alteração

  // retirei a conexao RSSI para ver o alcance máximo
  if (WiFi.status() != WL_CONNECTED){
    i = 0;
    conexao();
  }

  sendData();
  // delay(20);

  // Serial.print("ESP32 Board MAC Address:  ");
  // Serial.println(WiFi.macAddress());
  // depois preciso fazer o teste de consumo desabilitando o deepsleep...
  // fiz, fica 80mA full time
}
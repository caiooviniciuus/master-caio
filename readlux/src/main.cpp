/*
  Universidade Federal do ABC
  Mestrado em Engenharia da Informação
  Santo André - 2022

  Aluno: Caio Vinícius Ribeiro da Silva
  Professor: Doutor Carlos Eduardo Capovilla


  Hardware:

  TSL2561
  SDA - D2 (red)
  SCL - D1 (blue)
  D3  - VCC

  INA219
  SDA - D2 (red)
  SCL - D1 (blue)



*/

// Station - ESP8266 - send data

// 7C:9E:BD:5A:EE:44 (ESP 32 LORA)
// 28:6C:07:A1:B2:E6 (D1 mini Pro)
// 84:f3:eb:c9:cf:16 (nodeMCU)

// 1 - Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ESPNowW.h"
#include <Wire.h>
#include "SparkFunTSL2561.h"
#include "Adafruit_INA219.h"

#define BOARD_ID 2

// 2 - Variables

uint8_t receiver_mac[] = {0x7C, 0x9E, 0xBD, 0x5A, 0xEE, 0x44};

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
  double lux;
  // int ID;

  // // lux or random float
  // float a;

  // // voltage D1 or 3.14
  // float b;

  // // current D1 or true
  // float c;

  // // voltage bat
  // float d;

  // int32_t encoder;

  // int32_t encoder_zero;
} struct_message;

// Create a struct_message called myData
struct_message myData;

Adafruit_INA219 ina219;

// -----sensor snsLight
SFE_TSL2561 snsLight;

// Gain setting, 0 = X1, 1 = X16;
boolean gain;

// Integration ("shutter") time in milliseconds
unsigned int ms;

// Time to Deepleep
const int SleepTimeSeconds = 10;

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

void printError(byte error)
// If there's an I2C error, this function will
// print out an explanation.
{
  Serial.print("I2C error: ");
  Serial.print(error, DEC);
  Serial.print(", ");

  switch (error)
  {
  case 0:
    Serial.println("success");
    break;
  case 1:
    Serial.println("data too long for transmit buffer");
    break;
  case 2:
    Serial.println("received NACK on address (disconnected?)");
    break;
  case 3:
    Serial.println("received NACK on data");
    break;
  case 4:
    Serial.println("other error");
    break;
  default:
    Serial.println("unknown error");
  }

  // preciso enviar mensagens de erro via ESP-NOW
}

float readLux()
{

  unsigned int data0, data1;
  double lux;   // Resulting lux value
  boolean good; // True if neither sensor is saturated

  if (snsLight.getData(data0, data1))
  {
    // Perform lux calculation:

    good = snsLight.getLux(gain, ms, data0, data1, lux);

    // Print out the results:

    // Serial.print(" lux: ");
    // Serial.println(lux);
  }
  else
  {
    byte error = snsLight.getError();
    printError(error);
  }
  return lux;
}

float readVoltage()
{

  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  float loadbattery = analogRead(A0)*(3.7/1023.0); // leitura da tensão da bateria

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  // Serial.print(busvoltage);
  // Serial.print("\t");
  // Serial.print(shuntvoltage);
  // Serial.print("\t");
  // Serial.print(loadvoltage);
  // Serial.print("\t");
  // Serial.print(current_mA);
  // Serial.print("\t");
  // Serial.println(power_mW);

  return current_mA;
}

void sendData()
{
  // Set values to send
  myData.ID = BOARD_ID;
  myData.lux = readLux();
  myData.vd1 = ina219.getBusVoltage_V();
  myData.id1 = ina219.getCurrent_mA();
  myData.vbat = analogRead(A0) * (3.7 / 1023.0);

  // Serial.print("Tensao D1: ");
  // Serial.println(myData.vd1);

  // Serial.print("Corrente D1: ");
  // Serial.println(myData.id1);

  Serial.print("Luminosidade: ");
  Serial.println(myData.lux);

  // Serial.print("Tensao bateria ");
  // Serial.println(myData.vbat);

  ESPNow.send_message(receiver_mac, (uint8_t *)&myData, sizeof(myData));
}

void setup()
{
  // ------ Serial config
  Serial.begin(9600);

  // Serial.print("WiFi Conectada - Endereco IP do D1 Mini Pro ");
  // Serial.println(WiFi.localIP());
  // delay(1000);

  // ------ TSL2661 config
  snsLight.begin();

  unsigned char ID;

  if (snsLight.getID(ID))
  {
    // Serial.print("Got factory ID: 0X");
    // Serial.print(ID, HEX);
    // Serial.println(", should be 0X5X");
  }
  // Most library commands will return true if communications was successful,
  // and false if there was a problem. You can ignore this returned value,
  // or check whether a command worked correctly and retrieve an error code:
  else
  {
    byte error = snsLight.getError();
    printError(error);
  }

  // The light sensor has a default integration time of 402ms,
  // and a default gain of low (1X).

  // If you would like to change either of these, you can
  // do so using the setTiming() command.

  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)
  
  // tive problemas com o ganho 0, a luminosidade estava muito alta no teste em campo
  gain = 0;

  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration
  unsigned char time = 1; //antes era 2

  // Serial.println("Set timing...");
  snsLight.setTiming(gain, time, ms);

  // Serial.println("Powerup...");
  snsLight.setPowerUp();

  // ------ ESP-NOW config
  // Serial.println(WiFi.macAddress());
  // Serial.println("ESPNow sender Demo");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.add_peer(receiver_mac, BOARD_ID);

  ina219.begin();


}

void loop()
{
  // Send data
  sendData();
  delay(1000);
}
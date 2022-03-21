/*
  Universidade Federal do ABC
  Mestrado em Engenharia da Informação
  Santo André - 2022

  Aluno: Caio Vinícius Ribeiro da Silva
  Professor: Doutor Carlos Eduardo Capovilla


  // 7C:9E:BD:5A:EE:44 (ESP 32 LORA)
  // 5C:CF:7F:A1:B2:E6 (D1 mini Pro)
  // 84:f3:eb:c9:cf:16 (nodeMCU)



//
// Heltec 32 LoRa OLED (with SD Card)
//               ______________
//          Gnd |    |  USB|   | Gnd
//           5v |    | port|   |  5v
//          3v3 |    |_____|   | 3v3
//          Gnd |      ALL     |  36<-     <- : Input Only!
//           Rx |     GPIO     |  37<-
//           Tx |     3.3v     |  38<-
//          RST |   ________   |  39<-
//            0 |  |        |  |  34<-
//    (SCL)  22 |  |        |  |  35<-
// SPI MISO  19 |  |        |  |  32<-
//           23 |  |        |  |  33<-
//LoRa CS(HI)18 |  |        |  |  25  (LED)
// SPI SCK    5 |  |  OLED  |  |  26  LoRa IRQ
//OLED SCL   15 |  |        |  |  27  SPI MOSI
//            2 |  |        |  |  14  LoRa Rst
//OLED SCA    4 |  |        |  |  12
//           17 |  |________|  |  13
//OLED RST   16 |______________|  21  (SD_CS)
//

  Micro TF card pins to ESP32 GPIOs via this connecthin:
  SD_CS   -- GPIO22 pin 9
  SD_MOSI -- GPIO23 pin 11
  SD_SCK  -- GPIO17 pin 17
  SD_MISO -- GPIO13 pin 20

  RTC
  SDA - pin 19
  SCL - pin 9

*/
// Bibliotecas para o ESP32, WiFi e protocolo ESPNOW
#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include "ESPNowW.h"


// Bibliotecas para o Cartão SD e RTC
#include "FS.h"
#include "SD.h"
#include "SPI.h"

int32_t RSSIMestrado;

SPIClass spi1;

// Objeto para o display OLED, incluso na placa ESP32
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED);

// Estrutura da mensagem que será recebida
typedef struct struct_message
{
  int ID;
  // string
  char a[32];

  // lux or random float
  float b;
  
  // voltage D1 or 3.14
  float c;

  // current D1 or true
  float d;

  // voltage bat 
  float e; 
} struct_message;

// Create a struct_message called myData
struct_message myData;

// char nome [32];
String nome;
float lux, vd1, id1, vbat;
float nrandom, pi, bin;

// variaveis globais para o scan da rede
esp_now_peer_info_t slave;
#define CHANNEL 3
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

void getRSSI()
{
  int8_t scanResults = WiFi.scanNetworks();
  
  // Assume que o slave não foi encontrado
  bool RSSIflag = 0;

  // Não encontrou nenhuma rede
  if (scanResults == 0)
  {
    RSSIMestrado = 0;
  }
  else
  {
    for (int i = 0; i < scanResults; ++i)
    {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      delay(10);
      // Verifica se encontrou o SSID mestrado e se o macadress é o esperado
      if (SSID.indexOf("Mestrado") == 0 && BSSIDstr == "5E:CF:7F:A1:B2:E6")
      {
        RSSIMestrado = WiFi.RSSI(i);
        RSSIflag = 1;
        break;
      }
    }
  }

  // Se a flag = 1
  if (RSSIflag){
    // Serial.print("Consegui, o  RSSI mestrado é:");
    Serial.println(RSSIMestrado);
  }
  else{
    // Serial.println("Mestrado desconectado");
    RSSIMestrado = 0;
    // Serial.println(RSSIMestrado);
  }
  // clean up ram
  WiFi.scanDelete();
}

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

void showD1()
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 0, "Mestrado - Caio");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  display.drawString(0, 10, "VD1: " + String(vd1) + " V");
  display.drawString(0, 20, "ID1: " + String(id1) + " mA");
  // display.drawString(0, 30, "VBAT: " + String(vbat));

  if (RSSIMestrado != 0){
    display.drawString(0, 30, "RSSI: " + String(RSSIMestrado));
  }
  else{
    display.drawString(0, 30, "RSSI: sem sinal");
  }
    
  display.drawString(0, 40, "Lux: " + String(lux));
  display.drawString(0, 50, "N1: " + String(nrandom));
  display.display();
}

void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{

  memcpy(&myData, data, sizeof(myData));

  // Serial.printf("Board ID %u: %u bytes\n", myData.ID, data_len);

  if (myData.ID == 1)
  {
    nome = myData.a;
    nrandom = myData.b;
    pi = myData.c;
    bin = myData.d;

    // Serial.print("ID: ");
    // Serial.println(myData.ID);

    // Serial.print("String: ");
    // Serial.println(nome);

    // Serial.print("Numero: ");
    // Serial.println(nrandom);

    // Serial.print("pi: ");
    // Serial.println(pi);

    // Serial.print("True: ");
    // Serial.println(bin);
  }

  if (myData.ID == 2)
  {
    lux = myData.b;
    vd1 = myData.c;
    id1 = myData.d;
    vbat = myData.e;


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

void initESPNow(){
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.reg_recv_cb(onRecv);
}

void configSD(){

  // SPIClass(2);
  SPIClass(1);
  spi1.begin(17, 13, 23, 22);

  if (!SD.begin(22, spi1))
  {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println("Chegou aqui...");
}

void setup()
{
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());

  initESPNow(); // apenas separei em outra função...

}

void loop() 
{
  // showVbattery();
  showD1();

  getRSSI();

}
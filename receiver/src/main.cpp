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
  SDA - pin 16
  SCL - pin 14

  RTC (testar depois)
  SDA - pin 19
  SCL - pin 9

*/
// Bibliotecas para o ESP32, WiFi e protocolo ESPNOW
#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include "ESPNowW.h"
#include <esp_now.h>

// Objeto para o display OLED, incluso na placa ESP32
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED);

// Bibliotecas e objetos para o Cartão SD
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <WiFi.h>

SPIClass spi1;

// Biblioteca e objetos para função RTC (hora e data)
#include "Wire.h"
#include "RTClib.h"
RTC_DS3231 rtc;

int32_t encoder;

int32_t encoder_zero;

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

// Estrutura da mensagem que será recebida
typedef struct struct_message
{
  int ID;
  float rssi;
  double pacote1;
  float transmissao;
  float vd1;
  float id1;
  float vbat;
  double lux; 
} struct_message;

// Create a struct_message called myData
struct_message myData;

// char nome [32];
String nome;
double lux;
float vd1, id1, vbat;
float vrssi, pi, bin;

// numero incrementado que vem do D1 MiniPro
float vinc, vincAnterior = 0; 

// variaveis globais para o scan da rede
esp_now_peer_info_t slave;
#define CHANNEL 3
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

char ssid[20] = "Mestrado";
char password[20] = "ufabcaio";
int32_t RSSIMestrado;

#include <time.h>
// long timezone = 1;
long timezone = -4;
byte daysavetime = 1;

// const char *ssid = "Mestrado";
// const char *password = "ufabcaio";


String dataString = "";
File sensorData;

File file;

// Chave primária (ID na tabela)
int cprim = 1;

String nameCSV = "";

int PER = 0;
int SNR = 0;

// verificacao se há comunicacao... 
int recebeu_dado = 0;
// int recebeu_dado_anterior = 0;

// recebeu dado = True
bool comm = false;

int rep = 0;

double pacote1;
double pacote2;
float transmissao;

int flag = 0;

bool packet;

// status da comunicacao para salvar no CSV
bool c = false;

// grava se houve perda de pacote
bool grava_pacote = false;

// intervalo de tempo (0.5s)
const long interval = 500;
unsigned long previousMillis = 0;
String gravou = "";



int recebeu = 0;
int recebeu_anterior;


void displayPositions()
{
  Serial.println("Chegou aqui!");
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Mestrado - Caio");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Internet of Things");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Teste");
  display.display();
  delay(2000);
}

void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  memcpy(&myData, data, sizeof(myData));

  // D1
  if (myData.ID == 1)
  {    
    pacote1 = myData.pacote1;
    // pacote2 = myData.pacote2;
    transmissao = myData.transmissao;
    vrssi = myData.rssi;

    recebeu++;


    if(recebeu >10000){
      recebeu = 0;
      cprim = 0;
    }
  }

  // nodeMCU
  if (myData.ID == 2)
  {
    lux = myData.lux;
    vd1 = myData.vd1;
    id1 = myData.id1;
    vbat = myData.vbat;
  }

}

void initESPNow(){
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.reg_recv_cb(onRecv);
}

void configRTC(){

  #ifndef ESP8266
    while (!Serial)
      ; // wait for serial port to connect. Needed for native USB
  #endif

  if(!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (rtc.lostPower())
  {
    // colocar na tela OLED quando o RTC estiver sem energia
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2022, 4, 13, 17,23,00));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // we don't need the 32K Pin, so disable it
  rtc.disable32K();
  // rtc.adjust(DateTime(2022, 4, 3, 12,03,00));
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

char * uintToStr( const uint64_t num, char *str )
{
  uint8_t i = 0;
  uint64_t n = num;
  
  do
    i++;
  while ( n /= 10 );
  
  str[i] = '\0';
  n = num;
 
  do
    str[--i] = ( n % 10 ) + '0';
  while ( n /= 10 );

  return str;
}

void configSD(){
  // perdi MUITO tempo, porque estava alimentado com 3,3V
  // NECESSÁRIO ALIMENTAR COM 5VCC
  SPIClass(1);
  spi1.begin(17, 13, 23, 22);

  if (!SD.begin(22, spi1))
  {
    Serial.println("Card Mount Failed");
    
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(63, 0, "Falha na montagem");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 40, "do microSD");
    display.display();
    delay(2000);
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 0, "MicroSD");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 40, "não encontrado");
    display.display();
    delay(2000);
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 0, "Tipo do MicroSD:");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 40, "MMC");
    display.display();
    delay(2000);
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 0, "Tipo do MicroSD:");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 40, "SDSC");
    display.display();
    delay(2000);
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 0, "Tipo do MicroSD:");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 40, "SDHC");
    display.display();
    delay(2000);
  }
  else
  {
    Serial.println("UNKNOWN");

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 0, "Tipo do MicroSD:");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 40, "Desconhecido");
    display.display();
    delay(2000);
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  

  char str[21];

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(63, 0, "Capacidade do MicroSD:");
  display.setFont(ArialMT_Plain_16);
  // chamo função para converter a capacidade do cartão SD
  display.drawString(63, 40, String(uintToStr(cardSize,str)) + "MB");
  display.display();
  delay(2000);

  // Serial.println("Chegou aqui...");
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.print(file.name());
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size());
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

// void writeFile(fs::FS &fs, const char *path, const char *message)
void writeFile(fs::FS &fs, String path, String message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  // if (!file)
  // {
  //   Serial.println("Failed to open file for writing");
  //   return;
  // }
  // if (file.println(message))
  // {
  //   Serial.println("File written");
  // }
  // else
  // {
  //   Serial.println("Write failed");
  // }

  if (file){
    file.println(message);
    file.close();
  }


  // file.close();
}

// void appendFile(fs::FS &fs, const char *path, const char *message)
// void appendFile(fs::FS &fs, const char *path, String message)

// antes era void, mudei para char pelo retorno
String appendFile(fs::FS &fs, String path, String message)
{
  String card = "";
  // Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    // Serial.println("Failed to open file for appending");
    card = " (F)";
  }
  if (file.println(message)) // antes era print, sem o ln
  {
    // Serial.println("Message appended");
    card = " (G)";
  }
  else
  {
    // Serial.println("Append failed");
    card = " (E)";
  }
  file.close();
  return card;
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}


// vou chamar essa função por interrupção interna e gravar os dados
// void gravaDados(){

//   // D1 Mini Pro
//   // nome = myData.a;  // UFABC
//   // vrssi = myData.b; // RSSI
//   // vinc = myData.c;  // randon
//   // bin = myData.d;

//   // NodeMCU
//   // lux = myData.b;
//   // vd1 = myData.c;
//   // id1 = myData.d;
//   // vbat = myData.e;
  
//   // struct tm tmstruct;
//   // delay(1000);
//   // tmstruct.tm_year = 0;
//   // getLocalTime(&tmstruct, 5000);


//   // cpri, vd1, id1, vbat, lux, vrssi, nome, vinc (variavel de incremento), hora, minuto, segundo 
//   dataString = String(cprim) + "," + String(vd1) + "," + String(id1) + "," + String(vbat) + "," + String(lux) + "," + String(vrssi) + "," + String(nome) + "," + String(vinc) + "," + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec);

//   if (SD.exists("data.csv"))
//   {
//     sensorData = SD.open("data.csv");
//     sensorData.println(dataString);
//     sensorData.close();
//   }
//   // else{
//   //   // colocar erro no else
//   // }

//   cprim++;
// }

int abre_arquivo_gravacao(char filename[])
{
  file = SD.open(filename, FILE_WRITE);

  if (file)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void fecha_arquivo()
{
  if (file)
  {
    file.close();
  }
}



void showD1()
{


  
  // display.drawString(0, 20, "ID1: " + String(id1) + " mA");

  // if (vrssi != 0)
  // {
  //   display.drawString(0, 40, "RSSI: " + String(vrssi));
  // }
  // else
  // {
  //   display.drawString(0, 40, "RSSI: sem sinal");
  // }

  // display.drawString(0, 40, "Lux: " + String(lux));
  // display.drawString(0, 50, "N1: " + String(vinc));
  // display.drawString(0, 50, "VBat: " + String(vbat));
  
  
  
  // display.drawString(0, 50, "N1: " + String(vinc));
  // display.display();

  

  // antes, a ideia era montar uma mensagem e salvar no cartão SD
  // cpri, vd1, id1, vbat, lux, vrssi, nome, vinc (variavel de incremento), Horário
  
  // antes valiam as duas linhas abaixo
  // dataString = String(cprim) + "," + String(vd1) + "," + String(id1) + "," + String(vbat) + "," + String(lux) + "," + String(vrssi) + "," + String(nome) + "," + String(vinc) + "," + String((tmstruct.tm_year) + 1900) + "-"+ String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + " " + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec);
  // Serial.println(dataString);

  // String nameCSV = "/" + String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + "(" + String(daysOfTheWeek[tmstruct.tm_wday]) + ").csv";
  // Antes com o nome da semana no final...
  // nameCSV = "/" + String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + "(" + String(daysOfTheWeek[tmstruct.tm_wday]) + ").csv";
  // antes valiam a linha abaixo
  // nameCSV = "/" + String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + ".csv";

  // Serial.print(daysOfTheWeek[tmstruct.tm_wday]);

  // Serial.println(nameCSV);

  // appendFile(SD, nameCSV, dataString);


  // quero verificar se a variável anterior é igual a atual
  // if(vinc == vincAnterior){
  //   vinc = 0;
  //   comm = 0;
  // }
  // else{
  //   comm = 1;
  // }
  // vincAnterior = vinc;
  // delay(100);

  // nova lógica

  // 2 == 1



  // if (recebeu_dado == recebeu_dado_anterior){
  //   rep++;
  // }

  // if (rep > 10 && recebeu_dado_anterior == recebeu_dado)
  // {
  //   comm = false;
  //   // recebeu_dado = 0;
  // }

  // if (rep > 10 && recebeu_dado_anterior != recebeu_dado)
  // {
  //   comm = true;
  //   rep = 0;
  // }

  // if (!comm){
  //   recebeu_dado = 0;
  // }

  // recebeu_dado_anterior = recebeu_dado;



  // else{
  //   comm = true;
  //   rep = 0;
  // }

  // Serial.print("Dado anterior: ");
  // Serial.println(recebeu_dado_anterior);

  // Serial.print("recebeu_dado: ");
  // Serial.println(recebeu_dado);


  // Serial.print("rep: ");
  // Serial.println(rep);

  
  
  // 1==1

  // esp_err_t recebeu_dado = ;

  // Serial.print("comm: ");
  // Serial.println(comm);
  
  // Serial.print("recebeu_dado: ");
  // Serial.println(recebeu_dado);



  // dataString = "ID,VD1,ID1,VBAT,lux,RSSI,C,PER,SNR,DATA";


  // esp_err_t d = esp_now_unregister_recv_cb();

  // esp_err_t addStatus = esp_now_register_recv_cb(onRecv);

  // if (comm)
  // {
  //   Serial.println("Dado recebido: ");
  // }
  // else{
  //   Serial.println("Dado nao recebido: ");
  // }

  // Serial.print("comm: ");
  // Serial.println(comm);

  // Serial.print("packet: ");
  // Serial.println(packet);

  // Serial.print("recebeu_dado: ");
  // Serial.println(recebeu_dado);

  // esse bloco abaixo funciona, comentei pra testar outra possibilidade
  // // se recebeu dado
  // if (comm){
  //   recebeu_dado ++;
  //   flag = 0;
  //   c = true;

  //   if (pacote1 == 100100100100)
  //   {
  //     packet = true;
  //   }
  //   else
  //   {
  //     packet = false;
  //     grava_pacote = true;
  //     // escrever no cartao microSD que perdeu pacote
  //   }

  //   pacote1 = 0;

  //   if(recebeu_dado>10000){
  //     recebeu_dado = 0;
  //   }
  // }
  // else{
  //   flag++;
  // }

  // // se nao receber dados por 500 ms, considero que não há mais comunicacao
  // if (flag > 25){
  //   flag = 0;

  //   recebeu_dado = 0;
  //   vrssi = 0;
  //   packet = false;
  //   c = false;
  //   grava_pacote = true;

  // }




  // Serial.print("recebeu_dado ");
  // Serial.println(recebeu_dado);

  // Serial.print("recebeu_dado ");
  // Serial.println(recebeu_dado);


  // display.clear();
  // display.setTextAlignment(TEXT_ALIGN_CENTER);
  // display.setFont(ArialMT_Plain_10);

  // struct tm tmstruct;

  // tmstruct.tm_year = 0;
  // getLocalTime(&tmstruct, 5000);
  // // display.drawString(64, 0, String(tmstruct.tm_mday) + "/" + String((tmstruct.tm_mon) + 1) + "/" + String((tmstruct.tm_year) + 1900) + "-" + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec));
  // display.drawString(64, 0, String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + "      " + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec));

  // display.setTextAlignment(TEXT_ALIGN_LEFT);
  // display.setFont(ArialMT_Plain_10);

  // if (tmstruct.tm_wday == 6 || tmstruct.tm_wday == 0)
  // {
  //   display.drawString(0, 10, String(daysOfTheWeek[tmstruct.tm_wday]));
  // }
  // else
  // {
  //   display.drawString(0, 10, String(daysOfTheWeek[tmstruct.tm_wday]) + "-Feira");
  // }

  // dataString = String(cprim) + "," + // ID
  //             String(vd1) + "," +   // VD1
  //             String(id1) + "," +   // ID1
  //             String(vbat) + "," +  // VBAT
  //             String(lux) + "," +   // lux
  //             String(vrssi) + "," + // RSSI
  //             String(comm) + "," +  // dado recebido
  //             String(PER) + "," +   // PER
  //             String(SNR) + "," +   // SNR
  //             String((tmstruct.tm_year) + 1900) + "-" +
  //             String((tmstruct.tm_mon) + 1) + "-" +
  //             String(tmstruct.tm_mday) + " " +
  //             String(tmstruct.tm_hour) + ":" +
  //             String(tmstruct.tm_min) + ":" +
  //             String(tmstruct.tm_sec);


  // display.drawString(0, 20, "V: " + String(vd1) + " V" + " / " + "I: " + String(id1) + " mA");
  // display.drawString(0, 30, "Lux: " + String(lux) + " / " + "VBat: " + String(vbat));

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);

  struct tm tmstruct;

  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  // display.drawString(64, 0, String(tmstruct.tm_mday) + "/" + String((tmstruct.tm_mon) + 1) + "/" + String((tmstruct.tm_year) + 1900) + "-" + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec));
  display.drawString(64, 0, String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + "      " + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec));

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  if (tmstruct.tm_wday == 6 || tmstruct.tm_wday == 0)
  {
    display.drawString(0, 10, String(daysOfTheWeek[tmstruct.tm_wday]));
  }
  else
  {
    display.drawString(0, 10, String(daysOfTheWeek[tmstruct.tm_wday]) + "-Feira");
  }

  if(recebeu == recebeu_anterior){
    flag++;
  }
  // se for diferente, significa que teve dado recebido
  else{
    flag = 0;
    comm = true;

    if (pacote1 == 100100100100)
    {
      packet = true;
    }
    else
    {
      packet = false;
      grava_pacote = true;
      // escrever no cartao microSD que perdeu pacote
    }
  }
  recebeu_anterior = recebeu;

  // perda de comunicação
  if(flag>15){
    recebeu = 0;
    flag = 0;
    packet = false;
    comm = false;
    vrssi = 0;

    grava_pacote = true;
  }

  // varreudura do microcontrolador é até 5x mais rapida que o recebimento de dados 
  Serial.print("recebeu: ");
  Serial.println(recebeu);
  // Serial.print("flag: ");
  // Serial.println(flag);



  display.drawString(0, 20, "V: " + String(vd1) + " V" + " / " + "I: " + String(id1) + " mA");
  display.drawString(0, 30, "Lux: " + String(lux) + " / " + "VBat: " + String(vbat));
  display.drawString(0, 40, "RSSI: " + String(vrssi) + " / " + "C: " + String(comm) + String(gravou));
  // display.drawString(0, 50, "P: " + String(packet) + " / " + "T: " + String(recebeu_dado));
  display.drawString(0, 50, "P: " + String(packet) + " / " + "T: " + String(recebeu));
  display.display();

  // display.drawString(0, 40, "RSSI: " + String(vrssi) + " / " + "C: " + String(comm) + String(gravou));
  // display.drawString(0, 50, "P: " + String(packet) + " / " + "T: " + String(recebeu_dado));

  // pensar em como mostrar a gravação dos dados no display usando millis e o proprio retorno da funcao appendFile

  // nameCSV = "/" + String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + ".csv";

  // preciso gravar os dados a cada segundo, mais rapido que isso corrompe o microSD
  // usar a função millis()
  // funcao para gravar os dados: appendFile(SD, nameCSV, dataString)
  // display.drawString(0, 50, "C: " + String(comm) + String(appendFile(SD, nameCSV, dataString) + " / " + "T: " + String(recebeu_dado)));

  // display.display();


  unsigned long currentMillis = millis();

  // gravo a cada meio segundo no cartão
  if (currentMillis - previousMillis >= interval || grava_pacote)
  {
    cprim++;

    dataString = String(cprim) + "," +  // ID
                 String(vd1) + "," +    // VD1
                 String(id1) + "," +    // ID1
                 String(vbat) + "," +   // VBAT
                 String(lux) + "," +    // lux
                 String(vrssi) + "," +  // RSSI
                 String(comm) + "," +      // comunicação
                 String(packet) + "," + // PER
                 String(SNR) + "," +    // SNR
                 String((tmstruct.tm_year) + 1900) + "-" +
                 String((tmstruct.tm_mon) + 1) + "-" +
                 String(tmstruct.tm_mday) + " " +
                 String(tmstruct.tm_hour) + ":" +
                 String(tmstruct.tm_min) + ":" +
                 String(tmstruct.tm_sec);

    gravou = appendFile(SD, nameCSV, dataString);
    previousMillis = currentMillis;
    grava_pacote = false;

    // display.drawString(0, 50, "P: " + String(packet) + " / " + "T: " + String(recebeu_dado));
  }


}



void conexao()
{
  // const char *ssid = "F106_CS10";
  // const char *password = "Senai4.0";
 
  // const char *ssid = "2GVAICORINTHIANS";
  // const char *password = "C@io$Brun@21";


  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(63, 0, "Conectando a rede");
  display.setFont(ArialMT_Plain_10);
  display.drawString(63, 40, String(ssid));
  display.display();
  delay(500);

  WiFi.begin(ssid, password);

  // bool connect = true;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(63, 0, "Conectado com");
  display.setFont(ArialMT_Plain_16);
  display.drawString(63, 40, "Sucesso!");
  display.display();
  // delay(1200);
  delay(500);

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Contacting Time Server");
  // configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  configTime(3600 * timezone, daysavetime * 3600, "a.st1.ntp.br", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct;
  // delay(2000);
  delay(500);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
  Serial.println("");

  // Antes com o nome da semana no final...
  // nameCSV = "/" + String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + "(" + String(daysOfTheWeek[tmstruct.tm_wday]) + ").csv";
  nameCSV = "/" + String((tmstruct.tm_year) + 1900) + "-" + String((tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + ".csv";

  // IMPORTANTE
  // escrever o cabeçalho inicial do CSV
  // só pode escrever uma unica vez, se o arquivo não existir
  // se ele existir, não fazer nada
  // cpri, vd1, id1, vbat, lux, vrssi, nome, vinc (variavel de incremento), Horário
  
  dataString = "ID,VD1,ID1,VBAT,lux,RSSI,C,PER,SNR,DATA";

  // cria um novo arquivo para cada dia
  if (!SD.exists(nameCSV)){
    appendFile(SD, nameCSV, dataString);

  }

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(63, 0, "Dados no arquivo");
  display.setFont(ArialMT_Plain_10);
  display.drawString(63, 40, String(nameCSV));
  display.display();
  delay(500);

}


void setup(){
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  Serial.begin(115200);

  // fiz mudanças aqui, o comentário era o que estava antes...
  // nao posso manter
  // WiFi.mode(WIFI_MODE_STA);
  // Serial.println(WiFi.macAddress());
  
  // função para configurar a shield cartão SD
  configSD();

  conexao(); // conexão a internet necessária para coletar dados de data e hora
  
  WiFi.disconnect(); // precisei desconectar para ter receber os dados dia ESP-NOW
  delay(1000);

  // deixei o receiver como ap, para o esp8266 se comunicar...
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password, 1, false, 4);

  initESPNow(); // apenas separei em outra função...

  // esp_now_register_recv_cb(receiver);

  // listDir(SD, "/", 0);
  // removeDir(SD, "/mydir");
  // createDir(SD, "/mydir");
  // deleteFile(SD, "/hello.txt");
  // writeFile(SD, "/hello.txt", "Hello ");
  // appendFile(SD, "/hello.txt", "World!\n");
  // listDir(SD, "/", 0);

}


void loop() {
  
  // showVbattery();

  // if (WiFi.status() != WL_CONNECTED)
  // {
  // conexao();
  // }
  
  // RSSIMestrado = WiFi.RSSI();

  // getRSSI();
  
  showD1();
  // delay(20);


}


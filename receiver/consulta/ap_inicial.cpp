#define SSID "AP_mestrado"
#define PASSWORD "2019ufabc"
#define TIMEOUT 2000

WiFiServer server(80);
WiFiClient client;

String pacote_enviado = "ufabc_mestrado";

long rssi;
float volts;
int sendPack, receivePack, percBattery;

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

void testeDisplay()
{
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 0, "Mestrado - Caio");
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 26, "RSSI: " + String(rssi) + " dB");
    display.drawString(0, 46, "Bateria: " + String(volts) + "V");
    display.display();
}

void configWiFi()
{
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(63, 10, "Criando ponto de acesso");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 30, String(SSID));
    display.display();
    delay(2000);

    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1),
                      IPAddress(255, 255, 255, 0));
    WiFi.softAP(SSID, PASSWORD, 11);

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(63, 10, "Ponto de acesso criado");
    display.setFont(ArialMT_Plain_16);
    display.drawString(63, 30, String(SSID));
    display.display();

    delay(2000);
}

void esperaCliente()
{
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(63, 10, "Aguardando Conexão");
    display.setFont(ArialMT_Plain_10);
    display.drawString(63, 30, "do cliente");
    display.display();

    while (!(client = server.available()))
    {
        delay(500);
    }

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(63, 10, "Cliente conectado");
    display.display();

    // client.setTimeout(TIMEOUT);
}

void checaConexao()
{
    if (!client.connected())
    {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(ArialMT_Plain_10);
        display.drawString(63, 10, "Cliente deconectado");
        display.display();

        delay(2000);

        esperaCliente();
    }
}

void enviaCliente()
{
    if (client.connected())
    {
        client.println(pacote_enviado);
    }
}

void setup()
{
    Serial.begin(115200);

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    configWiFi();
    server.begin();
    esperaCliente();

    rssi = 20;
    volts = 3.6;
}

void loop()
{
    checaConexao();
}
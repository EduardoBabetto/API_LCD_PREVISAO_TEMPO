#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include "esp_sntp.h"


LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* nomeRedeWiFi = "SENAI EDUCACIONAL";
const char* senhaRedeWiFi = "info@134";
const String enderecoAPI = "https://wttr.in/Sao+Caetano+do+sul?m&lang=pt&format=";


String pedirParaApiClima(String);
void mostrarTela(int);

int contador = -1;

//* --------------------------Servidores NTP -------------------------
const char* NTP1 ="a.st1.ntp.br";
const char* NTP2 = "pool.ntp.org";
const char* FUSO = "<-03>3";
const char* DIAS[] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};

void setup() {
    Serial.begin(115200);
    WiFi.begin(nomeRedeWiFi,senhaRedeWiFi);

    lcd.init();
    lcd.backlight();

    Serial.print("\n\rIniciando conexao WiFi");
    lcd.setCursor(4,2);
    lcd.print("Conectando");
    while(WiFi.status() != WL_CONNECTED){
        lcd.setCursor(15,2);
        lcd.print(".");
        delay(1000);
    }
    contador++;
    
    Serial.print("\n\rConectado com sucesso!\n\r");
    Serial.printf("Endereco IP: %s\n\r", WiFi.localIP().toString());
    Serial.printf("Endereco MAC: %s\n\r", WiFi.macAddress().c_str());
    
    sntp_set_sync_interval(60*60*1000);  //* Intervalo de att da hora na internet padrao 1*60*60*100
    //*Configuracao de fuso e servidores NTP e inicia a sincronizacao
    configTzTime(FUSO, NTP1, NTP2);

    struct tm agora;
    Serial.print("\n\rSincronizando hora \n\r");
    //* o parametro e o endereco da estrutura tipo tm
    while(!getLocalTime(&agora)){
        Serial.print(".");
        delay(500);
    }
    Serial.print("\n\rHora sincronizada!");

}

void loop() {

    
    lcd.clear();

    mostrarTela(contador);

    delay(2000);

    contador++;

    if(contador > 2){
        contador = 0;
    }
  
}

String pedirParaApiClima(String codigo){
    HTTPClient http;
    http.begin(enderecoAPI + codigo);

    int codigoHTTP = http.GET(); //* Retorna o codigo HTTP ex.:200 -> OK ou 404 -> Not found

    String resposta;

    if(codigoHTTP == 200){
        resposta = http.getString();//* Retorna o conteudo HTML da pagina
    }
    else {
        Serial.printf("Deu erro. Codigo: %d\n\r",codigoHTTP );
    }
    http.end();
    return resposta;
}

void mostrarTela(int tela){
    struct tm agora;

    String clima = pedirParaApiClima("%C");
    int temperatura = pedirParaApiClima("%t").toInt();
    int umidade = pedirParaApiClima("%h").toInt();
    String vento = pedirParaApiClima("%w");

    switch (tela)
    {
    case 0:
        lcd.setCursor(0,0);
        lcd.printf("Wi-Fi : %s", WiFi.networkID());
        lcd.setCursor(0,1);
        lcd.printf("IP : %s", WiFi.localIP().toString());
        lcd.setCursor(0,2);
        lcd.printf("MAC : %s", WiFi.macAddress().c_str());
        lcd.setCursor(0,3);
        lcd.printf("SSID: %s",WiFi.SSID());
    
    case 1:
        lcd.setCursor(0,0);
        lcd.printf("Temp : %d", temperatura);
        lcd.setCursor(0,1);
        lcd.printf("Umidade : %d", umidade);
        lcd.setCursor(0,2);
        lcd.printf("Vento : %s", vento);
        lcd.setCursor(0,3);
        lcd.printf("%s", clima);
    case 2:
        if(getLocalTime(&agora)){
            lcd.setCursor(0,0);
            lcd.printf("Dia: %s", DIAS[agora.tm_wday]);
            lcd.setCursor(0,1);
            lcd.printf("Horas : %02d", agora.tm_hour);
            lcd.setCursor(0,2);
            lcd.printf("Minutos : %02d", agora.tm_min);
            lcd.setCursor(0,3);
            lcd.printf("Segundos: %02d", agora.tm_sec);
        }
    }
}


#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 40);

const char* nomeRedeWiFi = "SENAI EDUCACIONAL";
const char* senhaRedeWiFi = "info@134";
const String enderecoAPI = "https://wttr.in/Sao+Caetano+do+sul?m&lang=pt&format=";


String pedirParaApiClima(String);
String clima;
int temperatura;
int sensacaoTermica;

void setup() {
    Serial.begin(115200);
    WiFi.begin(nomeRedeWiFi,senhaRedeWiFi);
    lcd.init();
    lcd.backlight();

    Serial.print("\n\rIniciando conexao WiFi");
    while(WiFi.status() != WL_CONNECTED){
        Serial.print("#");
        delay(500);
    }

    Serial.print("\n\rConectado com sucesso!\n\r");
    Serial.printf("Endereco IP: %s\n\r", WiFi.localIP().toString());
    Serial.printf("Endereco MAC: %s\n\r", WiFi.macAddress().c_str());

}

void loop() {

    clima = pedirParaApiClima("%C");
    temperatura = pedirParaApiClima("%t").toInt();
    sensacaoTermica = pedirParaApiClima("%f").toInt();

    Serial.printf("Condicao climatica para hoje: %s \n\r",clima.c_str());
    Serial.printf("temperatura para hoje: %d \n\r",temperatura);
    Serial.printf("sensacao termica para hoje: %d \n\r",sensacaoTermica);

    String climaPrint = "Clima: %s";
    String temperaturaPrint = "";
    String sensacaoPrint = "Sensacao termica: %d *C";
    
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.printf("Temp : %d *C",temperatura);
    
    for(int i = 0; i < sizeof(sensacaoPrint) ; i++){
        lcd.scrollDisplayLeft();
        lcd.setCursor(0,1);
        lcd.printf("Sensacao termica: %d *C", sensacaoTermica);
        delay(500);
    }
    for(int i = 0 ; i < sizeof(climaPrint);i++){
        lcd.scrollDisplayLeft();
        lcd.setCursor(0,2);
        lcd.printf("Clima: %s", clima.c_str());
        delay(500);
    }

    
    lcd.setCursor(0,4);
    lcd.print("----");

    delay(10000);
  
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


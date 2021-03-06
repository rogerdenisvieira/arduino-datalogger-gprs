/**************************************************************
*                  Includes das bibliotecas                   *
***************************************************************/
#include "RTClib.h"
#include "DHT.h"
#include <SD.h>

/**************************************************************
*                         Constantes                          *
***************************************************************/

//Pino CS do cartao SD
int Pino_CS = 10;
//Definicoes do sensor de temperatura DHT11
#define DHTPIN A0         //Pino de conexao
#define DHTTYPE DHT11     //Tipo de sensor
DHT dht(DHTPIN, DHTTYPE); //Cria objeto DHT

RTC_DS1307 rtc;
File file;

/**************************************************************
*                    Funções principais                       *
***************************************************************/

void setup()
{
    Serial.begin(57600);

    //Inicializa o DHT11
    dht.begin();
    initializeRTC();
    initializeSDCard();
}

void loop()
{
    //Le as informacoes da temperatura
    float temperature = dht.readTemperature();

    saveLog("datalog.csv", temperature, 2.0, 3.0, 4.0, true);

    //Delay ate a proxima leitura
    delay(5000);
}

/**************************************************************
*                    Funções auxiliares                       *
***************************************************************/

void printLog(float temperature, float humidity, float altitude, float pressure)
{

    // pegando o momento atual
    DateTime now = rtc.now();

    //Serial monitor informacoes de hora
    Serial.print(now.hour() < 10 ? "0" : "");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute() < 10 ? "0" : "");
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second() < 10 ? "0" : "");
    Serial.print(now.second(), DEC);
    Serial.print(" ");
    Serial.print("Temperatura: ");
    Serial.print(temperature, 0);
    Serial.print("Umidade: ");
    Serial.print(humidity, 0);
    Serial.print("Altitude: ");
    Serial.print(altitude, 0);
    Serial.print("Pressão: ");
    Serial.print(pressure, 0);
}

void saveLog(const char *fileName, float temperature, float humidity, float altitude, float pressure, bool shouldLog)
{
    // exibe os logs
    if (shouldLog)
    {
        printLog(temperature, humidity, altitude, pressure);
    }

    // pegando o momento atual
    DateTime now = rtc.now();

    // solicitado para o módulo SD a abertura do SD em modo WRITE
    file = SD.open(fileName, FILE_WRITE);

    // gravação dos dados TODO: considerar a utilização de string interpolation
    file.print(now.hour() < 10 ? "0" : "");
    file.print(now.hour(), DEC);
    file.print(':');
    file.print(now.minute() < 10 ? "0" : "");
    file.print(now.minute(), DEC);
    file.print(':');
    file.print(now.second() < 10 ? "0" : "");
    file.print(now.second(), DEC);
    file.print(",");
    file.print(temperature, 0);
    file.print(",");
    file.print(humidity, 0);
    file.print(",");
    file.print(altitude, 0);
    file.print(",");
    file.print(pressure, 0);
    file.print(",");
    file.println();

    // fecha o I/O para não bloquear a thread
    file.close();
}

void initializeSDCard()
{
    //Inicia o cartao SD
    Serial.println("Initializing SD card...");

    if (!SD.begin(Pino_CS))
    {
        Serial.println("SD initialization failed.");
        return;
    }
    Serial.println("SD card initialized successfully.");
}

void initializeRTC()
{

    Serial.println("Initializing RTC...");
    //Verifica as condicoes do RTC
    if (!rtc.begin())
    {
        Serial.println("RTC wasn't found!");
        while (1)
            ;
    }
    if (!rtc.isrunning())
    {
        Serial.println("RTC isn't running!");
        //A linha abaixo ajusta o RTC com a data e hora do momento da compilacao
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        //A linha abaixo permite ajustar manualmente a data e hora do RTC
        //Formato: DateTime(ano, mes, dia, hora, minuto, segundo)
        //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
}
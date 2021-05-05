/**************************************************************
*                  Includes das bibliotecas                   *
***************************************************************/
#include "RTClib.h"
#include "DHT.h"
#include <SD.h>
#include <Ubidots.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>

/**************************************************************
*                         Constantes                          *
***************************************************************/

// dados para conexão GRPS
const char *APN = "timbrasil.br";
const char *USER = "tim";
const char *PASS = "tim";

// dados de autenticação e envio ao Ubidots
const char *TOKEN = "BBFF-Zydogj2uThBqI2wJAKrBaVZ5lKgmTU";
const char *DEVICE_LABEL = "Arduino_GPRS";
const char *VARIABLE_LABEL_1 = "Temperature";
const char *VARIABLE_LABEL_2 = "Pressure";
const char *VARIABLE_LABEL_3 = "Humidity";
const char *VARIABLE_LABEL_4 = "Altitude";

//Pino CS do cartao SD
int Pino_CS = 10;

//Definicoes do sensor de temperatura DHT11
#define DHTPIN A1         //Pino de conexao
#define DHTTYPE DHT11     //Tipo de sensor
DHT dht(DHTPIN, DHTTYPE); //Cria objeto DHT

// tipando o RTC
RTC_DS1307 rtc;

// objeto do tipo File
File file;


/**************************************************************
*                    Funções principais                       *
***************************************************************/


void setup()
{
    Serial.begin(57600);
    Serial.println("Initializing...");

    dht.begin();
    initializeRTC();
    initializeSDCard();
}

void loop()
{
    Serial.println("Collecting measurements...");
    float temperature = dht.readTemperature();      // lendo a temperatura pelo BMP180, poderia ser medido também pelo DHT
    // float pressure = bmp180.readPressure();      // lendo a pressão
    float pressure = 1.0;                           // lendo mock da pressão
    float humidity = dht.readHumidity();            // lendo a umidade
    // float altitude = bmp180.readAltitude();      // lendo altitude
    float altitude = 2.0;                           // lendo mock da altitude 
    
    saveLog("datalog.csv", temperature, humidity, altitude, pressure, true);

    // considerar separar isso em outra função...
    Serial.println("Preparing the request...");
    client.add(VARIABLE_LABEL_1, temperature);
    client.add(VARIABLE_LABEL_2, pressure);
    client.add(VARIABLE_LABEL_3, humidity);
    client.add(VARIABLE_LABEL_4, altitude);

    if(client.send())
    {
        Serial.print("Data sent to Ubidots successfully!");
    } 
    else
    {
        Serial.println("Data wasn't sent to Ubidots!");
    }


    //Delay ate a proxima leitura
    delay(5000);
}

/**************************************************************
*                    Funções auxiliares                       *
***************************************************************/


void saveLog(const char *fileName, float temperature, float humidity, float altitude, float pressure, bool shouldLog)
{
    // exibe os logs
    if (shouldLog)
    {
        printLog(temperature, humidity, altitude, pressure);
    }

    // pegando o momento atual
    DateTime now = rtc.now();

    Serial.println("Opening file...");
    // solicitado para o módulo SD a abertura do SD em modo WRITE
    file = SD.open(fileName, FILE_WRITE);

    Serial.println("Saving the measurement into SD card...")
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

    Serial.println("Clossing the file...");
    // fecha o I/O para não bloquear a thread
    file.close();
    Serial.println("Done!");


}

void printLog(float temperature, float humidity, float altitude, float pressure)
{
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

    Serial.println("Initializing RTC...")
    //Verifica as condicoes do RTC
    if (!rtc.begin())
    {
        Serial.println("RTC wasn't found!");
        while (1);
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
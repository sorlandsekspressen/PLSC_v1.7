/* Programmet er laget for MKR med SD, GPS og DS18B20 på PLSC-kort. Om ikke SD eller GPS er tilkoblet MKR, 
så kommenter ut funksjonene readGPSdata() og sdPrint().
Programmet er skrevet av Thor Inge Hansen og Nils K. Rossing
*/

#include <DS18B20.h>
#include <OneWire.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino_MKRGPS.h>
#include <MKRNB.h>
#include <WDTZero.h>

WDTZero myWDT;
NBClient client;
GPRS gprs;
NB nbAccess;
DS18B20 ds(0);
File myFile;


uint8_t address[] = { 40, 250, 31, 218, 4, 0, 0, 52 };
uint8_t selected;
char server[] = "sensor.marin.ntnu.no";
char path[] = "/cgi-bin/tof.cgi?";
char filename[] = "EVU-kurs.txt";     //Navn på filen som legges på server
int port = 80;
bool connected = false;  // Status oppkobling
char buffer[128];

int shutOffPin = 1;                   // Port til shutOffPin
float w_temperature = 0;
float GPSlat = 0;
float GPSlong = 0;
unsigned long epochTime;
int numSat = 0;
float battVolt = 0;

void setup() {
  pinMode(shutOffPin, OUTPUT);
  digitalWrite(shutOffPin, HIGH);

  analogReadResolution(12);  // 12Bits

  myWDT.attachShutdown(shutDownFunc);  // Initialiser intern vakthund 
  myWDT.setup(WDT_SOFTCYCLE2M);        // Sett toimeout vakthund til ca. 2 minutter (128 sek)

  readWaterTemp();
  readGPSdata();        // Les GPS data
  readBattVoltage();    // Les batterispenning
  makeString();         // Bygg opp bufferet
  sdPrint();            // Skriv til SD-kort
  connectToGPRS();      // Koble opp mot 4G (GPRS)
  connectToServer();    // Koble opp til server
  delay(1000);
  digitalWrite(shutOffPin, LOW);  //Be PICAXE om å slå av spenningen
}

// Les av vanntemperatur
void readWaterTemp() {
  w_temperature = ds.getTempC();
}

// Les av batterisepenningen
void readBattVoltage() {
  battVolt = analogRead(A1) * 2 * 3.3 / 4096;
}

// Les av GPS-data
void readGPSdata() {

  while (!GPS.available()) {}  // vent til GPS er tilgjengelig

  GPSlat = GPS.latitude();
  GPSlong = GPS.longitude();
  epochTime = GPS.getTime();
  numSat = GPS.satellites();
}

// Bygg opp databufferet for overføring til server
void makeString() {
  sprintf(buffer, "%s%s,%u,%.6f,%.6f,%i,%.1f,%.2f", path, filename, epochTime, GPSlat, GPSlong, numSat, w_temperature, battVolt);
}

// Koble til 4G GPRS
void connectToGPRS() {

  while (!connected) {
    if ((nbAccess.begin("", true, true) == NB_READY) && (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } 
    else delay(1000);
  }
}

// Koble til server
void connectToServer() {

  if (client.connect(server, port)) {

    client.print("GET ");  // Gjør et HTTP request:
    client.print(buffer);
    client.print(',');
    client.print(millis() + 4000);  //Prints runtime + set delay + calculated offset to show the runtime untill shuOffPin turns low
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  } 
}

// Skriv data-bufferet til SD-kort
void sdPrint() {

  if (!SD.begin(4))
    while (1) {}  //STOPP OPP

  myFile = SD.open("filename", FILE_WRITE);

  if (myFile) {
    myFile.println(buffer);
    myFile.close();
  }
}

// Funksjon som ber PICAXE slå av spenningen
void shutDownFunc() {
  digitalWrite(shutOffPin, LOW);
  delay(5000);
}

void loop() {}

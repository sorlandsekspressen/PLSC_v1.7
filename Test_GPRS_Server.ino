/*  Dette programmet er kun ment for av oppkobling til GPRS og opplasting av data til server på: "http://sensor.marin.ntnu.no/logs".
Data skrives også til seriemonitor. Programmet krever MKR NB1500, antenne og fungerende sim-kort. MKR skal ikke være tilkoblet PLSC.
MKR må kjøres via USB fra datamskin slik at seriekommunikasjon er åpen. Hvis DS18B20 er tilkoblet port 0, så sendes temperatur. 
Tilpass filnavn i variabel "filename". Forventet oppførsel er at det sendes noen målinger til server før GPRS faller ut og programmet 
stopper å sende. Inspirert av utallige lignende programmer. Thor Inge Hansen 08.09.23   */

#include <DS18B20.h> 
#include <OneWire.h>
#include <SPI.h>
#include <Arduino_MKRGPS.h>
#include <MKRNB.h>
 
NBClient client;
GPRS gprs;
NB nbAccess;
DS18B20 ds(0); 

uint8_t address[] = {40, 250, 31, 218, 4, 0, 0, 52};
uint8_t selected;
char server[] = "sensor.marin.ntnu.no";
char path[] = "/cgi-bin/tof.cgi?";
char filename[]="test_av_MKR.txt";  // filnavn.txt
int port = 80; // port 80 er default for HTTP
bool connectedGPRS = false; // Status oppkobling
bool connectedServer = false;
char buffer[128];

int num = 1; //teller
float w_temperature = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  
  connectToGPRS();
}

void loop() {
  connectedServer = false;
  readWaterData();
  makeString();
  connectToServer();
  serialPrint();
  num++;
  delay(10000);
}

void readWaterData(){
  w_temperature = ds.getTempC();
}

void makeString(){  
    sprintf(buffer,"%s%s,%u,%.1f", path, filename, num, w_temperature);
}

void connectToGPRS(){
  Serial.println("Prøver å koble til GPRS");
  while (!connectedGPRS) {
    if (nbAccess.begin("", true, true) == NB_READY) {
      Serial.println("nbAccess OK");
      if ((gprs.attachGPRS() == GPRS_READY)){
        connectedGPRS = true;
        Serial.println("GPRS attached");
      }
    }
    else delay(1000);    
  } 
}

void connectToServer(){
  Serial.println("Kobler til Server");
  while (!connectedServer){
    if (client.connect(server, port)){    
      client.print("GET "); // Gjør et HTTP request:
      client.print(buffer);
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(server);
      client.println("Connection: close");
      client.println();
      Serial.println("Data sendt til server");
      connectedServer = true; 
    } 
    else{
      delay(1000);
      connectedServer = false;
    }     
  }
}

void serialPrint(){
    Serial.println(buffer);
}

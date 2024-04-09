/* Programmet er laget for funksjonstest av PLSC. Programmet sender batterispenning og runTime
til NTNU-server i fila PLSC_funksjonstest.txt.  Programmet krever ikke GPS eller andre sensorer.
Krever MKR med gyldig sim-kort. All kode ligger i setup. Programmet er designet for bruk sammen med PLSC.
Forventet oppførsel er at data sendes 1 gang til server før PLSC skrur av MKR. Dette tar 10-20s. 
Thor Inge Hansen 08.09.23
*/

#include <MKRNB.h>
#include <WDTZero.h>                    

WDTZero myWDT;
NBClient client;
GPRS gprs;
NB nbAccess;                          
int shutOffPin = 1;

char server[] = "sensor.marin.ntnu.no";
char path[] = "/cgi-bin/tof.cgi?";
char filename[]="PLSC_funksjonstest.txt";  
int port = 80; 
bool connected = false; // Status oppkobling
char buffer[128];
float battVolt=0;
unsigned long restTid = 1271; //antall ms programmet kjører etter sending

void setup() {
  pinMode(shutOffPin, OUTPUT);
  digitalWrite(shutOffPin,HIGH);             
  myWDT.attachShutdown(shutDownFunc);   //Hvis vakthunden slår inn etter 2 min, kjør myshutdown()
  myWDT.setup(WDT_SOFTCYCLE2M);         //2 minutter før vakthunden slår inn
  
  analogReadResolution(12); // 12Bits

//Funksjonskall
  readBattVoltage();
  makeString();
  connectToGPRS();
  connectToServer();  
  delay(1000);
  digitalWrite(shutOffPin,LOW);         //Får Picaxe til å dra C0 høy og booster slår av 5V
}

// Funksjoner
void readBattVoltage(){
  
  battVolt = analogRead(A1)*2*3.3/4096;
}

void makeString(){  
    sprintf(buffer,"%s%s,%.2f",path, filename, battVolt);
}

void connectToGPRS(){
  while (!connected) {
    if ((nbAccess.begin("", true, true) == NB_READY) && (gprs.attachGPRS() == GPRS_READY)) {
      connected = true; 
    }      
  }
}

void connectToServer(){
  
  if (client.connect(server, port)) {    
    client.print("GET "); // Gjør et HTTP request:
    client.print(buffer);
    client.print(',');
    client.print(millis() + restTid);  //send runTime
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();    
  } 
}

void shutDownFunc(){                    //Funksjonen ligger som sikkerhet om MKR ikke slås av i linje 15 
  digitalWrite(shutOffPin,LOW);
  delay(10000);
}

void loop() {}          

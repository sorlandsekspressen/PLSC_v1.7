#include <WDTZero.h>                    //Bibliotek for vakthunden på MKR

WDTZero myWDT;                          // Lager et objekt til vakthunden 
int shutOffPin = 1;                     // Port til shutOffPin

void setup() {
  pinMode(shutOffPin, OUTPUT);
  digitalWrite(shutOffPin,HIGH);             
  myWDT.attachShutdown(shutDownFunc);   //Når vakthunden slår inn, kjør myshutdown()
  myWDT.setup(WDT_SOFTCYCLE2M);         //2 minutter før vakthunden slår inn
  
// Her legges alle funksjonskall eller kode

  delay(10000);
  digitalWrite(shutOffPin,LOW);         //Får Picaxe til å dra C0 høy og booster slår av 5V
}

// Her legges alle funksjonene som kalles over

void shutDownFunc(){                    //Funksjonen ligger som sikkerhet om MKR ikke slås av i linje 15 
  digitalWrite(shutOffPin,LOW);
  delay(10000);
}

void loop() {}                         //Ingen kode i loopen

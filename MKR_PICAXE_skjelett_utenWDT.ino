int shutOffPin = 1;                     // Port til shutOffPin

void setup() {
  pinMode(shutOffPin, OUTPUT);
  digitalWrite(shutOffPin,HIGH);             
  
// Her legges alle funksjonskall eller kode

  delay(10000);
  digitalWrite(shutOffPin,LOW); 
}

// Her legges alle funksjonene som kalles over

void loop() {}                         //Ingen kode i loopen

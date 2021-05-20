unsigned int PCLK = 53;


unsigned int mem_pin = 9;
unsigned int RW_pin = 8;
unsigned int reset = 7;


unsigned long start = 0;
unsigned long stopt = 0;
unsigned long tid = 0;

long cnt = 0;
int data = 0;
long r = 0;
long s = 0;

long wrData(byte x) { //Skrive funktion
  digitalWrite(PCLK, LOW); //simlurer pclk på kameraet
  digitalWrite(RW_pin, LOW); // sætter SRAM til skrive mode
  PORTA = x; //læser PORTA registret som svarer til hvad læses på pin 22-29
  digitalWrite(PCLK, HIGH); //pclk går høj for at skifte adresse
  return x;
}

long rdData() { //læs funktion
  digitalWrite(PCLK, LOW); 
  digitalWrite(RW_pin, HIGH); //Sætter SRAM til læse mode
  int x = PINC; //læser PORTA registret som svarer til hvad læses på pin 37-30
  digitalWrite(PCLK, HIGH); //skifter adresse
  return x;
} 




void setup() {
  // put your setup code here, to run once:
  pinMode(PCLK, OUTPUT);
  Serial.begin(115200);
  pinMode(RW_pin, OUTPUT);
  pinMode(mem_pin, OUTPUT);

  DDRA = 0b11111111; //sætter porta registret som OUTPUT
  DDRC = 0b00000000; //sætter portc registret som INPUT
 start = millis();


 digitalWrite(mem_pin, HIGH); //igang sættet en memory operation
 digitalWrite(reset, HIGH); //reset går høj lav for at være sikker på SRAM starter på adresse 0
 digitalWrite(reset, LOW);
  for(long j = 0; j<= 262144; j++){ // disse to foor lop sørger for der bliver printer 1,2,3,4 på alle adresser
    for ( int i = 1; i <= 4; i++){
    data=wrData(i);
     if(10000*r == cnt){ // printer værdien der bliver skrevet til SRAM chippen ved hvert 10000. adresse
        r++;
        Serial.print(data);Serial.print(",");
        Serial.println(cnt);
      }
      cnt++;
    }
  }
   
  
  
  stopt = millis();
  tid = stopt - start;

 digitalWrite(reset, HIGH); //resetter adressen til 0 igen
 digitalWrite(reset, LOW);
  //Serial.println(tid);
  for(long i = 0; i <= 1048574; i++){ //læser alle adresser på SRAM chippen
    //setAdr(i);//
    data=rdData();
      if(10000*s == i){ //printer værdien der er læst fra sram ved hvert 10000. adresse.
        s++;
        Serial.print(data);Serial.print(",");
        Serial.println(i);
      }
  }




    //Serial.println(i);
Serial.print(cnt);
digitalWrite(mem_pin, LOW);
Serial.println("idle");
delay(1000);
digitalWrite(mem_pin, HIGH);
digitalWrite(mem_pin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:


  }

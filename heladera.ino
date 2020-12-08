
// LIBRERIAS
#include <LiquidCrystal.h>
#include <TimerOne.h>

// VARIABLES

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


float R1 = 100000;    
float logR2, R2, temperatura;
float c1 = 2.114990448e-03, c2 = 0.3832381228e-04, c3 = 5.228061052e-07;// coeficientes Steinhart-hart 
volatile int medicion = 35; 

//PINES
const int intPin = 2;        //interrupción configuración
const int subir = A1;        //subir temperatura
const int bajar = A2;        //bajar temperatura
const int done  = A3;        //terminar la configuración
const int sensorPin = A0;    //sensor
const int enfriar=3;         //salida para enfriar
const int calentar=4;        //salida para calentar


//estados de los botones
volatile int subirState = HIGH;
volatile int bajarState = HIGH;
volatile int doneState = HIGH;

//variable auxiliar
volatile int deseada =  35; // temperatura deseada
volatile int deseadat =  35; //el cambio re refleja luego de un ciclo, antes no. Esta variable se usa para lograr esto

void setup() {
   lcd.begin(16, 2);
   pinMode(intPin, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(intPin), configuracion, FALLING); 
   pinMode(subir, INPUT);
   pinMode(bajar, INPUT);
   pinMode(done , INPUT);
   pinMode(enfriar, OUTPUT);
   pinMode(calentar,OUTPUT);
   Serial.begin(9600); 
   Timer1.initialize(250000); // Dispara cada 250 ms
   Timer1.attachInterrupt(printear); // Activa la interrupcion printear
}
void loop(){
  medicion = analogRead(sensorPin);           // lectura de A0
  R2 = R1 * (1023.0 / (float)medicion - 1.0); // conversion de tension a resistencia
  logR2 = log(R2);                            // calculo aux (si, tal vez un poco innecesario)
  temperatura = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));   // ecuacion Steinhart-hart 
  temperatura = temperatura - 273.15;   // Kelvin a Centigrados 
 
  if(temperatura>deseada){
     digitalWrite(enfriar, HIGH);
     digitalWrite(calentar, LOW);  
  }
  if(temperatura<deseada){
    digitalWrite(enfriar, LOW);
    digitalWrite(calentar, HIGH);
  }
  deseada = deseadat;
}
void printear(){
  lcd.setCursor(0, 0);
  lcd.print("T MEDIDA:");
  lcd.print(temperatura);
  lcd.print(" C");
  lcd.setCursor(0, 8);
  lcd.print("T DESEADA:");
  lcd.print(deseada);
  lcd.print(" C");  
  Serial.println(temperatura,4);
  }

void configuracion(){
   lcd.clear();
   lcd.print("CONFIGURACION");
   lcd.setCursor(0, 8);
   lcd.print("T DESEADA:");
   lcd.setCursor(13, 8);
   lcd.print(" C");
   doneState = digitalRead(done);
   while(doneState == HIGH){
      lcd.setCursor(11, 8);
      lcd.print(deseadat);
      subirState = digitalRead(subir);
      bajarState = digitalRead(bajar);
      doneState = digitalRead(done);
      if(subirState == LOW && deseadat<99){ deseadat++; delay(100000);}
      if(bajarState == LOW && deseadat>0 ){ deseadat--; delay(100000);}
   }
   lcd.clear();
  }

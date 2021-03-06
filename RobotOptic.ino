
/*
 * Programa per controlar un robot amb arduino de forma autonoma
 * 
 * Escrit per Xavier Delgado
 * GPL v3 License
 *
 * Com a sensor de distancia es fa servir l'HC-SR04 amb la llibreria NewPing.h 
 * http://playground.arduino.cc/Code/NewPing
 * 
 * 
 * Hi ha una fotoresistencia al pin A0 per mesurar la llum, es farà servir per seguir la linia
 * 
 * Modificació de un cotxe RC amb xip RX2
 * El codi de control del xip RX2 está basat en codi de:
 * Parts del codi de David Bernier
 * https://github.com/monsieurDavid/
 * Realtek RX2 (RX2B) Control via Arduino
 *
 * També basat en codi de mindprobe http://forum.arduino.cc/index.php?topic=171238.0
 * 
 * RX2B datasheet http://www.datasheetdir.com/RX-2B+download
 *   
 */

int avant; // Contador de les vegades que hem enviat el tipus de moviment
int enrera; // Indica si hem anat endarrera
int contador; // Revisar per eliminar, no es fa servir
long numAleatori;

// Sensor de llum ambiental
int sensorValue;
int sensorLow=1023;
int sensorHigh=0;
int tmpSensorValue;

const int valorBlanc = 400;

// Control xip RX2
//W1
#define ENDCODE         4
#define FORWARD        10
#define FORWARD_TURBO  16
#define TURBO          22
// Els 2 seguents els he intercambiat pq crec que estan malament
#define FORWARD_RIGHT  28
#define FORWARD_LEFT   34

#define BACKWARD       40
#define BACKWARD_RIGHT 46
#define BACKWARD_LEFT  52
// Tambe canviats
#define RIGHT           58
#define LEFT          64

//PIN de sortida i connectat a l'antena de la joguina
#define ANTENA     9
// Per tal que funcioni s'ha de cridar la funció moviment (int tipus_moviment) dient quin moviment fem i despres cridar-la amb ENDCODE

// Sensor UltraSonic de distancia
#include <NewPing.h>
// Definim on esta connectat i destància màxima
#define TRIGGER_PIN  13
#define ECHO_PIN     12
#define MAX_DISTANCE 400

// Inicialització de la funció
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
float cmMsec; //Variable on guardarem el resultat de sonar()

// Funció moviment 
void moviment (int tipus_moviment){
   for(int i=0; i<=3; i++) // Inici de la comunicació 4 W2 pulses 1KHz 75% duty cycle
 {
   digitalWrite(ANTENA, HIGH);
   delayMicroseconds(1500);

   digitalWrite(ANTENA, LOW);
   delayMicroseconds(500);
 }
 for(int i=0; i<=tipus_moviment; i++) // Fa el moviment que diem amb n W1 pulses 500Mhz 50% duty cycle
 {
   digitalWrite(ANTENA, HIGH);
   delayMicroseconds(500);

   digitalWrite(ANTENA, LOW);
   delayMicroseconds(500);
 }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ANTENA, OUTPUT);
  randomSeed(analogRead(1));
}

void loop() {
  // put your main code here, to run repeatedly:
  // Inicialitzem variables a cada loop
  contador=0;
  enrera=0;
  avant=0;
  cmMsec=0;
  
  delay(74);
  //Llegim valor del sensor de llum
  sensorValue = analogRead(A0);
  // Print del Valor per port serie
  Serial.println(sensorValue);
  // Obtenim la distancia
  cmMsec = sonar.ping_cm();
  delay(50);
  Serial.print("Fora Bucle, CM: ");
  Serial.println(cmMsec);
  /* Serial.print("Llum: ");
  Serial.print(sensorValue);
  Serial.print(", CM: ");
  Serial.println(cmMsec);*/
  if (cmMsec==0 or cmMsec>150){
    // Si val 0 es perque o esta fora de rang + de 4Mts o el sensor no llegeix bé
    // Alguns sensors HC-SR04 fallen i s'arreglar amb una resistencia de 10k http://therandomlab.blogspot.com.es/2015/05/repair-and-solve-faulty-hc-sr04.html
    moviment(FORWARD);
    moviment(ENDCODE);
  }
  while (cmMsec>38 and avant<4){
    moviment(FORWARD);
    moviment(ENDCODE);
    cmMsec = sonar.ping_cm();
    Serial.print("Endavant, CM: ");
    Serial.println(cmMsec);
    delay(50);
    avant++;
  }
  if (avant!=0){
    delay(500); // Donem temps a que es freni el cotxe  
  }
  else{
    avant=0;  
  }
  numAleatori=random(1,101); // Així farem que no sempre giri igual.
  
  while (cmMsec<58 and cmMsec>0 and avant <4){
    if (numAleatori>50){
      moviment(BACKWARD_RIGHT);
      moviment(ENDCODE);  
    }
    else{
      moviment(BACKWARD_LEFT);
      moviment(ENDCODE);
    }
    
    cmMsec = sonar.ping_cm();
    Serial.print("Endarrera dreta, CM: ");
    Serial.println(cmMsec);
    delay(50);
    enrera=1;
    avant++;
   }
   avant=0;
   if (enrera==1){
    while (cmMsec>38 and cmMsec>0 and avant<2){
      if (numAleatori>50){
        moviment(FORWARD_RIGHT);
        moviment(ENDCODE);  
      }
      else{
        moviment(FORWARD_LEFT);
        moviment(ENDCODE);
      }
      
      cmMsec = sonar.ping_cm();
      Serial.print("Endavant Dreta, CM: ");
      Serial.println(cmMsec);
      delay(50);
      avant++;
    }
   }
   enrera=0;
  
  //Estructura de seguiment linia
  /*if (sensorValue > valorBlanc){
    // Cotxe endavant
    Serial.println("Endavant");
  }
  else{
    //Mirem si es a la dreta o l'esquerra
    tmpSensorValue = sensorValue;
    //Girem dreta
    Serial.println("Dreta");
    //Nova Lectura valor del sensor
    sensorValue = analogRead(A0);
    if (tmpSensorValue < sensorValue){
      //Moviment OK
      Serial.println("OK");
    }
    else{
      //Hem de tornar 2 a l'esquerra
      Serial.println("Tornem");
      Serial.println("Esquerra"); 
    }
  }*/
  
}


#include <PubSubClient.h>
#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
#include <dht.h>
#include <Time.h>


//-------- CONFIGURACION DE CONEXION -----------


const char* ssid = "Godzilla";
const char* password = "@#_ayyy-L3M40~\\o/:D";

#define ORG "<freboard.com>"
#define DEVICE_TYPE "ESP8266"

char server[] = "";
char topic[] = "iot-2/evt/status/fmt/json";

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

//-----------ESTABLECE CONEXION------------------

Serial.print("Connecting to "); Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");

 Serial.print("WiFi connected, IP address: "); Serial.println(WiFi.localIP());


//conexion rele*********************************
// Conectamos el modulo de la siguiente forma:  
// GND -> GND  
// VDD -> 5V  
// IN  -> DITAL (BETWEN 8 & 13) 
//*********************************************
//Conexion sensor DHT1 (1)
//GND -> GND
//+ -> 5V (?)
// IN -> DIGITAL 7  // PROXIMO EN WIFI POR COMUNICACION DE RADIO AUTOMATA A UN WEBSERVER
//*********************************************
//SENSOR HUMEDAD PISO
//VCC a 5V
//GND a GND
//A0 a A0
//*********************************************

//DECLARACION RELÉ   -*LUEGO SE LE CAMBIARA LA POSICION--

const int r1 = 11;//KIT + COOLTUBE
const int r2 = 10;//VENTILADOR
const int r3 = 9;//HUMIFICADOR
const int r4 = 8;//EMERGENCIA
const int r5 = 12//INTRACTOR


float sensorHumedad;//A0
//DECLARACION LDR
int ldr1 = A1;
int valorLdr;

//.------------------------------------------------
//DEFINICION DEL TIEMPO
time_t fechaActual = now();

//dht DHT;
//DECLARACION SENSOR DHT11 (1)
//Declaramos que el sensor esta en el pin 6  
//#define DHTPIN  6 
//Definimos el modelo del sensor  

//Definimos la variable que utilizaremos para comunicarnos con el sensor  
dht dht1 ;

void setup() {

//RELES
  pinMode(r1,OUTPUT);
  pinMode(r2,OUTPUT); 
  pinMode(r3,OUTPUT);
  pinMode(r4,OUTPUT);
  pinMode(r5,OUTPUT);
 //--
 //DHT11
dht1.begin(2);
 //--
 //LDR
pinMode(ldr1, INPUT);
 //-- 
  //fechaActual.setTime(19,00,00,23,01,2016);//sincronizar el tiempo con el timer
Serial.begin(9600);
}

void loop() {

  //VERIFICAMOS CONEXION 
  if (!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }



  
// ***************  Lectura sensores ********************
// +++++++++++++++DHT(1)
//Leemos la humedad    
float h = dht1.humidity;    
//Leemos la temperatura    
float t = dht1.temperature;

//++++++++++++HUMEDAD SUELO
sensorHumedad = analogRead(A0); // leemos la entrada analógica pin 0:
sensorHumedad = 5-5*sensorHumedad/1023; // convertimos el valor análogo a centigrados.

//++++++++++++LDR 
valorLdr = analogRead(ldr1);
 
 Serial.println(valorLdr);
 valorLdr = map(valorLdr, 800, 50, 10, 1);
 delay(1000);



//*********************************************************
//FALTA ACA!!!!!!!!!!!!!!!!!!!
 if(t>32){
    encenderEmergenciaT();
    }
 if(t<27){
    apagarEmergenciaT();
  }

//SISTEMA RIEGO
Serial.print(sensorHumedad); // visualizamos el valor leído y sin salto de linea...
Serial.print("V, "); // Visualizamos los V conductividad entre los bornes del sensor.

sensorHumedad=sensorHumedad*20; /* Convertimos los 5 V a 100 para que cuando el sensor
lee 5v no de lectura de 1023 si no 100 y le damos la vuelta a la tortilla :) */

Serial.print(sensorHumedad); // Ahora si imprimimos la lectura correcta.
Serial.println ("%"); // ...Ponemos el símbolo de la Humedad
delay(1000); // retardo de medio segundo entre lecturas por estabilidad
if (sensorHumedad<=95) // si el sensor lee menos de 95%...
{
digitalWrite(13, HIGH); 
Serial.println("REGISTRO RIEGO COMUN ");
//Serial.print(printFechaCompleta(fechaActual));
//bloque imprimido
printFechaCompleta(fechaActual);


delay(30000);
}
else 
{
digitalWrite(13, LOW);
} 


//ENCENDER/APAGAR RELE


encenderAmpolleta(fechaActual);// capturar datos para que sea apartir de un horario creado 
apagarAmpolleta(fechaActual);
//------
//CONTROL LUZ 
if(valorLdr==1){
Serial.println("ERROR FATAL: INDOOR SIN LUZ A LAS");
printFechaCompleta(fechaActual);
}
//------
//vision de t h por ahora en serial, luego pantalla o web
Serial.println("Humedad: ");     
Serial.println(h);    
Serial.println("Temperatura: ");    
Serial.println(t);    



 delay(2000);

//---COMPOSICION DEL JSON ( 17-11 SOLO T° - H% )












 
  //----------------PUBLICA DATOS------------------------  VERIFICAR EL USO DEL SIGUIENTE APARTADO
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }


}

void encenderEmergenciaT()

{
  digitalWrite(r4,HIGH);//pinemergencia
  Serial.print("EMERGENCIA DE TEMPERATURA A LAS : ");
  printHora(fechaActual);
}

void apagarEmergenciaT(){
  digitalWrite(r4,LOW);//pinEmergencia    
  Serial.print("EMERGENCIA CONTROLADA A LAS : ");
  printHora(fechaActual);
}
void printHora(time_t t){
   Serial.print(hour(t));   Serial.print(+ ":") ;   Serial.print(minute(t));   Serial.print(":") ;
   Serial.println(second(t));
  
}
void printFechaCompleta(time_t  t)
   {
       Serial.print(day(t)) ;    Serial.print(+ "/") ;   Serial.print(month(t));   Serial.print(+ "/") ;
       Serial.print(year(t));    Serial.print( " ") ;
       Serial.print(hour(t));   Serial.print(+ ":") ;   Serial.print(minute(t));   Serial.print(":") ;
       Serial.println(second(t));
       delay(1000);
   }

void encenderAmpolleta(time_t horaEncendido){//luego pasar a loop con el apagado  
if(horaEncendido == fechaActual){
digitalWrite(r1,HIGH);
 Serial.print("Foco Encedido a las : ");
 printHora(fechaActual);
}


}
void apagarAmpolleta(time_t horaEncendido){

  
if(horaEncendido == fechaActual){
digitalWrite(r1,LOW);
 Serial.print("Foco Apagado a las : ");
 printHora(fechaActual);

}
}

//time_t rescatarFecha(){
//time_t aux =now();
//aux.setTime(1);

  //return aux;
//}














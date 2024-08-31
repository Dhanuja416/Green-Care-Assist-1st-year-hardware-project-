#include <WiFi.h>
#include "time.h"
#include <TimerOne.h>
/////////////////////////////////temp humidity libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
//////////////////////////////////temp and humidity


// Define the GPIO pin where the DHT22 data pin is connected
#define DHTPIN 4 
// Define the type of DHT sensor
#define DHTTYPE DHT22
// Create a DHT object
DHT dht(DHTPIN, DHTTYPE);

////////////////////////////////////////////soil
// Define the GPIO pin where the soil moisture sensor is connected
#define SOIL_MOISTURE_PIN 33  
// Define the minimum and maximum analog values for dry and wet soil
#define DRY_SOIL 3000     
#define WET_SOIL 4095  

////////////////////////////////////////////////////////relay
// Define GPIO pins for the relay module
#define RELAY_FAN 18
#define RELAY_PELTIER 19
#define RELAY_FANtwo 15
#define RELAY_humidifire 14
#define RELAY_waterPump 12

int fanAc=0;//if fan and Ac on value is 1
int fanhum=0;//if fan2 and humidifire on value is 1
int waterPump=0;
int fanAcCount=0;
int fanhumCount=0;
int waterPumpCount=0;
//////////////////////////////////////////////////

float humi=0;
float interruptHumi;
float temp=0;
float interruptTemp;
float soil=0;
float interruptSoil;
float interruptSoilpercent;
float avghumi;
float avgtemp;
float avgsoil;

// Replace with your network credentials
const char* ssid = "Dhanuja123";
const char* password = "uncy1765";

// NTP server to request time from
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 5 * 3600; // GMT +5 hours
const int   daylightOffset_sec = 30 * 60; // +30 minutes for Sri Lanka

//////////////////////////////////ultrasonic
#define trigPin 32
#define echoPin 34
long distance,duration;
int ultrasonicDetects=0;
//////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  // Initialize and obtain time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Wait for time to be set
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  // Initialize the DHT sensor
  dht.begin();

  //////////////////////////////////////////////
  // Initialize relay pins as outputs
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_PELTIER, OUTPUT);
  pinMode(RELAY_FANtwo, OUTPUT);
  pinMode(RELAY_humidifire, OUTPUT);
  pinMode(RELAY_waterPump, OUTPUT);

  // Turn off all relays initially
  digitalWrite(RELAY_FAN, LOW);
  digitalWrite(RELAY_PELTIER, LOW);
  digitalWrite(RELAY_FANtwo, LOW);
  digitalWrite(RELAY_humidifire, LOW);
  digitalWrite(RELAY_waterPump, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //////////////////////////////////////////

  Timer1.initialize(100000);
  Timer1.attachInterrupt(checkSensor);

}

void loop() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    delay(1000);
    return;
  }

  int minute = timeinfo.tm_min;
  int second = timeinfo.tm_sec;

  Serial.print(timeinfo.tm_hour);
  Serial.print(" . ");
  Serial.print(minute);
  Serial.print(" . ");
  Serial.print(second);

  if (minute % 5 == 3) {
      Serial.println("3-minute interval");
      Serial.println("humididy,temp");
      delay(30);
      // Read temperature as Celsius
      float tempC = dht.readTemperature();
      delay(30);
      // Read humidity
      float humidity = dht.readHumidity();
      // Check if any reads failed and exit early (to try again).
      if (isnan(tempC)){
        Serial.println("Failed to read temp!");
        temp+=28;
      }else{
        temp+=tempC;
      }

      if (isnan(humidity)) {
        Serial.println("Failed to read humidity!");
        humi+=87;
      }else{
        humi+=humidity;
      }

    } else if (minute == 5 || minute == 10 || minute == 20 || minute == 25 || minute == 35 || minute == 40 || minute == 50 || minute == 55 ) {
      Serial.println("5-minute interval");
      Serial.println("soil");
      delay(30);
      // Read the analog value from the soil moisture sensor
      int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
      // Convert the analog value to a percentage
      int soilMoisturePercent = map(soilMoistureValue, WET_SOIL, DRY_SOIL, 100, 0);
        // Ensure the percentage is within the range 0-100
      soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

      if (isnan(soilMoistureValue)){
        Serial.println("Failed to read soil!");
        soil+=40;
      }else{
        soil+=soilMoisturePercent;
      }

    }else if (minute == 15 || minute == 45) {
      Serial.println("15-minute interval");
      Serial.println("soil");
      delay(30);
      // Read the analog value from the soil moisture sensor
      int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
      // Convert the analog value to a percentage
      int soilMoisturePercent = map(soilMoistureValue, WET_SOIL, DRY_SOIL, 100, 0);
        // Ensure the percentage is within the range 0-100
      soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

      if (isnan(soilMoistureValue)){
        Serial.println("Failed to read soil!");
        soil+=40;
      }else{
        soil+=soilMoisturePercent;
      }

      Serial.println("humidity,temp average");
      Serial.println("fan ac on humidity fire");
      
      avghumi=humi/3;
      avgtemp=temp/3;
      humi=0;
      temp=0;
      if(avgtemp>=30.00){
        //fan1 ac cool for 28
        fanAc=1;
        delay(20);
        digitalWrite(RELAY_FAN, HIGH);
        delay(20);
        digitalWrite(RELAY_PELTIER, HIGH);
      }
      if(avghumi<=75.00){
        //fan2 humidi fier on humi 80 off
        fanhum=1;
        delay(20);
        digitalWrite(RELAY_FANtwo, HIGH);
        delay(20);
        digitalWrite(RELAY_humidifire, HIGH);
      }

      
    }else if (minute == 30 || minute == 0) {
      Serial.println("30-minute interval");

      Serial.println("humidity,temp average");
      Serial.println("fan ac on humidity fire");
      
      avghumi=humi/3;
      avgtemp=temp/3;
      humi=0;
      temp=0;
      if(avgtemp>=30.00){
        //fan1 ac cool for 28
        fanAc=1;
        delay(20);
        digitalWrite(RELAY_FAN, HIGH);
        delay(20);
        digitalWrite(RELAY_PELTIER, HIGH);
      }
      if(avghumi<=75.00){
        //fan2 humidi fier on humi 80 off
        fanhum=1;
        delay(20);
        digitalWrite(RELAY_FANtwo, HIGH);
        delay(20);
        digitalWrite(RELAY_humidifire, HIGH);
      }

      Serial.println("water pump and ultra sonic");
      Serial.println("soil average <=35 water motor relay off when>35");

      avgsoil=soil/5;
      soil=0;
      if(avgsoil<=35.00){
        for(int count=0;count<5;count++){
          delay(70);
          digitalWrite(trigPin,LOW);
          delayMicroseconds(2);
          digitalWrite(trigPin,HIGH);
          delayMicroseconds(10); // transmit waves for 10 micro seconds
          digitalWrite(trigPin,LOW);

          duration=pulseIn(echoPin,HIGH); //recieve reflected waves

          distance= duration/58.2; //distance in centimetres," velocity=distance/time " used here with unit conversion, and also dividing by 2 also used since reflection happens (wave hits and reflected back in same path)
          if(distance<=5 && distance >=3){
              ultrasonicDetects++;    
          }
        }
        if(ultrasonicDetects>3)
        {
              waterPump=1;
              delay(20);
              digitalWrite(RELAY_waterPump, HIGH);               
        }
        ultrasonicDetects=0;
      }
    } 

  if (timeinfo.tm_hour>=18 || timeinfo.tm_hour<=8) {
      Serial.println("light on");
    } 
  

}

void checkSensor(){
  if((timeinfo.tm_min>=2 && timeinfo.tm_min<=3 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=4 && timeinfo.tm_min<=5 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=7 && timeinfo.tm_min<=8 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=9 && timeinfo.tm_min<=10 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=12 && timeinfo.tm_min<=13 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=14 && timeinfo.tm_min<=15 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=17 && timeinfo.tm_min<=18 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=19 && timeinfo.tm_min<=20 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=22 && timeinfo.tm_min<=23 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=24 && timeinfo.tm_min<=25 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=27 && timeinfo.tm_min<=28 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=29 && timeinfo.tm_min<=30 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=32 && timeinfo.tm_min<=33 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=34 && timeinfo.tm_min<=35 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=37 && timeinfo.tm_min<=38 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=39 && timeinfo.tm_min<=40 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=42 && timeinfo.tm_min<=43 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=44 && timeinfo.tm_min<=45 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=47 && timeinfo.tm_min<=48 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=49 && timeinfo.tm_min<=50 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=52 && timeinfo.tm_min<=53 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=54 && timeinfo.tm_min<=55 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=57 && timeinfo.tm_min<=58 && timeinfo.tm_sec>50)||(timeinfo.tm_min>=59 && timeinfo.tm_sec>50)){
    Timer1.initialize(30000000);
  }else{
    Timer1.initialize(100000);
  }
  if(fanAc==1){
    interruptTemp=dht.readTemperature();
    if (isnan(interruptTemp)){
        Serial.println("Failed to read temp!");
        interruptTemp=28;
      }
    if(interruptTemp<=28){
      fanAcCount++;
    }else{
      fanAcCount=0;
    }    
  }
  if(fanhum==1){
    interruptHumi=dht.readHumidity();
    if (isnan(interruptHumi)){
        Serial.println("Failed to read humidity!");
        interruptHumi=87;
      }
    if(interruptHumi>=80){
      fanhumCount++;
    }else{
      fanhumCount=0;
    }  
  }
  if(waterPump==1){
      // Read the analog value from the soil moisture sensor
      interruptSoil = analogRead(SOIL_MOISTURE_PIN);
      // Convert the analog value to a percentage
      interruptSoilpercent = map(interruptSoil, WET_SOIL, DRY_SOIL, 100, 0);
      // Ensure the percentage is within the range 0-100
      interruptSoilpercent = constrain(interruptSoilpercent, 0, 100);

      if (isnan(interruptSoil)){
        Serial.println("Failed to read soil!");
        interruptSoilpercent=40;
      }
      
    if(interruptSoilpercent>35){
      waterPumpCount++;
    }else{
      WaterPumpCount=0;
    }  
  }

  if(fanAcCount>3){
    digitalWrite(RELAY_FAN, LOW);
    digitalWrite(RELAY_PELTIER, LOW);
    fanAcCount=0;
  }
  if(fanhumCount>3){
    digitalWrite(RELAY_FANtwo, LOW);
    digitalWrite(RELAY_humidifire, LOW);
    fanhumCount=0;
  }
  if(WaterPumpCount>3){
    digitalWrite(RELAY_waterPump, LOW);
    WaterPumpCount=0;
  }
}


      
      

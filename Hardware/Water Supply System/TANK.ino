#include <OneWire.h>
#include <DallasTemperature.h>
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

#define ONE_WIRE_BUS D5                          //D2 pin of nodemcu
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 

#define FIREBASE_HOST "aquaclear-****-default-rtdb.firebaseio.com"      
#define FIREBASE_AUTH "****"            
#define WIFI_SSID "****"                                  
#define WIFI_PASSWORD "****"


int DSPIN = D5; // Dallas Temperature Sensor
int trigPin = D6;
int echoPin = D7;
int relay = D1;

namespace pin {
const byte tds_sensor = A0;   // TDS Sensor
}
 
namespace device {
float aref = 3.3;
}
 
namespace sensor {
float ec = 0;
unsigned int tds = 0;
float ecCalibration = 1;
} 
 
void setup() {
  Serial.begin(115200); // Dubugging on hardware Serial 0
  sensors.begin();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(relay, INPUT);   //prevent relay from running continuously, output -> relay on, input -> relay on
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                  
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.println();
  Serial.print("Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());                               //prints local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
  Firebase.set("status", 0);
  Firebase.set("forcestart", 0);
  delay(2000);
}
 
 
void loop() {
  int level = getLevel();
  int tds = getTDS();
  sensors.requestTemperatures();            
  int temp = sensors.getTempCByIndex(0);
  
  Serial.print("TDS:"); Serial.println(tds);
  Serial.print("Temperature:"); Serial.println(temp);  
  Serial.print(""); 
  Serial.print("level:");
  Serial.println(level);
  
  Firebase.set("TDS", tds);         
  Firebase.set("temperature", temp);  
  Firebase.set("waterlevel", level);      
  if (Firebase.failed()) {
    Serial.print("pushing /logs failed:");
    Serial.println(Firebase.error()); 
    return;
  }

  if(Firebase.getInt("forcestart") == 1) {
    forceStart();
    Firebase.set("forcestart", 0);
    Firebase.set("start", 0);
  }
  else if(Firebase.getInt("start") == 1) {
    normalMode();
    Firebase.set("start", 0);
  }
  delay(2000);
}

void normalMode() {
  int level = getLevel();
  int motorstatus = Firebase.getInt("start");
  while(level < 100 && motorstatus) {
    Firebase.set("waterlevel", level);
    Serial.println(" ");
    Serial.print("level is: ");
    Serial.println(level);
    int tds = getTDS();
    Firebase.set("TDS", tds);
    if(tds > 400) { 
      pinMode(relay, INPUT);
      Firebase.set("notify", 1);
      Firebase.set("start", 0);
      Firebase.set("motorStatus", 0);
      //set start to 0 in db
      Serial.println("TDS level above acceptable limit, turning off the motor...");
      break;
    }
    delay(100);
    Firebase.set("motorStatus", 1);
    pinMode(relay, OUTPUT);
    delay(5000);
    pinMode(relay, INPUT);
    delay(2000);
    level = getLevel();
    if(level > 90) {
      pinMode(relay, INPUT);
      Firebase.set("tankfull", 1);
      Firebase.set("start", 0);
      Firebase.set("motorStatus", 0);
      Serial.println("Tank is almost full!");
      break;
    }
    motorstatus = Firebase.getInt("start");
    delay(100);
  }
  pinMode(relay, INPUT);
  Firebase.set("start", 0);
  Firebase.set("motorStatus", 0);
}

void forceStart() {
  int level = getLevel();
  int motorstatus = Firebase.getInt("forcestart");
  while(level < 100) {
    Firebase.set("waterlevel", level);
    Serial.println(" ");
    Serial.print("level is: ");
    Serial.println(level);

    Firebase.set("motorStatus", 1);
    pinMode(relay, OUTPUT);
    delay(4000);
    pinMode(relay, INPUT);
    delay(2000);

    level = getLevel();
    if(level > 90) {
      pinMode(relay, INPUT);
      Firebase.set("tankfull", 1);
      Firebase.set("forcestart", 0);
      Firebase.set("start", 0);
      Firebase.set("motorStatus", 0);
      Serial.println("Tank is almost full!");
      break;
    }
    motorstatus = Firebase.getInt("forcestart");
    delay(100);
  }
  pinMode(relay, INPUT);
  Firebase.set("forcestart", 0);
  Firebase.set("start", 0);
  Firebase.set("motorStatus", 0);
}

double getTDS() {
  //int waterTemp = TempRead();
  //waterTemp  = waterTemp*0.0625; // conversion accuracy is 0.0625 / LSB
  sensors.requestTemperatures();                // Send the command to get temperatures  
  //Serial.println("Temperature is: ");
  //Serial.println(sensors.getTempCByIndex(0));   // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  //delay(500);
  double waterTemp = sensors.getTempCByIndex(0);
  double rawEc = analogRead(pin::tds_sensor) * device::aref / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  double temperatureCoefficient = 1.0 + 0.02 * (waterTemp - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  
  sensor::ec = (rawEc / temperatureCoefficient) * sensor::ecCalibration; // temperature and calibration compensation
  sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5; //convert voltage value to tds value

  //Serial.print(F("TDS:")); Serial.println(sensor::tds);
  //Serial.print(F("Temperature:")); Serial.println(waterTemp,2);  
  //Serial.print(F("")); 

  return sensor::tds;
}

int getLevel() {
  long duration;
  double distance;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distance = duration * SOUND_VELOCITY/2;
  
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance (cm): ");
  //Serial.println(distance);
  
  double level = 8.5 - distance;
  if(level < 0) level = 0;
  if(level > 4) level = 4;

  double normalized = ((level - 0)/4.0)*100;
  return normalized;
}

boolean DS18B20_Init() {
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(DSPIN, LOW);
  delayMicroseconds(750);//480-960
  digitalWrite(DSPIN, HIGH);
  pinMode(DSPIN, INPUT);
  int t = 0;
  while(digitalRead(DSPIN)) {
    t++;
    if(t > 60) return false;
    delayMicroseconds(1);
  }
  t = 480 - t;
  pinMode(DSPIN, OUTPUT);
  delayMicroseconds(t);
  digitalWrite(DSPIN, HIGH);
  return true;
}
 
void DS18B20_Write(byte data) {
  pinMode(DSPIN, OUTPUT);
  for(int i=0; i<8; i++) {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(10);
    if(data & 1) digitalWrite(DSPIN, HIGH);
    else digitalWrite(DSPIN, LOW);
    data >>= 1;
    delayMicroseconds(50);
    digitalWrite(DSPIN, HIGH);
  }
}
 
byte DS18B20_Read() {
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(2);
  byte data = 0;
  for(int i=0; i<8; i++) {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(1);
    digitalWrite(DSPIN, HIGH);
    pinMode(DSPIN, INPUT);
    delayMicroseconds(5);
    data >>= 1; 
    if(digitalRead(DSPIN)) data |= 0x80;
    delayMicroseconds(55);
    pinMode(DSPIN, OUTPUT);
    digitalWrite(DSPIN, HIGH);
  }
  return data;
}
 
int TempRead() {
  if(!DS18B20_Init()) return 0;
   DS18B20_Write (0xCC); // Send skip ROM command
   DS18B20_Write (0x44); // Send reading start conversion command
  if(!DS18B20_Init()) return 0;
   DS18B20_Write (0xCC); // Send skip ROM command
   DS18B20_Write (0xBE); // Read the register, a total of nine bytes, the first two bytes are the conversion value
   int waterTemp = DS18B20_Read (); // Low byte
   waterTemp |= DS18B20_Read () << 8; // High byte
  return waterTemp;
}

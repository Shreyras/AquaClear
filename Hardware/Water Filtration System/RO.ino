#include <OneWire.h>
#include <DallasTemperature.h>
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

#define ONE_WIRE_BUS D5                          //D5 pin of nodemcu
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
int led = D2;
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
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
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
  //Firebase.set("status", 0);
  //Firebase.set("forcestart", 0);
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
  
  Firebase.set("ROTDS", tds);         
  Firebase.set("ROtemperature", temp);  
  Firebase.set("ROwaterlevel", level);      
  if (Firebase.failed()) {
    Serial.print("pushing /logs failed:");
    Serial.println(Firebase.error()); 
    return;
  }

  if(Firebase.getInt("ROstart") == 1) {
    if(level < 20) {
      if(Firebase.getInt("forceRO") == 1) {
        ROmode();
      }
      else {
        if(tds < 100) normalmode();
        else ROmode();
      }
    }
  }
  delay(3000);
}

void normalmode() {
  digitalWrite(led, LOW);
  int level = getLevel();
  int motorstatus = Firebase.getInt("ROstart");
  while(level < 100 && motorstatus) {
    if(Firebase.getInt("forceRO") == 1) {
      Firebase.set("ROMode", 1);
      ROmode();
    }
    int tds = getTDS();
    sensors.requestTemperatures(); 
    int temp = sensors.getTempCByIndex(0);
    Firebase.set("ROStatus", 1);
    Firebase.set("ROMode", 0);
    Firebase.set("ROwaterlevel", level);
    Firebase.set("ROTDS", tds);
    Firebase.set("ROtemperature", temp);

    pinMode(relay, OUTPUT);
    delay(4000);
    pinMode(relay, INPUT);
    delay(2000);
    
    level = getLevel();
    if(level > 90) {
      pinMode(relay, INPUT);
      Firebase.set("ROStatus", 0);
      break;
    }
    motorstatus = Firebase.getInt("ROstart");
  }
  pinMode(relay, INPUT);
  Firebase.set("ROStatus", 0);
}

void ROmode() {
  digitalWrite(led, HIGH);
  int level = getLevel();
  int motorstatus = Firebase.getInt("ROstart");
  while(level < 100 && motorstatus) {
    int tds = getTDS();
    sensors.requestTemperatures();
    int temp = sensors.getTempCByIndex(0);

    Firebase.set("ROStatus", 1);
    Firebase.set("ROMode", 1);
    Firebase.set("ROwaterlevel", level);
    Firebase.set("ROTDS", tds);
    Firebase.set("ROtemperature", temp);
    
    pinMode(relay, OUTPUT);
    delay(4000);
    pinMode(relay, INPUT);
    delay(2000);

    level = getLevel();
    if(level > 90) {
      pinMode(relay, INPUT);
      Firebase.set("ROStatus", 0);
      digitalWrite(led, LOW);
      break;
    }
    motorstatus = Firebase.getInt("ROstart");
  }
  pinMode(relay, INPUT);
  Firebase.set("ROStatus", 0);
  digitalWrite(led, LOW);
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
  Serial.print("Distance (cm): ");
  Serial.println(distance);
  
  double level = 8.5 - distance;
  if(level < 0) level = 0;
  if(level > 4) level = 4;

  double normalized = ((level - 0)/4.0)*100;
  return normalized;
}


 

 

 

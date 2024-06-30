//

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "hidden"
#define WIFI_PASSWORD "1onequestionmark?"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCSa91zZdJD-hyTkXas22i8XQUIaeibYd0"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://linelifemain-default-rtdb.asia-southeast1.firebasedatabase.app/" 

#include <MQ135.h>
// Include the required libraries


int R0 = 176;
int R2 = 1000;
float RS;
float PPM_acetone;
const int mq135Pin = A0;
MQ135 gasSensor = MQ135(mq135Pin);
const float ammoniaR0 = 150;
float aC;
float cH4;
float cO;
float h2S;
float nH4;




const int mq5Pin = A5;
int myled=7;
int buzzer=12;


//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;





unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup(){
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
 
  
   pinMode(myled,OUTPUT);
    pinMode(buzzer,OUTPUT);
  
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 150 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
      
    int sensorValue = analogRead(A0);
    
    int smqv=analogRead(A5);
    
    // read the input on analog pin 0:
    // convert to voltage:
    float volts = sensorValue * 5;
    volts = volts / 1023;
    // calculate RS
    RS = R2 * (1-volts);
    RS = RS/volts;
    // calculate acetone PPM
    PPM_acetone = 159.6 - 133.33*(RS/R0);
    // print out the acetone concentration:
    Serial.println("The amount of acetone (in PPM): ");
    Serial.println(PPM_acetone);
    aC=PPM_acetone;
    
    
    
    

    
    
    
    if (Firebase.RTDB.setFloat(&fbdo, "GASESS/acetone", double(aC))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  

  int cosensorValue = analogRead(A0);
  Serial.println("The amount of CO2 (in PPM): ");
  Serial.println(cosensorValue);
  h2S=cosensorValue;
      if (Firebase.RTDB.setFloat(&fbdo, "GASESS/H2S", double(h2S+0.01))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

  int rawValue = analogRead(mq135Pin);

  // Convert the raw analog value to ammonia concentration using the calibration factor
  float ppmAmmonia = gasSensor.getPPM();
  // You can also get other values like resistance, R0, etc., if needed.

  // Print the ammonia concentration to the serial monitor
  Serial.print("Ammonia Concentration (PPM): ");
  Serial.println(ppmAmmonia);
  nH4=ppmAmmonia; 
    if (Firebase.RTDB.setFloat(&fbdo, "GASESS/nH4",double(nH4))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    //.................................................

    int RrawValue = analogRead(mq5Pin);

  // Convert raw value to gas concentrations (adjust these values based on your sensor)
  float ppmH2 = map(RrawValue, 0, 1023, 0, 1000); // Hydrogen (H2)
  float ppmLPG = map(RrawValue, 0, 1023, 0, 5000); // LPG
  float ppmCH4 = map(RrawValue, 0, 1023, 0, 5000); // Methane (CH4)
  float ppmCO = map(RrawValue, 0, 1023, 0, 1000); // Carbon Monoxide (CO)
   
  
  // Print gas concentrations to the serial monitor
  Serial.print("Hydrogen (H2) Concentration (PPM): ");
  Serial.println(ppmH2);
    if (Firebase.RTDB.setFloat(&fbdo, "GASESS/ppmH2",double(ppmH2+0.1))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }


  Serial.print("LPG Concentration (PPM): ");
  Serial.println(ppmLPG);
    if (Firebase.RTDB.setFloat(&fbdo, "GASESS/ppmLPG",double(ppmLPG+0.1))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }


  Serial.print("Methane (CH4) Concentration (PPM): ");
  Serial.println(ppmCH4);
  cH4=ppmCH4;
      if (Firebase.RTDB.setFloat(&fbdo, "GASESS/cH4",double(cH4+0.1))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }




  Serial.print("Carbon Monoxide (CO) Concentration (PPM): ");
  Serial.println(ppmCO);
  cO=ppmCO;
        if (Firebase.RTDB.setFloat(&fbdo, "GASESS/cO",double(cO+0.1))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
     if(PPM_acetone<0)
  {
      
  digitalWrite(myled, HIGH);
   digitalWrite(buzzer, LOW);// turn the LED on (HIGH is the voltage level)
  
  
  }
  else
   {
   //wait for a second
 

   
   digitalWrite(myled, LOW);
   digitalWrite(buzzer, HIGH); 
 
  }



  }

}

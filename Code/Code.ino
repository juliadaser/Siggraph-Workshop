// Importing libraries
#include <WiFi.h>                 // to Connect the ESP32 to Wifi
#include <Firebase_ESP_Client.h>  // to interface the ESP32 with Firebase

// We need this for the firebase library to work
#include "addons/TokenHelper.h"  //Provide the token generation process info.
#include "addons/RTDBHelper.h"   //Provide the RTDB payload printing info and other helper functions.

// Wi-Fi credentials
#define WIFI_SSID "Lab181"
#define WIFI_PASSWORD "lovemessengers"

// Firebase credentials
#define API_KEY "insert API Key here"
#define DATABASE_URL "insert Database URL here"

// Hardware components (LED & button)
const int ledPin = 4;
const int buttonPin = 14;

// Global variables
bool firebaseData = false;
int buttonState = 1;

// Define Firebase Data object and other necessary objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long lastFirebaseUpdate = 0;
int count = 0;
bool signupOK = false;

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  connectWiFi();
  connectFirebase();
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - lastFirebaseUpdate > 2000)) {  // adding a timer, so that we dont overwhelm the database with requests
    lastFirebaseUpdate = millis();

    buttonState = digitalRead(buttonPin);
    uploadData(buttonState);
    downloadData();
    manageLED(buttonState, firebaseData);
  }
  delay(5);
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to Wifi...");
    delay(300);
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

void connectFirebase() {
  config.api_key = API_KEY;            // Assign the API key
  config.database_url = DATABASE_URL;  // Assign the Realtime Database URL

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase successful");
    signupOK = true;  // if sign up is successful we change this variable
  } else {
    Serial.printf("Firebase error");
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void uploadData(int buttonstate) {
  if (Firebase.RTDB.setInt(&fbdo, "/user_1", buttonstate)) {
    Serial.printf("Data UPLOAD successful,  ");
  } else {
    Serial.println("Data UPLOAD failed,  ");
  }
}

void downloadData() {
  if (Firebase.RTDB.getInt(&fbdo, "/user_2")) {
    firebaseData = fbdo.intData();
    Serial.println("Data DOWNLOAD successful");
  } else {
    Serial.println("Data DOWNLOAD failed");
  }
}

void manageLED(int buttonState, int firebaseData) {
  if (buttonState == LOW or firebaseData == LOW) {
    digitalWrite(ledPin, HIGH);
    delay(2000);
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Wi-Fi credentials
const char* ssid = "Lowkey2.4";
const char* password = "chukidaymee";

// Firebase Configuration
#define FIREBASE_HOST "host-url (vercel url is used for deployment)"
#define API_KEY "firebase-api-key"  // Found in Firebase Console -> Project Settings -> General -> Web API Key
#define USER_EMAIL "firebase-user2-email"           // Replace with the email for the Firebase user
#define USER_PASSWORD "firebase-3-password"          // Replace with the password for the Firebase user

// Firebase objects
FirebaseData fbdo;  // Firebase Data Object
FirebaseAuth auth;  // Firebase Authentication Object
FirebaseConfig config;  // Firebase Configuration Object

// Pin configuration
const int LED_PIN = 5;  // LED connected to GPIO 5
const int BUTTON_PIN = 4;  // Button connected to GPIO 4

void setup() {
  Serial.begin(115200);

  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to Wi-Fi");

  // Configure Firebase
  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Firebase initialized and authenticated.");
  } else {
    Serial.println("Failed to initialize Firebase:");
    Serial.println(fbdo.errorReason());
  }
}

void loop() {
  // Read and process FirstOrbState
  if (Firebase.RTDB.getInt(&fbdo, "/FirstOrbState")) {
    int state = fbdo.intData();
    if (state == 1) {
      Serial.println("FirstOrbState = 1. Turning LED on...");
      digitalWrite(LED_PIN, HIGH);  // Turn on LED
      delay(5000);                 // Keep LED on for 5 seconds
      digitalWrite(LED_PIN, LOW);  // Turn off LED
      Serial.println("LED turned off");

      // Reset FirstOrbState to 0
      Firebase.RTDB.setInt(&fbdo, "/FirstOrbState", 0);
    }
  } else {
    Serial.println("Error reading FirstOrbState: " + fbdo.errorReason());
  }

  // Check button press and update SecondOrbState
  if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed
    Serial.println("Button pressed. Updating SecondOrbState...");
    Firebase.RTDB.setInt(&fbdo, "/SecondOrbState", 1);
    delay(5000);
    Firebase.RTDB.setInt(&fbdo, "/SecondOrbState", 0);
    Serial.println("SecondOrbState reset to 0");
  }

  delay(100);  // Avoid busy looping
}

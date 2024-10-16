#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SPI.h>
#include <MFRC522.h>

//hidden code id : 1
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define DATABASE_URL ""

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const int relayPin = D2;
const int buzzerPin = D1;

const int wifiWaitTime = 15000;

#define SS_PIN D4
#define RST_PIN -1
MFRC522 rfid(SS_PIN, RST_PIN);

String cardID = "";

void beep(int type)
{
  if (type == 1)
  {
    digitalWrite(buzzerPin, HIGH);
    delay(250);
    digitalWrite(buzzerPin, LOW);
    delay(250);
    digitalWrite(buzzerPin, HIGH);
    delay(250);
    digitalWrite(buzzerPin, LOW);
    delay(250);
  }
  else if (type == 2)
  {
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }
}
void setup()
{
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < wifiWaitTime)
  {
    Serial.print(".");
    delay(300);
    i += 300;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
  if (i >= wifiWaitTime)
  {
    Serial.println();
    Serial.println("Connection failed");
    Serial.println();
  }

  config.signer.test_mode = true;
  config.database_url = DATABASE_URL;
  //hiden code id : 2
  config.signer.tokens.legacy_token = "";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  SPI.begin();     // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 module
  Serial.println("Scan an RFID tag...");
}
//rfid card scanning and pushing uid to firebase
void loop() {
  if(rfid.PICC_IsNewCardPresent()&&rfid.PICC_ReadCardSerial())
  {
    String cardID= getCardUID();
    Serial.println("Card Uid : "+cardID);
  if (Firebase.pushString(firebaseData, "/rfid_cards", cardID)) 
  {
    Serial.println("Pushed to Firebase successfully");
  }
  else
  {
    Serial.println("Failed to push to Firebase");
    Serial.println("REASON: " + firebaseData.errorReason());
  }
  delay(1000);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  //if card is present access grant part 
}
}
//converts to string 
String getCardUID()
{
  String uidString="";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    cardID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    cardID += String(rfid.uid.uidByte[i], HEX);
  }
  cardID.toUpperCase();
  return cardID;
}

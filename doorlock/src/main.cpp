int cardCount=0;
void loop(){
 if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) 
 {
    String cardUID = "";  
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
        cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");  
        cardUID += String(mfrc522.uid.uidByte[i], HEX); 
    }
cardUID.toUpperCase();  
Serial.println("Card detected, UID: " + cardUID);
cardCount++;
if (Firebase.pushString(firebaseData, "/RFID_Logs/" + String(cardCount), cardUID)) {
    Serial.println("UID pushed to Firebase: " + cardUID);
} 
else 
{
    Serial.println("Failed to push UID to Firebase");
    Serial.println("Reason: " + firebaseData.errorReason());
}
mfrc522.PICC_HaltA();
mfrc522.PCD_StopCrypto1();
delay(3000); 
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
 
 
#define bleServerName "ESP_32_BT_BAROIU_SILVIAN"
bool deviceConnected = false;
 
#define SERVICE_UUID "347d021a-d15c-4d15-8065-a0fcf77452f1"
#define CHARACTERISTIC_UUID "cbc94826-eee6-4334-acce-a3ca63239bf4"
 
 
 
BLECharacteristic characteristic(
  CHARACTERISTIC_UUID,
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
);
BLEDescriptor *characteristicDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
 
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};
 
class CharacteristicsCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    std::string data = characteristic->getValue();
    Serial.println(data.c_str()); // <-- This is the message sent from the app, according to the specs
 
    DynamicJsonDocument doc(256);
    deserializeJson(doc, data);
 
    std::string action = doc["action"];
    std::string teamId = doc["teamId"];
    if (action == "getNetworks") {
      DynamicJsonDocument response(1024);
 
      // Code to get list of Wi-Fi networks in vicinity
      int numNetworks = WiFi.scanNetworks();
 
      // Add the Wi-Fi networks to the response
      for (int i = 0; i < numNetworks; i++) {
        JsonObject network = response.createNestedObject();
        network["ssid"] = WiFi.SSID(i);
        network["strength"] = WiFi.RSSI(i);
        network["encryption"] = WiFi.encryptionType(i);
        network["teamId"] = teamId;
        std::string jsonResponse;
        serializeJson(network, jsonResponse);
        characteristic->setValue(jsonResponse.c_str());
        characteristic->notify();
      }
    }
    const char* ssid = doc["ssid"];
    const char* password = doc["password"];
    if (action == "connect")
    { 
        std::string ssid = doc["ssid"].as<std::string>();
        std::string password = doc["password"].as<std::string>();
        StaticJsonDocument<64> response;
        response["teamId"] = teamId;
        response["ssid"] = ssid;
        WiFi.begin(ssid.c_str(), password.c_str());
        int tries = 0;
        while (WiFi.status() != WL_CONNECTED && tries < 10) {
          delay(1000);
          tries++;
        }
        if (WiFi.status() == WL_CONNECTED) {
          response["connected"] = true;
        } else {
          response["connected"] = false;
        }
        std::string responseData;
        serializeJson(response, responseData);
        characteristic->setValue(responseData.c_str());
        characteristic->notify();
    }
     if( action == "getData")
    {
 
      {
      // Access the API and send back the data
      HTTPClient http;
      http.begin("http://proiectia.bogdanflorea.ro/api/avatar-the-last-airbender/characters");
 
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String api_response = http.getString();
        // Parse the API response
        const size_t capacity = JSON_ARRAY_SIZE(17) + 17 * JSON_OBJECT_SIZE(4) + 500;
        DynamicJsonDocument api_doc(15000);
 
        // Deserialize the JSON std::string
        DeserializationError error = deserializeJson(api_doc, api_response);
 
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }
 
        // Access each object in the JSON array
        JsonArray objects = api_doc.as<JsonArray>();
        for (JsonObject obj : objects) {
          // Extract the values from each object
          const char* id = obj["_id"];
          const char* name = obj["name"];
          const char* imagePath = obj["photoUrl"];
 
          // Create a new object with the desired type
          JsonObject newObject = doc.to<JsonObject>();
          newObject["id"] = id;
          newObject["name"] = name;
          newObject["image"] = imagePath;
          newObject["teamId"] = "B02";  // Replace with your desired team ID
 
          // Convert the new object to a std::string
          std::string result;
          serializeJson(newObject, result);
 
          // Print the result
          characteristic->setValue(result.c_str());
          characteristic->notify();
        }
        http.end();
      }
    }
  }
 
  if (action == "getDetails") {
  String id = doc["id"];

  // Access the API and send back the data
  HTTPClient http;
  http.begin("http://proiectia.bogdanflorea.ro/api/avatar-the-last-airbender/character?_id=" + id);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String api_response = http.getString();
    // Parse the API response
    const size_t capacity = JSON_ARRAY_SIZE(17) + 17 * JSON_OBJECT_SIZE(4) + 500;
    DynamicJsonDocument api_doc(15000);

    // Deserialize the JSON string
    DeserializationError error = deserializeJson(api_doc, api_response);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Access the JSON object
    JsonObject obj = api_doc.as<JsonObject>();

    // Extract the values from the JSON object
    int char_id = obj["char_id"];
    const char* photourl = obj["photoUrl"];
    const char* name = obj["name"];

    String affiliation;
    if (obj.containsKey("affiliation")) {
      affiliation = obj["affiliation"].as<String>();
    } else {
      affiliation = "Non-existant";
    }

    // Create a new JSON object for the response
    DynamicJsonDocument responseDoc(JSON_OBJECT_SIZE(5) + JSON_ARRAY_SIZE(17) + 17 * JSON_OBJECT_SIZE(2) + capacity);
    JsonObject newObject = responseDoc.to<JsonObject>();

    // Assign values to the new object
    newObject["id"] = char_id;
    newObject["name"] = name;
    newObject["image"] = photourl;
    newObject["teamId"] = "B02";

    const JsonArray& alliesArray = obj["allies"];
    String allies;
    for (const JsonVariant& value : alliesArray) {
      allies += value.as<String>() + ", ";
    }
    allies.remove(allies.length() - 2, 2);
    const JsonArray& enemiesArray = obj["enemies"];
    String enemies;
    for (const JsonVariant& value : enemiesArray) {
      enemies += value.as<String>() + ", ";
    }
    enemies.remove(enemies.length() - 2, 2);

    // Format the description string
    char descriptionBuffer[500];
    sprintf(descriptionBuffer, "Allies: %s.\nEnemies: %s.\nName: %s.\nAffiliation: %s.", allies.c_str(), enemies.c_str(), name, affiliation.c_str());
    newObject["description"] = descriptionBuffer;

    // Convert the new object to a string
    String result;
    serializeJson(newObject, result);

    // Set the value and notify the characteristic
    characteristic->setValue(result.c_str());
    characteristic->notify();

    // Clean up
    http.end();
  }
}

 
  }
};
 
void setup() {
  Serial.begin(115200);
  BLEDevice::init(bleServerName);
 
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *bleService = pServer->createService(SERVICE_UUID);
  bleService->addCharacteristic(&characteristic);  
  characteristic.addDescriptor(characteristicDescriptor);
  characteristic.setCallbacks(new CharacteristicsCallbacks());
  bleService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}
 
void loop() {
  // Nothing to do here
}
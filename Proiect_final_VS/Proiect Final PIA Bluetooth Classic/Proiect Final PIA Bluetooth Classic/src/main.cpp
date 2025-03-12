#include <BluetoothSerial.h>
#include <WiFi.h>
#include <unordered_set>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Arduino.h>
struct StringHash {
    std::size_t operator()(const String& str) const {
        std::hash<std::string> hash_fn;
        return hash_fn(std::string(str.c_str()));
    }
};
 
BluetoothSerial SerialBT;
 
// Flag to indicate whether the program has received the first message or not
bool received_first_message = false;
 
// Flag to indicate whether the network list has been sent or not
bool network_list_sent = false;
 
std::unordered_set<String, StringHash> sent_networks;
 
void connectToNetwork(const String& ssid, const String& password) {
  // Check if already connected to the specified network
  if (WiFi.SSID() == ssid) {
    Serial.println("Already connected to the specified network.");
    return;
  }
 
  // Disconnect from the current network
  WiFi.disconnect(true);
 
  // Connect to the specified network
  WiFi.begin(ssid.c_str(), password.c_str());
 
  // Wait for the connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  if(WiFi.status() == WL_CONNECTED)
  Serial.println("Connected!");
 
  // Connection successful
}
 
void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT_Serial");
  WiFi.mode(WIFI_STA);
}
 
void loop() {
  // Wait for incoming Bluetooth data
  while (!SerialBT.available()) {
    delay(10);
  }
 
  // Read the incoming data
  String input = SerialBT.readString();
 
  // Parse the JSON object
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, input);
 
  // If parsing is successful, print the parsed data and set the flag to true
  if (!error) {
    String action = doc["action"];
    if (action == "connect") {
      String ssid = doc["ssid"];
      String password = doc["password"];
      Serial.printf("Received connection request for SSID: %s\n", ssid.c_str());
      connectToNetwork(ssid, password);
 
      // Create a JSON object to hold the connection status
      DynamicJsonDocument status_doc(1024);
      JsonObject status = status_doc.to<JsonObject>();
      status["ssid"] = ssid;
      status["connected"] = WiFi.status() == WL_CONNECTED;
      status["teamId"] = "B03";
 
      // Convert the status object to a string without indentation and line breaks
      String status_json;
      serializeJson(status, status_json);
 
      // Send the serialized status string over Bluetooth
      SerialBT.println(status_json);
      Serial.println(status_json);
    } else if (action == "getData") {
      // Access the API and send back the data
      HTTPClient http;
      http.begin("http://proiectia.bogdanflorea.ro/api/the-wolf-among-us/characters");
 
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
 
        // Access each object in the JSON array
        JsonArray objects = api_doc.as<JsonArray>();
        for (JsonObject obj : objects) {
          // Extract the values from each object
          int id = obj["id"];
          const char* name = obj["name"];
          const char* imagePath = obj["imagePath"];
 
          // Create a new object with the desired type
          JsonObject newObject = doc.to<JsonObject>();
          newObject["id"] = id;
          newObject["name"] = name;
          newObject["image"] = imagePath;
          newObject["teamId"] = "B03";  // Replace with your desired team ID
 
          // Convert the new object to a string
          String result;
          serializeJson(newObject, result);
 
          // Print the result
          Serial.println(result);
          SerialBT.println(result);
        }
        http.end();
      }
      network_list_sent = true;
    }
 
 
 
 
    if(action=="getDetails")
    {
      String ID=doc["id"];
        // Access the API and send back the data
      Serial.println(ID);
      HTTPClient http;
      http.begin("http://proiectia.bogdanflorea.ro/api/the-wolf-among-us/character?id="+ID);
 
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String api_response = http.getString();
        // Parse the API response
        Serial.println(api_response);
        const size_t capacity = JSON_ARRAY_SIZE(17) + 17 * JSON_OBJECT_SIZE(4) + 500;
        DynamicJsonDocument api_doc(15000);
 
        // Deserialize the JSON string
        DeserializationError error = deserializeJson(api_doc, api_response);
 
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }
 
        // Access each object in the JSON array
        JsonObject object = api_doc.as<JsonObject>();
 
          // Extract the values from each object
          int id = object["id"];
          const char* name = object["name"];
          String species=object["species"];
          String gender=object["gender"];
          String occupation=object["occupation"];
          String hairColour=object["hairColour"];
          String eyeColour=object["eyeColour"];
          String description=object["description"];
          const char* imagePath = object["imagePath"];
          String type=object["type"];
 
          // Create a new object with the desired type
          JsonObject newObject = doc.to<JsonObject>();
          newObject["id"] = id;
          newObject["name"] = name;
          newObject["image"] = imagePath;
          newObject["description"]="Species: "+species+"\n"+"Gender: "+gender+"\n"+"Occupation: "+occupation+"\n"+"Hair Colour: "+hairColour+"\n"+"Eye Colour: "+eyeColour+"\n"+"Type: "+type+"\n"+"Description:\n"+description;
          newObject["teamId"] = "B03";  // Replace with your desired team ID
 
          // Convert the new object to a string
          String result;
          serializeJson(newObject, result);
 
          // Print the result
          Serial.println(result);
          SerialBT.println(result);
 
        http.end();
      }
      network_list_sent = true;
 
 
 
 
    }
 
 
 
 
    received_first_message = true;
  }
 
    // If the network list has not been sent and the first message has been received
  if (!network_list_sent && received_first_message) {
    // Scan for available Wi-Fi networks
    int num_networks = WiFi.scanNetworks();
 
    // Create a JSON object to hold the network data
    DynamicJsonDocument network_doc(1024);
 
    // Add each network as a separate object to the JSON document
    for (int i = 0; i < num_networks; i++) {
      String ssid = WiFi.SSID(i);
 
      // Skip the network if it has already been sent
      if (sent_networks.count(ssid) > 0) {
        continue;
      }
 
      sent_networks.insert(ssid);
 
      JsonObject network = network_doc.createNestedObject();
      network["ssid"] = ssid;
      network["strength"] = WiFi.RSSI(i);
      network["encryption"] = WiFi.encryptionType(i);
      network["teamId"] = "B03";
 
      // Convert the network object to a string without indentation and line breaks
      String network_json;
      serializeJson(network, network_json);
 
      // Send the serialized network string over Bluetooth
      SerialBT.println(network_json);
      Serial.println(network_json);
    }
 
    // Set the flag to indicate that the network list has been sent
    network_list_sent = true;
  }
}
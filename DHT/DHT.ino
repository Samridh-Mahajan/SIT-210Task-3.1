#include <DHT.h>
#include <WiFiNINA.h>

// Replace with your WiFi credentials
char ssid[] = "hotspot";
char pass[] = "123456781";

// ThingSpeak settings
char server[] = "api.thingspeak.com";
String apiKey = "SAFH5DO52SG1HYV0";

// Pin configuration for DHT11 sensor
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Initialize the WiFi client
WiFiClient client;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    delay(10000); // Adjust delay as needed
  }

  Serial.println("Connected to WiFi");
  dht.begin(); // Initialize the DHT sensor
}

void loop() {
  // Read temperature and humidity from DHT11 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again)
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Prepare the data to send to ThingSpeak
  String data = "api_key=" + apiKey;
  data += "&field1=" + String(temperature);
  data += "&field2=" + String(humidity);

  // Make an HTTP POST request to ThingSpeak
  if (client.connect(server, 80)) {
    client.print("POST /update HTTP/1.1\r\n");
    client.print("Host: " + String(server) + "\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.print("Content-Length: " + String(data.length()) + "\r\n");
    client.print("\r\n");
    client.print(data);
    
    Serial.println("Data sent to ThingSpeak!");
  } else {
    Serial.println("Connection to ThingSpeak failed!");
  }

  // Wait for response and close the connection
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  }
  client.stop(); // Close the connection

  // Wait before sending the next set of data
  delay(15000); // Adjust delay as needed
}

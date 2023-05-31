#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <esp_heap_caps.h>

// Set WiFi Name and Password (ganti nama wifi)
const char* ssid = "Daven";
const char* password = "daven123";

// Set MQTT Address
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
struct timespec start, end;

// WiFi Setup on ESP32
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } 
    
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// GPS Settings

// Define hardware serial port for GPS module
HardwareSerial SerialGPS(2);

// Define TinyGPS++ object
TinyGPSPlus gps;

void setup() {
  // Initialize hardware serial port for GPS module
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  delay(1000);
  
  // Initialize serial communication for debugging
  Serial.begin(115200);
  while (!Serial);

  // Setup WiFi and MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Print message to indicate successful initialization
  Serial.println("GPS module initialized.");
}

void loop() {
  // Reconnect if connection lost
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read data from GPS module
  while (SerialGPS.available()) {
    char c = SerialGPS.read();
    if (gps.encode(c)) {
      // Publish Latitude and Longitude to MQTT
      double lat = gps.location.lat();
      double lng = gps.location.lng();

      snprintf (msg, MSG_BUFFER_SIZE, "%lf,%lf", lat, lng);
      client.publish("servlatlon123", msg);

      // Print GPS data to serial monitor
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);

      uint32_t freeHeapBytes = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
      uint32_t totalHeapBytes = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
      float percentageHeapFree = freeHeapBytes * 100.0f / (float)totalHeapBytes;

      // Print to serial
      Serial.printf("[Memory] %.1f%% free - %d of %d bytes free\n", percentageHeapFree, freeHeapBytes, totalHeapBytes);
      Serial.println();
    }
  }
}

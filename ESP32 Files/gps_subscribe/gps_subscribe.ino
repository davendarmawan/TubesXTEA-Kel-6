#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <esp_heap_caps.h>

// Set WiFi Name and Password (Ganti nama wifi dan password)
const char* ssid = "Daven";
const char* password = "daven123";

// Set MQTT Address
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
char data[MSG_BUFFER_SIZE];
char latlng[MSG_BUFFER_SIZE];
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
      client.subscribe("servlatlon123");
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

// Haversine Formula
double d(double r, double lat_1, double lat_2, double lng_1, double lng_2) {
    // Konversi derajat ke radians
    double r_lat1 = lat_1 * M_PI / 180;
    double r_lat2 = lat_2 * M_PI / 180;

    double r_lng1 = lng_1 * M_PI / 180;
    double r_lng2 = lng_2 * M_PI / 180;

    // Rumus Menghitung jarak
    double d = 2 * r * asin(sqrt(pow(sin((r_lat2 - r_lat1) / 2), 2) + (cos(r_lat1) * cos(r_lat2) * pow(sin((r_lng2 - r_lng1) / 2), 2))));
    return d * 1000;
}

#define buzzer 21
double lat2 = 0;
double lng2 = 0;

// Acquiring MQTT Data
void callback(char* topic, byte* payload, unsigned int length) {
  char in_comm;
  double lat, lng;
  char *token;

  snprintf (data, MSG_BUFFER_SIZE, "");

  for (int i = 0; i < length; i++) {
    in_comm = (char)payload[i];
    strncat(data, &in_comm, 1);
  }

  // Coordinate parsing and decipher
  token = strtok(data, ",");
  lat = atof(token);

  token = strtok(NULL, "\0");
  lng = atof(token);

  snprintf (msg, MSG_BUFFER_SIZE, "%lf, %lf", lat, lng);

  // Read GPS Data

  // Print MQTT Data to Serial Monitor
  Serial.print("Data from MQTT:");
  Serial.println();
  Serial.println(msg);
  Serial.println();

  // Print GPS Data to Serial Monitor
  Serial.print("Data from GPS:");
  Serial.println();
  Serial.print("Latitude: ");
  Serial.println(lat2, 6);
  Serial.print("Longitude: ");
  Serial.println(lng2, 6);
  Serial.println();

  // Count and Print Distance
  double dist = d(6371, lat, lat2, lng, lng2);
  Serial.print("Distance: ");
  Serial.println(dist, 6);

  // Print Free Heap Memory
  uint32_t freeHeapBytes = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
  uint32_t totalHeapBytes = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
  float percentageHeapFree = freeHeapBytes * 100.0f / (float)totalHeapBytes;

  // Print to serial
  Serial.printf("[Memory] %.1f%% free - %d of %d bytes free\n", percentageHeapFree, freeHeapBytes, totalHeapBytes);
  Serial.println();

  // Buzzer Conditions
  if (dist > 10 && dist < 20){
    tone(buzzer, 440);
    delay(1000);
  }

  else if (dist < 10){
    tone(buzzer, 770);
    delay(1000);
  }

  else{
    noTone(buzzer);
  }
}

void setup() {
  // Initialize buzzer as output
  pinMode(buzzer, OUTPUT);

  // Initialize hardware serial port for GPS module
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  delay(1000);
  
  // Initialize serial communication for debugging
  Serial.begin(115200);
  while (!Serial);

  // Setup WiFi and MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

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
      // Update latitude data
      lat2 = gps.location.lat();
      
      // Update longitude data
      lng2 = gps.location.lng();
    }
  }
}

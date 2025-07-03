#define BLYNK_TEMPLATE_ID        "TMPL29BFQFJsu"
#define BLYNK_TEMPLATE_NAME      "Smart Plant Care System"
#define BLYNK_AUTH_TOKEN         "WCEJh_9dybdb2CnCO8xrNUSAalk2u6s1"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "sameh";
char pass[] = "00000000";

#define DHTPIN     18
#define DHTTYPE    DHT11
#define SOIL_PIN   35

DHT dht(DHTPIN, DHTTYPE);

// Virtual pin for “Add Water” label (V0)
#define VPIN_ADD_WATER V0

void setup() {
  Serial.begin(115200);
  // Begin Blynk (blocks until connected)
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  while (Blynk.connected() == false) {
    delay(100);
  }
  Serial.println("Connected to WiFi and Blynk");

  // Initialize DHT11
  dht.begin();
  delay(2000);
  Serial.println("Sensor Test Starting...");
}

void loop() {
  Blynk.run();

  // 1) Read DHT11
  float humidity    = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT11 read error!");
    // Skip virtualWrite for V2/V3 if error
  } else {
    Serial.printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
    Blynk.virtualWrite(V2, temperature);
    Blynk.virtualWrite(V3, humidity);
  }

  // 2) Read soil moisture
  int soilRaw = analogRead(SOIL_PIN);
  Serial.printf("Soil raw value: %d\n", soilRaw);

  // Map raw to percentage: dry = 4095 → 0%, wet = 200 → 100%
  int dry  = 4095;
  int wet  =  200;
  int soilPercent = map(soilRaw, dry, wet, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  Serial.printf("Soil Moisture: %d%%\n", soilPercent);
  Blynk.virtualWrite(V1, soilPercent);

  // 3) Check threshold and send “Add Water” notification on V0
  const int thresholdPercent = 30;
  static bool notified = false;

  if (soilPercent >= thresholdPercent) {
    // Soil is fine again → reset notification flag & clear label
    notified = false;
    Blynk.virtualWrite(VPIN_ADD_WATER, "WATER ZAI EL FOL");
  }

  if (soilPercent < thresholdPercent && !notified) {
    // First time below threshold → set label without logging event
    notified = true;
    Blynk.virtualWrite(VPIN_ADD_WATER, "🚨 Add Water to the Plant!");
    
  

  }
 
 

  
  

  Serial.println("-----------------------------");
  delay(3000);
}


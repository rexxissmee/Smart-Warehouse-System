// Define Blynk template information
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Warehouse System"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN"

// Include necessary libraries
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_PCF8574.h>
#include <Adafruit_TCS34725.h>

// WiFi credentials
char ssid[] = "WIFI_SSID";
char pass[] = "WIFI_PASSWORD";

// Hardware connections
#define WATER_LEVEL_PIN A0
#define BUZZER_PIN D3
#define DHT_PIN D4
#define LED_PIN D5

// Define Blynk virtual pins for data stream
WidgetLED LEDCONNECT(V0);
#define TEMP_VPIN V1
#define HUMID_VPIN V2
#define LIGHT_VPIN V3
#define WATER_VPIN V4
#define RELAY1_VPIN V5
#define RELAY2_VPIN V6
#define RELAY3_VPIN V7
#define STEPPER_VPIN V8
#define BUZZER_VPIN V9

// Initialize sensor objects
BH1750 lightMeter;
DHT dht(DHT_PIN, DHT11);
Adafruit_PCF8574 pcf8574;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_300MS, TCS34725_GAIN_1X);

// Declare global variables
unsigned long lastUpdate = 0;
bool buzzerActive = false;
int stepperState = 0;
const int stepperPins[4] = { 4, 5, 6, 7 };  //Stepper connection pins on PCF8574
uint8_t fullStep[4][4] = {                  //Full-stepping mode
  { 1, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 1, 1 },
  { 1, 0, 0, 1 }
};

int targetColors[280][3] = {                //Data for sorting agricultural products

};

// Function to check sensor initialization
void checkSensors() {
  Serial.println("\nChecking sensors...");
  double temp = dht.readTemperature();
  double humid = dht.readHumidity();
  if (isnan(temp) || isnan(humid)) {
    Serial.println("[ERROR] Failed to initialize DHT11!");
  } else {
    Serial.println("[OK] DHT11 sensor initialized successfully.");
  }

  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23)) {
    Serial.println("[ERROR] Failed to initialize BH1750!");
  } else {
    Serial.println("[OK] BH1750 sensor initialized successfully.");
  }

  double waterLevel = (analogRead(WATER_LEVEL_PIN) / 1024.0) * 100.0;
  if (waterLevel < 0 || waterLevel > 100) {
    Serial.println("[ERROR] Water sensor reading invalid!");
  } else {
    Serial.println("[OK] Water sensor initialized successfully.");
  }

  if (!tcs.begin()) {
    Serial.println("[ERROR] Failed to initialize TCS3472!");
  } else {
    Serial.println("[OK] TCS3472 sensor initialized successfully.");
  }

  if (!pcf8574.begin(0x20)) {
    Serial.println("[ERROR] Failed to initialize PCF8574!");
  } else {
    Serial.println("[OK] PCF8574 initialized successfully.");
  }
}

// Establish WiFi and Blynk connection
void checkConnection() {
  Serial.print("Connecting");
  WiFi.begin(ssid, pass);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) { 
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[OK] WiFi connected!");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    if (Blynk.connected()) {
      Serial.println("[OK] Blynk connected! Start syncing data...");
    } else {
      Serial.println("[ERROR] Failed to connect to Blynk! Check auth token and retry.");
    }
  } else {
    Serial.println("\n[ERROR] Failed to connect to WiFi! Press RESET button to retry.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Wire.begin();
  pcf8574.begin(0x20);
  for (int i = 0; i < 4; i++) {
    pcf8574.pinMode(stepperPins[i], OUTPUT);
  }
  dht.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);
  tcs.begin(0x29);

  checkSensors();
  checkConnection();
}

//Sync the state with Blynk
BLYNK_CONNECTED() { Blynk.syncAll(); }

//Manual control via Blynk
BLYNK_WRITE(RELAY1_VPIN) { pcf8574.digitalWrite(1, !param.asInt()); }
BLYNK_WRITE(RELAY2_VPIN) { pcf8574.digitalWrite(2, !param.asInt()); }
BLYNK_WRITE(RELAY3_VPIN) { pcf8574.digitalWrite(3, !param.asInt()); }

BLYNK_WRITE(BUZZER_VPIN) {
  int buzzerState = param.asInt();
  buzzerActive = (buzzerState == 1);
  digitalWrite(BUZZER_PIN, buzzerActive ? 1 : 0);
}

// Function to control stepper motor
void stepperMotor(int step) {
  for (int i = 0; i < 4; i++) {
    pcf8574.digitalWrite(stepperPins[i], fullStep[step][i]);
  }
}

BLYNK_WRITE(STEPPER_VPIN) {
  int direction = param.asInt();
  int totalSteps = 3 * 2048;
  
  if (direction != stepperState) {
    for (int i = 0; i < totalSteps; i++) {
      if (direction == 1) {
        stepperMotor(i % 4);
      } else if (direction == 0) {
        stepperMotor(3 - (i % 4));
      }
      delay(2);
      yield();
    }
  }
  stepperState = direction;
}

void checkAlert(double temp, double humid, double waterLevel) {
  if ((temp > 30 || humid > 80 || waterLevel < 20) && !buzzerActive) {
    buzzerActive = true;
    digitalWrite(BUZZER_PIN, 1);
    Blynk.virtualWrite(BUZZER_VPIN, 1);
    delay(5000);
    buzzerActive = false;
    digitalWrite(BUZZER_PIN, 0);
    Blynk.virtualWrite(BUZZER_VPIN, 0);
  }
}

// Function to send data to Blynk
void sendSensorData() {
  double temp = dht.readTemperature();
  double humid = dht.readHumidity();
  double lightIntensity = lightMeter.readLightLevel();
  double waterLevel = (analogRead(WATER_LEVEL_PIN) / 1024.0) * 100.0;

  Blynk.virtualWrite(TEMP_VPIN, temp);
  Blynk.virtualWrite(HUMID_VPIN, humid);
  Blynk.virtualWrite(LIGHT_VPIN, lightIntensity);
  Blynk.virtualWrite(WATER_VPIN, waterLevel);

  // Check alert
  checkAlert(temp, humid, waterLevel);
}

// Led widget indicate connection status
void blinkLED() {
  if (LEDCONNECT.getValue()) {
    LEDCONNECT.off();
  } else {
    LEDCONNECT.on();
  }
}

// Check if the detected color matches a target color
bool isTargetColor(int R, int G, int B) {
  for (int i = 0; i < sizeof(targetColors) / sizeof(targetColors[0]); i++) {
    if (targetColors[i][0] == R && targetColors[i][1] == G && targetColors[i][2] == B) {
      return true;
    }
  }
  return false;
}

// Recognite color, standardizing the original color code (16-bit) to RGB color(8-bit) and compare with the data provided
void checkColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  uint8_t R = (r * 255) / c;
  uint8_t G = (g * 255) / c;
  uint8_t B = (b * 255) / c;
  
  Serial.print("R: "); Serial.print(R);
  Serial.print(" G: "); Serial.print(G);
  Serial.print(" B: "); Serial.print(B);
  
  if (isTargetColor(R, G, B)) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println(" Meet the standards");
  } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println(" Does not meet the standards");
  }
  
  delay(1000);
}

void loop() {
  Blynk.run();

  if (millis() - lastUpdate > 5000) { // Update data every 5 seconds
    sendSensorData();
    blinkLED();
    lastUpdate = millis();
  }

  checkColor();
}

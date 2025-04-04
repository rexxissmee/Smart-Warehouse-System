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
  {59, 197, 0}, {60, 197, 0}, {61, 197, 0}, {62, 197, 0}, {63, 198, 0}, 
  {64, 198, 0}, {65, 198, 0}, {66, 199, 0}, {67, 199, 0}, {68, 199, 0}, 
  {69, 199, 0}, {70, 200, 0}, {71, 200, 0}, {72, 200, 0}, {73, 201, 0}, 
  {74, 201, 0}, {75, 201, 0}, {76, 202, 0}, {77, 202, 0}, {78, 202, 0},
  {79, 202, 0}, {80, 203, 0}, {81, 203, 0}, {82, 204, 0}, {83, 204, 0}, 
  {84, 204, 0}, {85, 204, 0}, {86, 205, 0}, {87, 205, 0}, {88, 205, 0}, 
  {89, 205, 0}, {90, 206, 0}, {91, 206, 0}, {92, 206, 0}, {93, 207, 0}, 
  {94, 207, 0}, {95, 207, 0}, {96, 207, 0}, {97, 208, 0}, {98, 208, 0}, 
  {99, 209, 0}, {100, 209, 0}, {101, 209, 0}, {102, 209, 0}, {103, 210, 0}, 
  {104, 210, 0}, {105, 210, 0}, {106, 210, 0}, {107, 211, 0}, {108, 211, 0}, 
  {109, 212, 0}, {110, 212, 0}, {111, 212, 0}, {112, 212, 0}, {113, 213, 0}, 
  {114, 213, 0}, {115, 213, 0}, {116, 214, 0}, {117, 214, 0}, {118, 214, 0}, 
  {119, 214, 0}, {120, 215, 0}, {121, 215, 0}, {122, 215, 0}, {123, 215, 0}, 
  {124, 216, 0}, {125, 216, 0}, {126, 217, 0}, {127, 217, 0}, {128, 217, 0}, 
  {129, 217, 0}, {130, 218, 0}, {131, 218, 0}, {132, 218, 0}, {133, 219, 0}, 
  {134, 219, 0}, {135, 219, 0}, {136, 220, 0}, {137, 220, 0}, {138, 220, 0}, 
  {139, 220, 0}, {140, 221, 0}, {141, 221, 0}, {142, 221, 0}, {143, 222, 0}, 
  {144, 222, 0}, {145, 222, 0}, {146, 222, 0}, {147, 223, 0}, {148, 223, 0}, 
  {149, 223, 0}, {150, 223, 0}, {151, 224, 0}, {152, 224, 0}, {153, 225, 0}, 
  {154, 225, 0}, {155, 225, 0}, {156, 225, 0}, {157, 226, 0}, {158, 226, 0}, 
  {159, 226, 0}, {160, 227, 0}, {161, 227, 0}, {162, 227, 0}, {163, 228, 0}, 
  {164, 228, 0}, {165, 228, 0}, {166, 228, 0}, {167, 229, 0}, {168, 229, 0}, 
  {169, 229, 0}, {170, 230, 0}, {171, 230, 0}, {172, 230, 0}, {173, 230, 0}, 
  {174, 231, 0}, {175, 231, 0}, {176, 232, 0}, {177, 232, 0}, {178, 232, 0}, 
  {179, 232, 0}, {180, 233, 0}, {181, 233, 0}, {182, 233, 0}, {183, 233, 0}, 
  {184, 234, 0}, {185, 234, 0}, {186, 234, 0}, {187, 235, 0}, {188, 235, 0}, 
  {189, 235, 0}, {190, 236, 0}, {191, 236, 0}, {192, 236, 0}, {193, 237, 0}, 
  {194, 237, 0}, {195, 237, 0}, {196, 237, 0}, {197, 238, 0}, {198, 238, 0}, 
  {199, 238, 0}, {200, 238, 0}, {201, 239, 0}, {202, 239, 0}, {203, 240, 0}, 
  {204, 240, 0}, {205, 240, 0}, {206, 240, 0}, {207, 241, 0}, {208, 241, 0}, 
  {209, 241, 0}, {210, 241, 0}, {211, 242, 0}, {212, 242, 0}, {213, 242, 0}, 
  {214, 243, 0}, {215, 243, 0}, {216, 243, 0}, {217, 244, 0}, {218, 244, 0}, 
  {219, 244, 0}, {220, 245, 0}, {221, 245, 0}, {222, 245, 0}, {223, 245, 0}, 
  {224, 246, 0}, {225, 246, 0}, {226, 246, 0}, {227, 246, 0}, {228, 247, 0}, 
  {229, 247, 0}, {230, 248, 0}, {231, 248, 0}, {232, 248, 0}, {233, 248, 0}, 
  {234, 249, 0}, {235, 249, 0}, {236, 249, 0}, {237, 250, 0}, {238, 250, 0}, 
  {239, 250, 0}, {240, 251, 0}, {241, 251, 0}, {242, 251, 0}, {243, 251, 0}, 
  {244, 252, 0}, {245, 252, 0}, {246, 252, 0}, {247, 253, 0}, {248, 253, 0}, 
  {249, 253, 0}, {250, 253, 0}, {251, 254, 0}, {252, 254, 0}, {253, 255, 0}, 
  {253, 254, 0}, {254, 254, 0}, {255, 254, 0}, {254, 253, 0}, {253, 252, 0}, 
  {252, 251, 0}, {252, 250, 0}, {251, 249, 0}, {251, 249, 1}, {250, 248, 1}, 
  {250, 247, 1}, {249, 246, 1}, {249, 245, 1}, {248, 244, 1}, {248, 243, 1}, 
  {247, 242, 1}, {246, 241, 1}, {246, 240, 1}, {245, 239, 1}, {245, 239, 2}, 
  {245, 238, 2}, {244, 237, 2}, {243, 237, 2}, {243, 236, 2}, {242, 235, 2}, 
  {242, 234, 2}, {241, 233, 2}, {241, 232, 2}, {240, 231, 2}, {239, 230, 2}, 
  {239, 229, 2}, {239, 229, 3}, {238, 228, 3}, {238, 227, 3}, {237, 226, 3}, 
  {236, 225, 3}, {236, 224, 3}, {235, 223, 3}, {235, 222, 3}, {234, 222, 3}, 
  {234, 221, 3}, {233, 220, 3}, {233, 220, 4}, {232, 219, 4}, {232, 218, 4}, 
  {231, 217, 4}, {231, 216, 4}, {230, 215, 4}, {229, 214, 4}, {229, 213, 4}, 
  {228, 212, 4}, {228, 211, 4}, {227, 210, 4}, {227, 209, 4}, {227, 209, 5}, 
  {226, 208, 5}, {225, 207, 5}, {224, 206, 5}, {224, 205, 5}, {223, 204, 5}, 
  {222, 203, 5}, {222, 202, 5}, {221, 201, 5}, {221, 200, 5}, {221, 200, 6}, 
  {220, 199, 6}, {220, 198, 6}, {219, 197, 6}, {218, 196, 6}, {218, 195, 6}, 
  {217, 194, 6}, {217, 193, 6}, {216, 192, 6}, {215, 192, 6}, {215, 191, 6}, 
  {214, 190, 6}, {214, 190, 7}, {214, 189, 7}, {213, 188, 7}, {212, 187, 7}, 
  {212, 186, 7}, {211, 185, 7}, {211, 184, 7}, {210, 183, 7}, {210, 182, 7}
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

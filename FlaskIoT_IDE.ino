#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Replace with your network credentials
const char* ssid = "MAJU";
const char* password = "tercapai";

// Your server URL
const char* serverUrl = "http://192.168.1.6:5000/data"; // Change to your server's local IP address

#define DEBUG_ESP_HTTP_CLIENT
#define DHTPIN 4  // GPIO pin where the DHT22 is connected
#define ldr 36
#define pir 16
#define Relay_Lamp 2
#define Relay_AC 17

#define DHTTYPE DHT11
LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT dht(DHTPIN, DHTTYPE);

const float GAMMA = 0.7;
const float RL10 = 50;
int pengunjung = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(pir, INPUT);
  pinMode(ldr, INPUT);
  pinMode(Relay_Lamp,OUTPUT);
  pinMode(Relay_AC, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  // Read temperature and humidity from DHT22
  float temp_data = dht.readTemperature();
  float hum_data = dht.readHumidity();

  int data_ldr = analogRead(ldr);
  int data_pir = digitalRead(pir);
  float voltage = data_ldr / 4095.0 * 5.0;
  float resistance = 2000 * voltage / (1 - voltage / 5.0);
  float intensitas = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));


  // Check if any reads failed
  if (isnan(temp_data) || isnan(hum_data)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  lcd.setCursor(0,0);
  lcd.print("Temp = ");
  lcd.print(temp_data);
  lcd.setCursor(0,1);
  lcd.print("Hum = ");
  lcd.print(hum_data);

  // Print values to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temp_data);
  Serial.print(" °C, Humidity: ");
  Serial.print(hum_data);
  Serial.println(" %");
  Serial.print("Intensitas Cahaya : ");
  Serial.print(intensitas);
  Serial.println(" lux");

  if(temp_data >= 40){
      digitalWrite(Relay_AC, HIGH);
      Serial.println("AC Menyala");
  }
  else{
    digitalWrite(Relay_AC,LOW);
    Serial.println("AC Mati");
  }

  if(intensitas > 400){
    digitalWrite(Relay_Lamp, LOW);
    Serial.println("Lampu Mati");
  }
  else{
    digitalWrite(Relay_Lamp, HIGH);
    Serial.println("Lampu Menyala");
  }

  if(data_pir == 1){
    Serial.println("Terdapat Pengunjung");
    pengunjung++;
    Serial.print("Jumlah Pengunjung :");
    Serial.println(pengunjung);
  }
  else{
    Serial.print("Jumlah Pengunjung :");
    Serial.println(pengunjung);
  }

  // Send data to server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temperature\":" + String(temp_data) + ",\"humidity\":" + String(hum_data) + ",\"Lux\":" + String(intensitas) + ",\"pengunjung\":" + String(pengunjung) + "}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  Serial.println("----------------------------------------------");
  // Send a request every 10 seconds
  delay(10000);
}

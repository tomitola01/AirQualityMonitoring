/*
  Code for air quality monitoring system.
  Code heavily adapted using guides from https://randomnerdtutorials.com/projects-esp32/
*/

//libraries included
#include <Wire.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <WiFi.h>
#include "time.h" 
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "ESPAsyncWebServer.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Network credentials
const char* ssid = //enter network SSID;
const char* password = //password for SSID;

//Used to display correct time depending on timezone. Default set to BST.
//Change gmtOffset_sec to desired timezone difference in seconds
//Change daylightOffset_sec if necessary
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

String openWeatherMapApiKey = //write free OpenWeatherMap API key here;

String city = //place town/city here
String countryCode = //place country code here

String jsonBuffer;

//define BME connections
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 15
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

//define OLED screen connections
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET     17 // Reset pin 
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float temperature;
float humidity;
float pressure;
float gasResistance;

//maximum values for warnings
float maxTemp = 28.0;
float maxHumidity = 70.0;
float maxVOC = 10.0;

bool pressed = false;

AsyncWebServer server(80);
AsyncEventSource events("/events");

//update website, oled and time every second
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;  // send readings timer

const int button = 16;
int state = 0;

//button interrupt method
void IRAM_ATTR isr() {
  if(state == 0){
    state++;
  }else{
    state = 0;
  }
  pressed = true;
  delay(20);
}

//read bme680 values
void read680Values(){
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Could not begin reading :("));
    return;
  }
  if (!bme.endReading()) {
    Serial.println(F("Could not complete reading :("));
    return;
  }
  temperature = bme.temperature;
  pressure = bme.pressure / 100.0;
  humidity = bme.humidity;
  gasResistance = bme.gas_resistance / 1000.0;
}


void setup() {
  Serial.begin(115200);

  //initialise button interrupt
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(button, isr, RISING);

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi Access Point");
  }
  Serial.print("ESP32 Web Server: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //initialise bme680 sensor
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  //initialise bme680 sensor
  if (!bme.begin()) {
    Serial.println(F("BME680 sensor not connected"));
    while (1);
  }
  //oversampling and filter initialisation
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  if(!SPIFFS.begin()){
    Serial.println("Could not mount SPIFFS");
    return;
  }  

  //Web server handling
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  //Graphs
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temperature).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(humidity).c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(pressure).c_str());
  });

  server.on("/resistance", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(gasResistance).c_str());
  });

  server.begin();

  //initialise time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printTime();
}

void loop() {
    display.setCursor(0,0);
    display.clearDisplay();
    //Sensor warnings
    if(temperature >= maxTemp){
          state = 2;
          pressed = false;
          display.println("Warning: Temperature too high");
          display.display();
        }else if(humidity >= maxHumidity){
          state = 2;
          pressed = false;
          display.println("Warning: Humidity too high");
          display.display();
        }else if((millis()>=10000)&&gasResistance <= maxVOC){
          state = 2;
          pressed = false;
          display.println("Warning: Air quality too low");
          display.display();
    }

    //update website, weather and OLED every second or change OLED display when the button is pressed
    if (((millis() - lastTime) > timerDelay)||pressed == true) {
      read680Values();
      Serial.printf("%.2f, ", temperature);
      Serial.printf("%.2f, ", humidity);
      Serial.printf("%.2f, ", pressure);
      Serial.printf("%.2f ", gasResistance);
      //state 0 displays Time, date, weather and day on OLED
      if(state == 0){
        printTime();
        if(WiFi.status()== WL_CONNECTED){
          String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
          
          jsonBuffer = httpGETRequest(serverPath.c_str());
          Serial.println(jsonBuffer);
          JSONVar myObject = JSON.parse(jsonBuffer);
      
          if (JSON.typeof(myObject) == "undefined") {
            Serial.println("Parsing input failed!");
            return;
          }
          display.println("Weather Forecast: ");
          display.println(myObject["weather"][0]["description"]);
        }
        display.display();
      //state 1 displays BME680 readings on OLED
      }else if(state == 1){
          display.printf("Temperature: %.2f *C\n", temperature);
          display.printf("Humidity: %.2f %%\n", humidity);
          display.printf("Pressure: %.2f hPa\n", pressure);
          display.printf("VOC: %.2f KOhm \n", gasResistance);
          display.display();
      }
      pressed = false;
      Serial.println();
      display.display();
      lastTime = millis();
    }
}

//print time to OLED
void printTime(){
  struct tm timeinfo;
  display.setCursor(0,0);
  display.clearDisplay();
  if(!getLocalTime(&timeinfo)){
    display.println("Failed to obtain time");
    return;
  }
  display.println(&timeinfo, "%H:%M:%S");
  display.println(&timeinfo, "%A, %B %d %Y");
}

//Get data from HTTP to update weaather
String httpGETRequest(const char* serverName) {
  HTTPClient http;
  
  http.begin(serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}

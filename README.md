# Air Quality Monitoring System
This repository includes the Arduino C code (Monitoring_ArduinoC.ino) for the air quality monitoring system. To be used with the ESP32 microcontroller, a BME680 sensor, a push button for input, and an Adafruit 128x32 OLED. <br />
Also included is the HTML file (Monitoring_webserver.html) to build the web server with the ESP32. <br />
To do build the server: 
1. Place Monitoring_ArduinoC.ino in an Arduino sketch folder
2. Create a new folder in the created Arduino sketch folder and name it "data"
3. Place Monitoring_webserver.html in the "data" folder
4. Upload the sketch to the ESP32 and run it
5. Open the serial monitor and use obtain the ESP32s IP address
6. Enter IP address into web browser 

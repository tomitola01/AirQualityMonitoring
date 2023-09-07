# Air Quality Monitoring System
This repository includes the Arduino C code (monitoring_arduinoC.ino) for the air quality monitoring system. To be used with the ESP32 microcontroller, a BME680 sensor, a push button for input, and an Adafruit 128x32 OLED. <br />
\
![](./CAD/aqm_housing.png)
![the breadboard prototype working on an Adafruit 128x32 shows an example of the system operating normally](./Monitoring_Operation.jpg)
The schematics and PCB design of the system can be found in the circuits_file folder.
The 3D design for the housing of the AQM system can be found in the CAD folder. Fusion 360 is required to open the .fz3 file.
\
Also included is the HTML file (monitoring_webserver.html) to build the web server with the ESP32. <br />
To build the server: 
1. Download the Monitoring_Code folder
2. Open the project using Ardunio
3. Upload the sketch to the ESP32 and run it
4. Open the serial monitor and use obtain the ESP32's IP address
5. Enter IP address into web browser 

# Air Quality Monitoring System
This repository includes the Arduino C code (monitoring_arduinoC.ino) for the air quality monitoring system. To be used with the ESP32 microcontroller, a BME680 sensor, a push button for input, and an Adafruit 128x32 OLED. <br />
\
A JPG (Monitoring_Operation.jpg) of the breadboard prototype working on an Adafruit 128x32 shows an example of the system operating normally. <br />
The monitor_pcb.pdf includes a layout of Printed Circuit Board that would be used in the final product. <br />
Monitor_Fusion360.png is a model of the final product designed in Fusion 360 by another team member, that I assisted in designing. <br />
\
Also included is the HTML file (monitoring_webserver.html) to build the web server with the ESP32. <br />
To build the server: 
1. Download the Monitoring_Code folder
2. Open the project using Ardunio
3. Upload the sketch to the ESP32 and run it
4. Open the serial monitor and use obtain the ESP32's IP address
5. Enter IP address into web browser 

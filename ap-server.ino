/*
  Skyrc IMAX B6 - uart to serial port over esp8266 server
  Using for esp8266
  Remark: If you got bad uart signal :
  remove resistor R4 https://github.com/neolead/skyrc-imax-b6-usb-uart-over-wifi-with-esp8266/raw/main/uart.jpg
  thanks to : https://github.com/stawel/cheali-charger/issues/256
  Created by neolead: janvarry@gmail.com nov.2020
*/

#include <ESP8266WiFi.h>


//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
#define TCP_PORT (23)           // Choose any port you want
WiFiServer tcpServer(TCP_PORT);
WiFiClient tcpServerClients[MAX_SRV_CLIENTS];
IPAddress apIP(192, 168, 4, 1);
const char SSID[] = "TCPUARTBridge";  // Choose any SSID
const char PASSWORD[] = "12345678"; // minimum 8 characters.

#define SerialDebug Serial1   // Debug goes out on GPIO02
#define SerialGPS   Serial    // GPS or other device connected to the ESP UART

#ifndef min
#define min(x,y)  ((x)<(y)?(x):(y))
#endif

void setup() {
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  // !!! Debug output goes to GPIO02 !!!
  SerialDebug.begin(9600);
  SerialDebug.println("TCP <-> UART bridge");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID, PASSWORD);
  SerialGPS.begin(9600);

  // Start TCP listener on port TCP_PORT
  tcpServer.begin();
  tcpServer.setNoDelay(true);
  SerialDebug.print("Ready! Use 'telnet or nc ");
  SerialDebug.print(WiFi.localIP());
  SerialDebug.print(' ');
  SerialDebug.print(TCP_PORT);
  SerialDebug.println("' to connect");
}

void loop() {
  uint8_t i;
  char buf[1024];
  int bytesAvail, bytesIn;

  //check if there are any new clients
  if (tcpServer.hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //find free/disconnected spot
      if (!tcpServerClients[i] || !tcpServerClients[i].connected()) {
        if (tcpServerClients[i]) tcpServerClients[i].stop();
        tcpServerClients[i] = tcpServer.available();
        SerialDebug.print("New client: "); SerialDebug.print(i);
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient tcpServerClient = tcpServer.available();
    tcpServerClient.stop();
  }

  //check clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (tcpServerClients[i] && tcpServerClients[i].connected()) {
      //get data from the telnet client and push it to the UART
      while ((bytesAvail = tcpServerClients[i].available()) > 0) {
        bytesIn = tcpServerClients[i].readBytes(buf, min(sizeof(buf), bytesAvail));
        if (bytesIn > 0) {
          SerialGPS.write(buf, bytesIn);
          delay(0);
        }
      }
    }
  }

  //check UART for data
  while ((bytesAvail = SerialGPS.available()) > 0) {
    bytesIn = SerialGPS.readBytes(buf, min(sizeof(buf), bytesAvail));
    if (bytesIn > 0) {
      //push UART data to all connected telnet clients
      for (i = 0; i < MAX_SRV_CLIENTS; i++) {
        if (tcpServerClients[i] && tcpServerClients[i].connected()) {
          tcpServerClients[i].write((uint8_t*)buf, bytesIn);
          delay(0);
        }
      }
    }
  }
}

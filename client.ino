/*
  Skyrc IMAX B6 - uart to serial port over esp8266 client
  I used this scetch with nodemcu, because it got uart to serial port converter. 
  Possible using with esp8266 + any ftdi convertor.
  Remark: If you got bad uart signal :
  remove resistor R4 https://github.com/neolead/skyrc-imax-b6-usb-uart-over-wifi-with-esp8266/raw/main/uart.jpg
  thanks to : https://github.com/stawel/cheali-charger/issues/256
  Created by neolead: janvarry@gmail.com nov.2020
  main part maid by from by Tom Igoe 
 */

#include <ESP8266WiFi.h>
#define MY_BLUE_LED_PIN 2


const char* ssid     = "TCPUARTBridge";
const char* password = "12345678";

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 4, 1);
int port = 23;

// Initialize the Wifi client library
WiFiClient client;

void setup() {
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  Serial.begin(9600);
  pinMode(MY_BLUE_LED_PIN, OUTPUT); 
  delay(10);


  // We start by connecting to a WiFi network

  //Serial.println();
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(MY_BLUE_LED_PIN, LOW);
    delay(500);
    digitalWrite(MY_BLUE_LED_PIN, HIGH);
    //Serial.print(".");
  }

  //Serial.println("");
  //Serial.println("WiFi connected");  
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    //Serial.println("connected to server");
    digitalWrite(MY_BLUE_LED_PIN, HIGH);
    delay(50);
      }
  else {
    // if you didn't get a connection to the server:
//    Serial.println("connection to server failed");
    digitalWrite(MY_BLUE_LED_PIN, LOW);
    delay(200);
    digitalWrite(MY_BLUE_LED_PIN, HIGH);
    delay(200);
    digitalWrite(MY_BLUE_LED_PIN, LOW);
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    digitalWrite(MY_BLUE_LED_PIN, LOW);
    delay(10);
    digitalWrite(MY_BLUE_LED_PIN, HIGH);
    delay(10);
  }

  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    digitalWrite(MY_BLUE_LED_PIN, LOW);
    delay(10);
    digitalWrite(MY_BLUE_LED_PIN, HIGH);
    delay(10);
    if (client.connected()) {
      client.print(inChar);
    } else client.connect(server, port);
    delay(10);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println("x");
    digitalWrite(MY_BLUE_LED_PIN, LOW);
    delay(10);
    digitalWrite(MY_BLUE_LED_PIN, HIGH);
    delay(1000);
    digitalWrite(MY_BLUE_LED_PIN, LOW);
    delay(10);
    client.stop();
    delay(1000);
  }
}

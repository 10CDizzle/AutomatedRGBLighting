/*
   WebSocketServer.ino

    Created on: 22.05.2015

*/

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsServer.h>

WiFiMulti WiFiMulti;
WebSocketsServer webSocket = WebSocketsServer(81);

#define USE_SERIAL Serial

// setting PWM properties
const int freq = 5000;
const int resolution = 16;
const int ledChannels[3] = {0, 1, 2};
const int ledPins[3] = {18, 19, 21};

char StrBuf[50];

//storage variable for LED Values
float LEDSet[3] = {0, 0, 0};


//Handler for getting three-value commands from the client
void ParseNums(float (& val)[3], String input)
{
  int FirstSpace = input.indexOf(' ');
  int SecondSpace = input.indexOf(' ', FirstSpace+1);
  int ThirdSpace = input.indexOf(' ', SecondSpace+1);
  String FirstNumber = input.substring(0, FirstSpace);
  String SecondNumber = input.substring(FirstSpace, SecondSpace);
  String ThirdNumber = input.substring(SecondSpace, ThirdSpace);
  val[0] = FirstNumber.toFloat();
  val[1] = SecondNumber.toFloat();
  val[2] = ThirdNumber.toFloat();
  if (val[0] > 65536 || val[0] < 0)
  {
    val[0] = 0;
  }
  else if (val[1] > 65536 || val[1] < 0)
  {
    val[1] = 0;
  }
  if (val[2] > 65536 || val[2] < 0)
  {
    val[2] = 0;
  }
}


void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++) {
    if (i % cols == 0) {
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

      sprintf(StrBuf, "%s", payload);
      //USE_SERIAL.printf(StrBuf);

      //Parse Numbers here...
      ParseNums(LEDSet, StrBuf);
      //USE_SERIAL.printf("%.2f %.2f %.2f",LEDSet[0],LEDSet[1],LEDSet[2]);

      //Set PWM values:

      ledcWrite(ledChannels[0], LEDSet[0]);
      ledcWrite(ledChannels[1], LEDSet[1]);
      ledcWrite(ledChannels[2], LEDSet[2]);

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
      hexdump(payload, length);

      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }

}

void setup() {
  // USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);

  // configure LED PWM functionalitites
  for (int i = 0; i <= 2; i++)
  {
    ledcSetup(ledChannels[i], freq, resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(ledPins[i], ledChannels[i]);
  }

  //Serial.setDebugOutput(true);
  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 10; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP("SuperSecret", "EvenMoreSecret");

  //If it can't connect, just reset the system
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    USE_SERIAL.printf("Resetting...");
    ESP.restart();
  }
  USE_SERIAL.printf("Connected!");
  USE_SERIAL.printf(WiFi.localIP().toString().c_str());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


}

void loop() {
  webSocket.loop();
}

// API
// /id returns id and name in json
//
// /ledControl?line=0&position=0&color=00FF00&blink=0
// line = 0-3, position = 0-19, color=hex color, blink = 0 fixed, 2 sec, 1 sec, .25 second
//
// /test starts test procedure

#include <SPI.h>
#include <Ethernet2.h>
#include <aREST.h>
#include "FastLED.h"
//#include <avr/wdt.h>

#define NUM_LEDS 20
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x41 };

// IP address in case DHCP fails
//IPAddress ip(192,168,10,2);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Function to be exposed
  rest.function("led", ledControl);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("123456");
  rest.set_name("Estante_A1");

  // Start the Ethernet connection and the server
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    //    Ethernet.begin(mac, ip);
  }
  server.begin();
  Serial.print("Controller is at:");
  Serial.println(Ethernet.localIP());

  // Start watchdog
  //  wdt_enable(WDTO_4S);
}

void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  //  wdt_reset();

}

void updateLED(int l, int p, String c, int b) {
  
}

// Custom function accessible by the API
// Modified to parse multiple parameters
int ledControl(String command) {

  String params = command;
  int firstAmpersandIndex = params.indexOf('&');
  String lineString = params.substring(0, firstAmpersandIndex);
  int line = lineString.toInt();
  Serial.println(line);
  int firstEqualsIndex = params.indexOf('=', firstAmpersandIndex);
  int secondAmpersandIndex = params.indexOf('&', firstEqualsIndex);
  String positionString = params.substring(firstEqualsIndex + 1, secondAmpersandIndex);
  int pos = positionString.toInt();
  Serial.println(pos);
  int secondEqualsIndex = params.indexOf('=', secondAmpersandIndex);
  int thirdAmpersandIndex = params.indexOf('&', secondEqualsIndex);
  String colorString = params.substring(secondEqualsIndex + 1, thirdAmpersandIndex);
  Serial.println(colorString);
  int fourthEqualsIndex = params.indexOf('=', thirdAmpersandIndex);
  String blinkString = params.substring(fourthEqualsIndex + 1);
  int blink = blinkString.toInt();
  Serial.println(blink);
  return 1;
}

// command would be "0&position=0&color=00FF00&blink=0"


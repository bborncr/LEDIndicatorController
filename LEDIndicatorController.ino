// API
// /id returns id and name in json
//
// /led?line=0&position=0&color=00FF00&blink=0
// line = 0-3, position = 0-19, color=hex color, blink = 0 fixed, 1 = 2 sec, 2 = 1 sec, 3 = .25 second
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

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
    FastLED.show();
  }
  delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
    FastLED.show();
  }
  delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }

  // Start watchdog
  //  wdt_enable(WDTO_4S);
}

void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  //  wdt_reset();

}

uint64_t StrToHex(const char* str)
{
  return (uint64_t) strtoull(str, 0, 16);
}

void updateLED(int l, int p, String c, int b) {
  int hexcolor = StrToHex(c.c_str());
  leds[p] = hexcolor;
  FastLED.show();

}

// Custom function accessible by the API
// Modified to parse multiple parameters
int ledControl(String command) {

  String params = command;
  int firstAmpersandIndex = params.indexOf('&');
  String lineString = params.substring(0, firstAmpersandIndex);
  int line = lineString.toInt();
  //  Serial.println(line);
  int firstEqualsIndex = params.indexOf('=', firstAmpersandIndex);
  int secondAmpersandIndex = params.indexOf('&', firstEqualsIndex);
  String positionString = params.substring(firstEqualsIndex + 1, secondAmpersandIndex);
  int pos = positionString.toInt();
  //  Serial.println(pos);
  int secondEqualsIndex = params.indexOf('=', secondAmpersandIndex);
  int thirdAmpersandIndex = params.indexOf('&', secondEqualsIndex);
  String colorString = params.substring(secondEqualsIndex + 1, thirdAmpersandIndex);
  //  Serial.println(colorString);
  int fourthEqualsIndex = params.indexOf('=', thirdAmpersandIndex);
  String blinkString = params.substring(fourthEqualsIndex + 1);
  int blink = blinkString.toInt();
  //  Serial.println(blink);
  updateLED(line, pos, colorString, blink);
  return 1;
}


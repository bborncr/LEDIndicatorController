// API
// /id returns id and name in json
//
// /led?line=0&position=0&color=00FF00&blink=0
// line = 0-3, position = 0-19, color=hex color, blink = 0 fixed, 1 = blink
//
// /blinkrate?params=1000 
// /test starts test procedure

#include <SPI.h>
#include <Ethernet2.h>
#include <aREST.h>
#include "FastLED.h"
#include <Adafruit_SleepyDog.h>

#define NUM_STRIPS 4
#define NUM_LEDS_PER_STRIP 20

int blinkMode = 0;
int t = 1000;
int blinkArray[NUM_STRIPS][NUM_LEDS_PER_STRIP];
int colorArray[NUM_STRIPS][NUM_LEDS_PER_STRIP];

CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x41 };

// IP address in case DHCP fails
//IPAddress ip(192,168,10,2);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

void setup()
{
  // Start Serial
  Serial.begin(115200);

  // Functions to be exposed
  rest.function("led", ledControl);
  rest.function("blinkrate", setBlinkRate);

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
  Serial.print("Controller is at: ");
  Serial.println(Ethernet.localIP());

  FastLED.addLeds<NEOPIXEL, 5>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 6>(leds[1], NUM_LEDS_PER_STRIP);

  for (int i = 0; i < NUM_STRIPS; i++) {

    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      leds[i][j] = CRGB::Red;
      FastLED.show();
    }
    delay(1000);
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      leds[i][j] = CRGB::Green;
      FastLED.show();
    }
    delay(1000);
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      leds[i][j] = CRGB::Black;
      FastLED.show();
    }
  }
  Serial.println("Ready...");
  // Start watchdog
  int countdownMS = Watchdog.enable(4000);
  
}

void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);

  // Handle blinking
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      if (blinkArray[i][j] > 0) {
        // blink
        if (millis() % t > t / 2) {
          leds[i][j] = colorArray[i][j]; // get the previous color
        } else {
          leds[i][j] = 0x000000; // turn off
        }
      }
    }
  }
  FastLED.show();

  Watchdog.reset();

}

uint64_t StrToHex(const char* str)
{
  return (uint64_t) strtoull(str, 0, 16);
}

void updateLED(int l, int p, String c, int b) {
  int hexcolor = StrToHex(c.c_str());
  leds[l][p] = hexcolor;
  colorArray[l][p] = hexcolor;
  blinkArray[l][p] = b;
  FastLED.show();

}

// Custom function accessible by the API
// Modified to parse multiple parameters
int ledControl(String command) {

  String params = command;
  int firstAmpersandIndex = params.indexOf('&');
  String lineString = params.substring(0, firstAmpersandIndex);
  int lineNum = lineString.toInt();
  //  Serial.println(lineNum);
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
  updateLED(lineNum, pos, colorString, blink);
  return 1;
}

int setBlinkRate(String command) {
  t = command.toInt();
}


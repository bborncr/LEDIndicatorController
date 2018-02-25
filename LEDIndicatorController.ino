// API
// /id returns id and name in json
//
// /led?line=0&position=0&color=00FF00&blink=0
// line = 0-3, position = 0-19, color=hex color, blink = 0 fixed, 2 sec, 1 sec, .25 second
//
// command would be "0&position=0&color=00FF00&blink=0"
//
// trim()
// indexOf(&) read index -1 convert to int (line)
// indexOf(=) get int between = and & (position)
// readUntil= get hex between = and & (color)
// readUntil= get next int
//
// /test starts test procedure

#include <SPI.h>
#include <Ethernet.h>
#include <aREST.h>
//#include <avr/wdt.h>

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x41 };

// IP address in case DHCP fails
//IPAddress ip(192,168,10,2);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

// Variables to be exposed to the API
int chain_length;
String led_data;

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Init variables and expose them to REST API
  chain_length = 24;
  led_data = "rgbrgbbgrrgb";
  rest.variable("chain_length",&chain_length);
  rest.variable("led_data",&led_data);

  // Function to be exposed
  rest.function("led",ledControl);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("123456");
  rest.set_name("Corridor_A1");

  // Start the Ethernet connection and the server
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
//    Ethernet.begin(mac, ip);
  }
  server.begin();
  Serial.print("server is at ");
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

// Custom function accessible by the API
int ledControl(String command) {

  led_data = command;
  Serial.println(led_data);
  led_data.trim();
  return 1;
}

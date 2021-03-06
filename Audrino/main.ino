//
// SETUP
//


//https://create.arduino.cc/projecthub/reanimationxp/how-to-multithread-an-arduino-protothreading-tutorial-dd2c37
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#define WIFI_SSID "<REDACTED>"
#define WIFI_PASSWORD "<REDACTED>"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    5
#define LED_COUNT  60
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

ESP8266WebServer server(80);

//
// WEBSITE VARIABLES
//
int brightness = 25;
int Local_IP_Display = 0;



//
//  SETUP
//
void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();
  strip.setBrightness(brightness);
  colorWipe(strip.color(0, 0, 50), 5);
  fadeOutBlue(25,0,30);
  /* #region Setup WiFi */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    colorWipe(strip.color(0, 50, 30), 5);
    colorWipe(strip.color(0, 0, 0), 5);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  fadeInGreen(0,50,30);
  delay(1000);
  fadeOutGreen(50,0,30);

  Local_IP_Display = WiFi.localIP().toString().substring(WiFi.localIP().toString().lastIndexOf('.') + 1).toInt();

  /* set up the server */
  server.on("/", sendHtml);

  server.on(UriBraces("/update/brightness/{}"), []() {
    brightness = server.pathArg(0).toInt();
    sendRedirect();
  });


// Start server and listen for connections
  server.begin();
  Serial.println("HTTP server started");

  //come up with super epic way to show the ip address on the clock
  ShowIPOnClock(Local_IP_Display);// problem for future me
}



void loop() {
  server.handleClient();
  strip.setBrightness(brightness);
  rainbowFade(5,1);
}




















void sendHtml() {
  String response = R"(<!doctype html><title>ESP-8266 - 60 LED Ring</title><meta name=viewport content="width=device-width,initial-scale=1"><link href="http://fonts.googleapis.com/css?family=Lato:400,700" rel=stylesheet><link href=https://github.saeraphinx.dev/ColorRing/WebFiles/style.css rel=stylesheet><link rel=icon href=https://saeraphinx.dev/images/favicon.ico type=image/x-icon><body><script src=https://github.saeraphinx.dev/ColorRing/WebFiles/script.js></script><div class=topbar><p class=topbar>ESP-8266 Web Interface -<rainbow>NeoPixel 60 LED Ring</rainbow><p class="topbar right">Local IP: _LOCAL_IP_</div><div class=panels><div class="panel brightness"><p class=brightness>Brightness<p class=brightness>_BRIGHTNESS_</p><input class=brightness id=in_bright><br><button onclick=brightness()>Update</button></div></div>
  )";
  response.replace("_LOCAL_IP_", WiFi.localIP().toString());
  response.replace("_BRIGHTNESS_", brightness.toString());
  server.send(200, "text/html", response);
}

void sendRedirect() {
  server.sendHeader("Location", "/"); //hopefully it works lmao
  server.send(302, "text/plain", "");
}


void ShowIPOnClock(int last_digit_ip) {
  strip.setPixelColor(0, strip.Color(255, 255, 255));
  strip.setPixelColor(10, strip.Color(255, 255, 255));
  strip.setPixelColor(20, strip.Color(255, 255, 255));
  if ((last_digit_ip / 100) >= 1) {
    strip.setPixelColor(last_digit_ip / 100, strip.Color(255, 255, 255)); //first digit
    strip.setPixelColor(((last_digit_ip % 100) / 10) + 10, strip.Color(255, 255, 255)); //second digit
    strip.setPixelColor(((last_digit_ip % 100) % 10) + 20, strip.Color(255, 255, 255)); //third digit

  } else if((last_digit_ip / 10) >= 1) {
    strip.setPixelColor(last_digit_ip / 10, strip.Color(255, 255, 255)); //first digit
    strip.setPixelColor((last_digit_ip % 10) + 10, strip.Color(255, 255, 255)); //second digit
     //integer division hopefully

  } else {
    strip.setPixelColor(last_digit_ip, strip.Color(255, 255, 255)); //1st digit
  }
  
}

// For Startup Animation
void colorWipe(uint32_t color, int speed) {
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(speed);
  }
}

void fadeOutBlue(int start, int end, int speed) {
  for(int i=start; i>end; i--) {
    strip.fill(strip.Color(0, 0, i));
    strip.show();
    delay(speed);
  }
}

void fadeOutGreen(int start, int end, int speed) {
  for(int i=start; i>end; i--) {
    strip.fill(strip.Color(0, i, 0));
    strip.show();
    delay(speed);
  }
}

void fadeInGreen(int start, int end, int speed) {
  for(int i=start; i<end; i++) {
    strip.fill(strip.Color(0, i, 0));
    strip.show();
    delay(speed);
  }
}

// regular animation
void rainbowFade(int wait, int rainbowLoops) {

  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {

    for(int i=0; i<strip.numPixels(); i++) { 
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255, 255)));
    }

    strip.show();
    delay(wait);
}
}
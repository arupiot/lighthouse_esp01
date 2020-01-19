/*
 * lighthouse_esp01
 * ESP-01 control for LEDs
 * 
 * 2020 francesco.anselmo@arup.com
 * 
 * 25 white through hole ESP-01 controlled LEDs inside little ceramic lighthouses
 * for the 2020 Mark Fisher Scholarship event: 
 * A Love Letter to the Lighthouse in the Expanded Field
 * IP address numbers are from 21 to 45
 */

#define IP_ADDRESS 21 // last number of IP address
#define NODE_NAME "lighthouse1"


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define LED_PIN    2
#define DELAY      10

int transition = 1;
int brightness = 150;
 
const char *ssid = "SSID"; // change to WiFi SSID (requires an existing WiFi router
const char *password = "PASSWORD"; // change to real password
 
MDNSResponder mdns;
ESP8266WebServer server ( 80 );

void handleRoot() {
    digitalWrite ( BUILTIN_LED, 1 );
    String out = "<html><head><title>A Love Letter to the Lighthouse in the Expanded Field</title></head>";
    out += "<body style='background-color:gray'><center>";
    out += "<span style='display:block; width:100%; font-size:2em; font-family:Verdana; text-align:center'>Lighthouse lighting</span><br/>";
    out += "<a href='brighter'><span style='display:block; font-family:Verdana; background-color:lightgray; width:100%; height:3em;'>Brighter</span></a><br/>";
    out += "<a href='dimmer'><span style='display:block; font-family:Verdana; background-color:darkgray; foreground-color:white; width:100%; height:3em;'>Dimmer</span></a><br/>";
    out += "<a href='on'><span style='display:block; font-family:Verdana; background-color:white; width:100%; height:3em;'>On</span></a><br/>";
    out += "<a href='off'><span style='display:block; font-family:Verdana; background-color:black; width:100%; height:3em;'>Off</span></a><br/>";
    out += "</center></body>";
    out += "</html>";
    server.send ( 200, "text/html", out );
    digitalWrite ( BUILTIN_LED, 0 );
}

void handleNotFound() {
    digitalWrite ( BUILTIN_LED, 1 );
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }
    server.send ( 404, "text/plain", message );
    digitalWrite ( BUILTIN_LED, 0 );
}


void setup ( void ) {
    pinMode ( LED_PIN, OUTPUT );
    digitalWrite ( LED_PIN, 0 );

    delay(3000); // 3 second delay 
 
    Serial.begin ( 115200 );
    // config static IP
    IPAddress ip(192, 168, 1, IP_ADDRESS); // set desired IP Address
    IPAddress subnet(255, 255, 255, 0); // set subnet mask 
    IPAddress gateway(192, 168, 1, 1); // set gateway 
    IPAddress dns(8,8,8,8);
    //WiFi.config(ip, dns, gateway, subnet);
    WiFi.config(ip, gateway, subnet, dns);
    Serial.print(F("Setting static ip to : "));
    Serial.println(ip);
    WiFi.begin ( ssid, password );
    Serial.println ( "" );
    // Wait for connection
    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
    }
    
    Serial.println ( "" );
    Serial.print ( "Connected to " );
    Serial.println ( ssid );
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );
    if ( mdns.begin ( "esp8266", WiFi.localIP() ) ) {
        Serial.println ( "MDNS responder started" );
    }
    
    server.on ( "/", []() {handleRoot();} );
    server.on ( "/on", []() {setColor(1); handleRoot();} );
    server.on ( "/off", []() {setColor(0); handleRoot();} );
    server.on ( "/brighter", []() {brighter(); handleRoot();} );
    server.on ( "/dimmer", []() {dimmer(); handleRoot();} );
    server.onNotFound ( handleNotFound );
    server.begin();
    Serial.println ( "HTTP server started" );
}

void loop ( void ) {
    mdns.update();
    server.handleClient();
}

void setColor(int color) {
  int i;
  Serial.println(color);
  switch (color) {
    case 1:
      // LED on
      analogWrite(LED_PIN, 255);
      break;
    case 0:
      // LED off
      analogWrite(LED_PIN, 0);
      break;
  }
    delay(DELAY);
}



void brighter()
{
  brightness+=20;
  if (brightness>250) brightness=250;
  // put LED to desired brightness
  analogWrite(LED_PIN, brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
}

void dimmer()
{
  brightness-=20;
  if (brightness<0) brightness=0;
  // put LED to desired brightness
  analogWrite(LED_PIN, brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
}

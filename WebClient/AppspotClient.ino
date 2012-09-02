/*
  Web client

This sketch connects to a Google App Engine server (http://yourserver.appspot.com)
using an Arduino Wiznet Ethernet shield. 

Circuit:
* Ethernet shield attached to pins 10, 11, 12, 13

created 18 Dec 2009
by David A. Mellis

edited 31 Aug 2012
by Ankit Daftery
*/

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x90,0xA2,0xDA,0x00,0x09,0xD9};  //Replace with your Ethernet shield MAC

byte ip[] = { 192,168,2,8};  //The Arduino device IP address. DHCP server running on router, no other setting added

char appspotserver[] = "http://yourserver.appspot.com";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // start the serial library:
  Serial.begin(9600);

  // start the Ethernet connection:
  //Ethernet.begin(mac,ip,dns,gateway,subnet);
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  Serial.println("Began");
    
  delay(2000);
  
  Serial.println("connecting...");
  // if you get a connection, report back via serial:
  if (client.connect(server,80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /custompage?content=Your-content HTTP/1.1");
    client.println("HOST: yourapp.appspot.com");  // Necessary headers, or doesn't return results
    client.println("User-Agent: arduino-ethernet"); //Some additional headers
    client.println("Connection: close");
    client.println();
  } 
  else {
    // you didn't get a connection to the server:
    Serial.println("connection failed");
    for(;;)
      ;
  }
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them
  if (client.connected())
  {
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }  
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing forevermore:
    for(;;)
      ;   
  }
}
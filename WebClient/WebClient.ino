/*
  Web client
 
 This sketch connects to a Google App Engine server (http://yourserver.appspot.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 
 edited 3 Sep 2012
 by Ankit Daftery
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x90,0xA2,0xDA,0x00,0x09,0xD9};  //Replace with your Ethernet shield MAC

/*
 * Connection specific settings here
 * Necessary to get right or you'll keep on getting connection errors
 * 
 */

byte ip[] = { 192,168,2,8};
byte gateway[]= {172,28,10,254};
byte dns[]= {172,28,10,1};
byte subnet[]= {255,255,224,0};

char appspotserver[] = "http://yourapp.appspot.com";
char googleserver[] = "www.google.com";
char formserver[]= "https://docs.google.com";
char formkey[]= "your_form_key_here";

boolean foo=false;  // variable to ensure we make a request only once

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // start the serial library:
  Serial.begin(9600);
  // start the Ethernet connection:
  Ethernet.begin(mac,ip,dns,gateway,subnet);
  Serial.println("Began");
  Serial.println(Ethernet.localIP());    
  delay(2000);
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them
  if (!foo)
  {
  Serial.println("connecting...");
  if (client.connect(appspotserver,80)) //Select appropriate server
  {
  Serial.println("connected");  
  
  /*
   * The header format is vital in these requests
   */
  get_request("content=get+request+from+arduino","yourapp.appspot.com");
  post_request("content=posting from arduino","yourapp.appspot.com"); 
  form_request("entry.0.single=now&entry.1.single=sensorvalue&submit=Submit","docs.google.com"); //A form with two fields
  foo=true; //toggle the variable, now won't enter the loop again  
  } 
  }
  
  /*
   * Check for and print received data if any
   */

  if (client.connected())
  {
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  } 
  }

}

void post_request(String data,String host)
{
    client.println("POST /yourposturl HTTP/1.1");
    client.println("HOST: "+ host);  // Necessary headers, or doesn't return results
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("User-Agent: arduino-ethernet"); //Some additional headers
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();
}

void form_request(String data,String host)
{
    client.print("POST /spreadsheet/formResponse?formkey=");
    client.print(formkey);
    client.println("&ifq HTTP/1.1");
    client.println("HOST: "+ host);  // Necessary headers, or doesn't return results
    client.println("Connection: keep-alive");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("User-Agent: arduino-ethernet"); //Some additional headers
    client.print("Referer: https://docs.google.com/spreadsheet/viewform?formkey=");
    client.println(formkey);
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();
}


void get_request(String data,String host)
{
    client.print("GET /yourgeturl?content=");
    client.print(data);
    client.println(" HTTP/1.1");
    client.println("HOST: "+ host);  // Necessary headers, or doesn't return results
    client.println("User-Agent: arduino-ethernet"); //Some additional headers
    client.println("Connection: close");
    client.println();
}



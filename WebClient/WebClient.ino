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

#define APIKEY "YOUR API KEY STRING" // your cosm api key
#define FEEDID YOUR_FEED_ID // your feed ID
#define USERAGENT "Cosm Arduino Example" // user agent is the project name

byte ip[] = { 192,168,2,8};
byte gateway[]= {172,28,10,254};
byte dns[]= {172,28,10,1};
byte subnet[]= {255,255,224,0};

char appspotserver[] = "http://yourapp.appspot.com";
char googleserver[] = "www.google.com";
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
  // Connecting to docs.google.com won't work,connect to your own server or to the google.com instead for posting to google spreadsheets
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

void cosm_push(double dataToSend)
{
  if (client.connect("api.Cosm.com", 80)) {
    Serial.println("Connecting to Cosm...");

    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.print(".csv HTTP/1.1\n");
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.println("Host: api.cosm.com");

    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("Content-Length: ");
   // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 8 + getLength(dataToSend);
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.print("sensor1,");
    client.println(dataToSend);
    client.println();
  }
}



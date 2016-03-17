

#include <SPI.h>
#include <WiFi101.h>
#include <SD_XPro.h>
#include <Wire.h>
#include <Servo.h>
#include <stdlib.h>

#define HIGH 0
#define LOW 1

#define on 0
#define off 90

char ssid[] = "Wifi_Arduino";            // your network SSID (name)
char pass[] = "aarduino";

char command[10];
Servo left;
Servo right;
bool isSD;

int status = WL_IDLE_STATUS;

WiFiServer server(80);
String readString;
int pos = 0;
// Setup:
void setup() {


  left.attach(14);
  right.attach(15);
  // Start connection

  server.begin();
  Serial.begin(9600); // For debug
  pinMode(PIN_LED_13, OUTPUT); //Wireless LED
  left.writeMicroseconds(1500);
  right.writeMicroseconds(1500);
  
  //Start Wifi
  wifi_init();

  //Start SD
  if (!SD.begin(PIN_PB05)) {
    Serial.println("No SD card inserted!");
    isSD = false;
  }
  else
    isSD = true;
}

void wifi_init(void)
{
  digitalWrite(PIN_LED_13, LOW); //Wireless not connected

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED)
  {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    for (int i = 0; i < 10; ++i) {
      if (status == WL_CONNECTED) break;
      digitalWrite(PIN_LED_13, HIGH);
      delay(500);
      digitalWrite(PIN_LED_13, LOW);
      delay(500);
    }

  }

  digitalWrite(PIN_LED_13, HIGH); //Wireless connected

  server.begin();

  //  connected now, so print out the status:
  printWifiStatus();
}


// Main loop:
void loop() {

  WiFiClient client = server.available();

  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
        }

        //if HTTP request has ended
        if (c == '\n') {
          Serial.println(readString); //print to serial monitor for debuging
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();

          //Index
          if (readString.indexOf("/ ") > 0) {
            String file = "index.txt";
            File indexHTML = SD.open(file);
            if (isSD && indexHTML) {
              // read from the file until there's nothing else in it:
              while (indexHTML.available()) {
                client.write(indexHTML.read());
              }
              // close the file:
              indexHTML.close();
            } else { //Default
              Serial.print("No ");
              Serial.print(file);
              Serial.println(" found");
              char content[] = "<html>"
                               "File not found!"
                               "</html>";
              client.write(content);
            }            
          }
          else if (readString.indexOf("files/") > 0) {
            String url = parseQuery(readString);

            //Remove the "files"
            url.replace("files", "");
            if (url.length() == 0) break;

            Serial.print("Asking: ");
            Serial.println(url);

            File requestedFile = SD.open(url);
            if (isSD && requestedFile) {
              // read from the file until there's nothing else in it:
              while (requestedFile.available()) {
                client.write(requestedFile.read());
              }
              // close the file:
              requestedFile.close();
            }
            else {
              Serial.print("No ");
              Serial.print(url);
              Serial.println(" found");
              char content[] = "<html>"
                               "File not found!"
                               "</html>";
              client.write(content);
            }
          }
          else if (readString.indexOf("?Stop") > 0) {
            left.write(90);
            right.write(90);
          }
          else if (readString.indexOf("?Left") > 0) {
            left.write(180); //Backward
            right.write(180); //Forward
          }
          else if (readString.indexOf("?Right") > 0) {
            left.write(0); //Forward
            right.write(0); //Backward
          }
          else if (readString.indexOf("?Forward") > 0) {
            left.write(0); //Forward
            right.write(180); //Forward
          }
          else if (readString.indexOf("?Backward") > 0) {
            left.write(180); //Backward
            right.write(0); //Backward
          }
          else if (readString.indexOf("$") > 0) {
            String command = parseQuery(readString);

            //Remove the "$"
            command.replace("$", "");

            Serial.print("Command: ");
            Serial.println(command);

            // Servos:
            char s1[4] = {command[4], command[5], command[6], '\0'};
            //char s2[4] = {command[7], command[8], command[9], '\0'};

            // Convert char to int:
            int s1i = atol (s1);
            //int s2i = atol (s2);

            // Set servo position:
            left.write (s1i);
            //servo2.write (s2i);


          }

          delay(1);
          //stopping client
          client.stop();
          //clearing string for next read
          readString = "";

        }
      }
    }
  }

  delay(1);
  client.stop();


}

String parseQuery (String query) {
  if (query.startsWith("GET")) {
    query.replace("GET", "");
    query.replace("HTTP/1.1", "");
    query.replace("/", "");
    query.trim();
    return query;
  }
  else return "";
}
void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

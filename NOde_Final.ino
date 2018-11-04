#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include<SoftwareSerial.h>

SoftwareSerial Uno(D7, D8); // RX, TX

const char* ssid = "Insulation Lab";
const char* password = "hvenkroyiot";

// The ID below comes from Google Sheets.
// Towards the bottom of this page, it will explain how this can be obtained
const char *GScriptId = "AKfycbzNR_dMj3PdII0PVjqQ5BMaHxBddHiJRv7OsGhhCtQ7";

// Push data on this interval
const int dataPostDelay = 10000;  // 10 seconds

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";

const int httpsPort =  443;
HTTPSRedirect client(httpsPort);

// Prepare the url (without the varying data)
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "D2 49 07 79 6F CC A5 0B 1F 8B 70 08 26 BB 6A A5 3E 48 33 AD";

int percentage;

void setup() {
  Serial.begin(115200);
  Uno.begin(9600);
  Serial.print("Program Started");
  delay(1000);
  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());

  
  Serial.print(String("Connecting to "));
  Serial.println(host);

  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  // Connection Status, 1 = Connected, 0 is not.
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();
  
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    Serial.flush();
    return;
  }

//  // Data will still be pushed even certification don't match.
//  if (client.verify(fingerprint, host)) {
//    Serial.println("Certificate match.");
//  } else {
//    Serial.println("Certificate mis-match");
//  }
}

// This is the main method where data gets pushed to the Google sheet
void postData(String tag, float value){
  if (!client.connected()){
    Serial.println("Connecting to client again..."); 
    client.connect(host, httpsPort);
  }
  Serial.print(String("url" + url));
  String urlFinal = url+"percentage="+String(percentage);
  Serial.print("Final Url:  "); Serial.print(urlFinal);
  client.printRedir(urlFinal, host, googleRedirHost);
  Serial.print("Data Posted to Cloud");
  Uno.write("Data Posted to Cloud:  ");
}

// Continue pushing data at a given interval
void loop() {
  
  // Read analog value, in this case a soil moisture
    if(Uno.available())
 {
  percentage = Uno.read();
  Serial.print("Data Received :   ");   Serial.print(percentage); Serial.print("    ");
  Uno.flush();
 }

  // Post these information
  postData("Bin Status", percentage);
  
  delay (dataPostDelay);
  Serial.println("Loop completed    ");
}

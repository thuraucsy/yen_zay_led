/*
 *  ESP8266 JSON Decode of server response
 *  -Manoj R. Thkuar
 *  https://circuits4you.com
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <TM1637Display.h>


// WiFi Parameters
const char* wifiName = "YOUR_WIFI_SSD_NAME";
const char* wifiPass = "YOUR_WIFI_PASSWORD";

//Web Server address to read/write from 
//const char *host = "http://arduinojson.org/example.json";
const char *host = "http://thuraucsy.github.io/yen_zay/v1/day/today.json";

const int CLK = D2; //Set the CLK pin connection to the display
const int DIO = D3; //Set the DIO pin connection to the display
TM1637Display display(CLK, DIO); //set up the 4-Digit Display.

void setup() {
  
  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  Serial.print("Connecting to ");
  Serial.println(wifiName);

  WiFi.begin(wifiName, wifiPass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP
    
  display.setBrightness(0x0a); //set the diplay to maximum brightness
  display.showNumberDecEx(0000, 0b01000000);
}

void loop() {
  
  HTTPClient http;    //Declare object of class HTTPClient

  Serial.print("Request Link:");
  Serial.println(host);
  
  http.begin(host);     //Specify request destination
  
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload from server

  Serial.print("Response Code:"); //200 is OK
  Serial.println(httpCode);   //Print HTTP return code

  Serial.print("Returned data from Server:");
  Serial.println(payload);    //Print request response payload
  
  if(httpCode == 200)
  {
    // Allocate JsonBuffer
    // Use arduinojson.org/assistant to compute the capacity.
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(24) + 24*JSON_OBJECT_SIZE(4) + 3000;
    DynamicJsonBuffer jsonBuffer(capacity);
  
   // Parse JSON object
    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success()) {
      Serial.println(F("Parsing failed!"));
      return;
    }
    
    int count = (int) root["Count"];
    double yenZayDouble = (float(root["Items"][count - 1]["MMKRatePerYen"]) * 100) + 0.5;

    Serial.println(yenZayDouble);
    display.showNumberDecEx(yenZayDouble, 0b01000000); //Display the numCounter value;
  }
  else
  {
    Serial.println("Error in response");
  }

  http.end();  //Close connection
  
  delay(60000);  //GET Data at every min
}

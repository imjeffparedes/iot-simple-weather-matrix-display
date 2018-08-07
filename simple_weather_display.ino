
/**
 * Easy Matrix Weather Display
 * Weather Matrix Display
 * by Jefferson Paredes
 * Version: 1.0.0
 * 
 * Simple introduction to Internet of Things project 
 * Check this project on Instructables.com
 * Link: https://www.instructables.com/id/Weather-Matrix-Display/
 * 
 * Contribute to improve this project on github!
 * Link: https://github.com/imjeffparedes/iot-simple-weather-matrix-display
 * 
 * Need Help?
 * Please email me here: imjeffparedes@gmail.com
 * Please Follow me on instagram: @imjeffparedes
 * Licensed under Creative Commons BY-NC-SA
 */
#include <Thread.h>
#include <SPI.h>
#include <WiFi101.h>
#include<HexFont.h>
#include <ArduinoJson.h>


/***** Weather Objects ****/
Thread updateWeatherThread = Thread();
Thread checkInternetThread = Thread();

// Lets use thread to check the condition every interval
Thread apiRequestThread = Thread();

// set time interval of each request in seconds
int timeInterval  = 10; 

// set time counter to start
int timeCount = 0;

char ssid[] = ""; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

String apiKey = "8f0ff191defb4a20b5583518171203"; // your apixu api key
String coordinates = "34.0522,118.2437"; // your location coordinates

int status = WL_IDLE_STATUS;

char server[] = "api.apixu.com";    // your api address

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

// save json result here
String readResult;

// tells if the data is already collected
bool dataCollected = false;

// curent weather condition
String currentCondition;
String currentConditionCode;

/*  1000 Sunny
 *  1003 Partly Cloudy 
 *  1006 Cloudy
 *  1009 Overcast
 *  1030 Mist
 *  1063 Patchy rain nearby
 *  1066 Patchy snow nearby
 *  1069 Patchy sleet nearby
 *  1072 Patchy freezing drizzle nearby
 *  1087 Thundery outbreaks in nearby
 *  1114 Blowing snow
 *  1117 Blizzard
 *  1135 Fog
 *  1147 Freezing fog
 *  1150 Patchy light drizzle
 *  1153 Light drizzle
 *  1168 Freezing drizzle
 *  1171 Heavy freezing drizzle
 *  1180 Patchy light rain
 *  1183 Light rain
 *  1186 Moderate rain at times
 *  1189 Moderate rain
 *  1192 Heavy rain at times
 *  1195 Heavy rain
 *  1198 Light freezing rain
 *  1201 Moderate or heavy freezing rain
 *  1204 Light sleet
 *  1207 Moderate or heavy sleet
 *  1210 Patchy light snow
 *  1213 Light snow
 *  1216 Patchy moderate snow
 *  1219 Moderate snow
 *  1222 Patchy heavy snow
 *  1225 Heavy snow
 *  1237 Ice pellets
 *  1240 Light rain shower
 *  1243 Moderate or heavy rain shower
 *  1246 Torrential rain shower
 *  1249 Light sleet showers
 *  1252 Moderate or heavy sleet
 *  1255 Light snow showers
 *  1258 Moderate or heavy snow showers
 *  1261 Light showers of ice pellets
 *  1264 Moderate or heavy showers
 *  1273 Patchy light rain in area with thunder
 *  1276 Moderate or heavy rain in area with thunder
 *  1279 Patchy light snow in area 
 *  1282 Moderate or heavy snow in area with thunder
 * 
 */


 /**********End of weather objects ********/


/***** Display Objects ************/
Thread refreshThread = Thread();
Thread animateThread = Thread();


// 2-dimensional array of row pin numbers:
const int col[8] = {
  10,3,2,7,0,8,12,13 
};

// 2-dimensional array of column pin numbers:
const int row[8] = {
  6,11,20,9,5,21,4,1
};

// 2-dimensional array of pixels:
int pixels[8][8],count=1000;
int supportedASCICount = 0;
byte currentChar[8] , nextChar[8] ;
int currentLetter = 0 ;
String message = "Loading...";

 /**********End of display objects ********/

Thread sampleLoopThread = Thread();

void setup() {
  // initialize the I/O pins as outputs
  // iterate over the pins:
  for (int thisPin = 0; thisPin < 8; thisPin++) {
    // initialize the output pins:
    pinMode(col[thisPin], OUTPUT);
    pinMode(row[thisPin], OUTPUT);
    // take the col pins (i.e. the cathodes) high to ensure that
    // the LEDS are off:
    digitalWrite(col[thisPin], HIGH);
  }
  for (int x = 0; x < 8; x++) {
   currentChar[x] = cp437_font[supportedAsci.indexOf(" ")][x];
  }

  refreshThread.onRun(refreshScreen);
  
  animateThread.onRun(slideRight);
  animateThread.setInterval(100);
  
  setupChar();

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // start update weather  
  checkInternetThread.onRun(checkConnection);
  // set one second delay for every loop
  // this will be the basis of request interval
    // wait 10 seconds for connection:
  checkInternetThread.setInterval(1);
  
  
  // start weather request 
  apiRequestThread.onRun(getCondition);
  // set one second delay for every loop
  // this will be the basis of request interval
  apiRequestThread.setInterval(5000);



  // start update weather  
  updateWeatherThread.onRun(updateWeatherCondition);
  // set one second delay for every loop
  // this will be the basis of request interval
  updateWeatherThread.setInterval(100);

}

void loop() {
  // refresh screen display
  // checks if thread should run
  if(refreshThread.shouldRun())
    refreshThread.run();

    
  if(animateThread.shouldRun())
    animateThread.run();

   
  // checks if thread should run
  if(checkInternetThread.shouldRun())
    checkInternetThread.run();

    
  // checks if thread should run
  if(apiRequestThread.shouldRun())
    apiRequestThread.run();

    
  // checks if thread should run
  if(updateWeatherThread.shouldRun())
    updateWeatherThread.run();
  
}

void setupChar(){
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      bool v = covertToBin(currentChar[x])[y] == '1';
      if(v){
        pixels[x][y] = LOW;
      }else{
        pixels[x][y] = HIGH;
      }
    }
  }
}

void refreshScreen() {
  // iterate over the rows (anodes):
  for (int thisRow = 0; thisRow < 8; thisRow++) {
    // take the row pin (anode) high:
    digitalWrite(row[thisRow], HIGH);
    // iterate over the columns (cathodes):
    for (int thisCol = 0; thisCol < 8; thisCol++) {
      // get the state of the current pixel;
      int thisPixel = pixels[thisRow][thisCol];
      // when the row is HIGH and the column is LOW,
      // the LED where they meet turns on:
      digitalWrite(col[thisCol], thisPixel);
      // turn the pixel off:
      if (thisPixel == LOW) {
        digitalWrite(col[thisCol], HIGH);
      }
    }
    // take the row pin low to turn off the whole row:
    digitalWrite(row[thisRow], LOW);
  }
}

String covertToBin(byte rowHex){
  String binString =  String(rowHex,BIN);
  int  binStringCount = binString.length();
  for(int i=0; i< 8-binStringCount ;i++){
    binString = "0" +binString;
  }
  return binString;
}

int currentCol = 0;

void slideRight(){

  int colTransferStart = 7-currentCol;
  
  // move all active led one column to left
  for(int x= 0 ; x<colTransferStart ;x++){
    currentChar[x] = currentChar[x+1];
  }

  // copy current letter starts from last column of previous
  int colCountOfCurrentLetter = 0;
  for(int x= colTransferStart ; x<8 ;x++){
    currentChar[x] = cp437_font[supportedAsci.indexOf( message[currentLetter] )][ colCountOfCurrentLetter];
    colCountOfCurrentLetter++;
  }
  currentCol++;  
  if(currentCol > 7){
    currentCol=0;
    currentLetter++;
  }
  //if message is complete 
  if(currentLetter==message.length()){
    currentLetter=0;
  }
  setupChar();
}




/***** Weather Data Function ******/


void printWiFiStatus() {
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

// this function will make api request
void makeRequest(){
   Serial.println("\nStarting connection to server...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
      Serial.println("connected to server");
      // Make a HTTP request:
      client.println("GET /v1/current.json?key="+ apiKey +"&q=" +coordinates+ " HTTP/1.1");
      client.println("Host: api.apixu.com");
      client.println("Connection: close");
      client.println();
    } 
    
}

// loop 
void getCondition(){
  // if we reached the time interval
  // do a request
  if(timeInterval == timeCount && status == WL_CONNECTED){
    dataCollected = false;

    makeRequest();
    
    //set time count back to start
    timeCount = 0;
  }
  // count time after 1 second 
  timeCount++; 
  
}

void updateWeatherCondition(){
  
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    readResult += c;
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected() && !dataCollected) 
  {
    Serial.println();
    // Copy from start of json result
    // if we have { means that the start of json values
    String rawJson = readResult.substring(readResult.indexOf("{")) ;

    // initialize json parser
    DynamicJsonBuffer jsonBuffer;
    JsonObject& result = jsonBuffer.parseObject(rawJson);
  
    //update display text to new weather condition
    message = "";
    
    //get the current condition
    String condition =  result["current"]["condition"]["text"];
    appendMessage(condition);
    appendMessage(" ");
    String temp_c = result["current"]["temp_c"] ;
    appendMessage(temp_c);
    appendMessage("C");
    appendMessage(" ");
    String humidity = result["current"]["humidity"];
    appendMessage(humidity);
    appendMessage("H");
    appendMessage(" ");
     
    Serial.println("Current condition: "+ message);
    
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    Serial.println("Connection closed.");

    dataCollected = true;
  }
}

int checkInternetCounter = 0;
int checkInternetTimeInterval = 10000;
bool connectionStatusPrinted = false;

void checkConnection(){

    //update display message when connecting to Wifi
    if(status!= WL_CONNECTED && message!="Connecting to WIFI..."){
      changeMessage("Connecting to WIFI..."); 
      connectionStatusPrinted = false;
    }

    //check if needed to connect to wifi
   if(checkInternetCounter == checkInternetTimeInterval && status!= WL_CONNECTED){
        // attempt to connect to WiFi network:
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        Serial.println("Please wait...");
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
        checkInternetCounter = 0;
   } 
   checkInternetCounter++;

   //count again if check internet time interval is already met
   if(checkInternetCounter>checkInternetTimeInterval)
      checkInternetCounter = 0;

    //update display message when checking data
    if(status == WL_CONNECTED && !connectionStatusPrinted){
      Serial.println("Connected to wifi");
     changeMessage("Checking data...");
      
      connectionStatusPrinted = true;
      printWiFiStatus();
    }
}

void changeMessage(String msg){
  message = msg;
  // add long space after displaying message
  message+="  ";
  currentLetter = 0;
}


void appendMessage(String msg){
  message += msg;
  currentLetter = 0;
}




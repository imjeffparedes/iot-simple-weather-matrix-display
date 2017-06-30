# Weather Matrix Display

Arduino code for Mini weather status display from a 8x8 LED matrix display.
Using Genuino MKR1000 to fetch temperature, humidity and condition of weather within a selected location.
* Version v1.0.0 | By Jeff Paredes

## Getting Started

These instructions will show you how to use the code

### Prerequisite Libraries

1. Thread.h
2. WiFi101.h
3. ArduinoJson.h


### Installing

Please follow these step by step series of examples that tell you have to use this codes

1. Install prerequisite libraries

2. Add the attached **HexFont** folder to your Arduino libraries.

3. Open **simple_weather_display.ino** with your Arduino IDE

4. Replace with your WiFi settings
```
char ssid[] = ""; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)
```

5. Replace with your Apixu Api Key and location coordinates
```
String apiKey = "8f0ff191defb4a20b5583518171203"; // your apixu api key
String coordinates = "14.3335591,120.9758737"; // your location coordinates
```

6. You can also changed what data to be displayed on this part of the code
```
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
 ```

7. For example if you want to add wind_mph from apixu api result:
```
String wind_mph = result["current"]["wind_mph"] ;
appendMessage(wind_mph);
appendMessage("Wind mph");
appendMessage(" ");
```

## Deployment

Flash Your MKR1000 using the code

Please visit [Hardware Instruction Page](https://www.instructables.com/id/Weather-Matrix-Display/) on Instructables.com

## Built With

* [Arduino IDE](https://www.arduino.cc/en/Main/Software) - Programming IDE Used

## Contributing

Please read [CONTRIBUTING.md](https://github.com/imjeffparedes/) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* **Jefferson Paredes** - *Developer* - [imjeffparedes](https://github.com/imjeffparedes/)

## License

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/imjeffparedes/) file for details

## Acknowledgments

* Publishing Page - [Instructables](https://www.instructables.com/id/Weather-Matrix-Display/)
* Inspiration - [E.M.I.L.Y](https://www.instructables.com/id/Electronic-Matrix-I-Love-You/)
* Sponsors - [Hive Electronics](https://www.facebook.com/HiveElectronics/)

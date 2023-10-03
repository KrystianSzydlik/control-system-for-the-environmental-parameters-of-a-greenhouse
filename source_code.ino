
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#include "FS.h"


  bool is1V1Output = false;
const int analogInPin = A0;
int minADC = 38;
int maxADC = 940;
const int relay = 14;
const int relay2 = 12;
int moistureValue, mappedValue;

int Podlewanko; 
int Nawilzanko;




#define DHTPIN 5

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

const char* ssid = "nazwa sieci wifi";
const char* password = "haslo";

const char* PARAM_INT = "inputInt";
const char* PARAM_INT2 = "inputInt2";

float t = 0.0;
float h = 0.0;
float hg = 0.0;



const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Interfejs parametry srodowiskowe</title>
  <meta name="viewport" content="width=device-width, initial-scale=1"> 

  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  
  <style>
    html {
      font-family: Times New Roman;
      display: flex;
      align-items: center;
      justify-content: center;
      height: 100%%;
    }
    
    body {
      text-align: center;
    }
    
    h2 {
      font-size: 3.5rem;
    }
    
    p {
      font-size: 3.5rem;
    }
    
    .jednostki {
      font-size: 1.5rem;
    }
    
    .Odczyt-DHT {
      font-size: 1.8rem;
      vertical-align: middle;
      padding-bottom: 20px;
    }

       input {
      width: 15%%;
      padding: 12px 20px;
      margin: 8px 0;
      box-sizing: border-box;
      border: 2px solid green;
      border-radius: 2px;
    }
  </style>
</head>

<body>
  <h2>Parametry srodowiskowe szklarni</h2>
  
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="Odczyt-DHT">Temperatura</span> 
    <br>
    <span id="Temperatura">%Temperatura%</span>
    <sup class="jednostki">&deg;C</sup>
  </p>
  
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="Odczyt-DHT">Wilgoc w powietrzu</span>
    <br>
    <span id="Wilgoc">%Wilgoc%</span>
    <sup class="jednostki">&#37;</sup>
  </p>
  
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="Odczyt-DHT">Wilgotnosc gleby</span>
    <br>
    <span id="WilgocGleby">%WilgocGleby%</span>
    <sup class="jednostki">&#37;</sup>
  </p>
  
  <br>
  
  <form action="/get" target="hidden form">
    <div class="Odczyt-DHT">
      <i class="fa fa-leaf" style="color:#12ef4c;"></i>
      Podlewanie:
      <input type="number" name="inputInt" name="inputField">
      <input type="submit" value="wyslij" id="inputField">
    </div>
  </form>
  
  <br>
  
  <form action="/get">
    <div class="Odczyt-DHT">
      <i class="fas fa-water" style="color:#00add6;"></i>
      Nawilzanie:
      <input type="number" name="inputInt2" name="inputField2">
      <input type="submit" value="wyslij" id="inputField2">
    </div>
  </form>
  
</body>

<script>
  setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("Temperatura").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/Temperatura", true);
    xhttp.send();
  }, 10000);

  setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("Wilgoc").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/Wilgoc", true);
    xhttp.send();
  }, 10000);

  setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("WilgocGleby").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/WilgocGleby", true);
    xhttp.send();
  }, 10000);
</script>

</html>
)rawliteral";

unsigned long previousMillis = 0;  // Kiedy ostatnio DHT był zaktualizowany

// aktualizacja odczytów Dht co 10s
const long interval = 10000;

String processor(const String& var) {  // funkcja processor(), zastępuje placeholdery w tekście HTML rzeczywistymi wartościami temperatury i wilgotności.
  //Serial.println(var);
  if (var == "Temperatura") {  //Kiedy strona internetowa jest zażądana, sprawdza, czy HTML ma jakieś placeholdery. Jeśli znajdzie placeholder %TEMPERATURA%,
    return String(t);          //zwraca temperaturę, która jest przechowywana na zmiennej t.
  } else if (var == "Wilgoc") {
    return String(h);
  } else if (var == "WilgocGleby") {
    return String(hg); 
  }


  return String();
}

void setup() {

  Serial.begin(115200);
  dht.begin();           //Inicjalizacja czujnika DHT
  pinMode(relay,OUTPUT); 
   pinMode(relay2,OUTPUT);



  //łączenie z siecią wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  Serial.println(WiFi.localIP());



  //obsługa serwera WWW.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {  //Kiedy wykonuje żądanie na głównym adresie URL, wysyła tekst HTML, który jest przechowywany w zmiennej index_html.
    request->send_P(200, "text/html", index_html, processor);   
  });

  
  server.on("/Temperatura", HTTP_GET, [](AsyncWebServerRequest* request) {  //dodanie dwów dodatkowych handlerów, aby zaktualizować odczyty temperatury i wilgotności.
    request->send_P(200, "text/plain", String(t).c_str());                  //Kiedy otrzyma żądanie na adres URL /temperatura,  to wysyła zaktualizowaną wartość temperatury.
  });
                                                                          //Jest to zwykły tekst i powinien być wysłany jako znak, więc używta została metody c_str().
  server.on("/Wilgoc", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", String(h).c_str());
  });
  
  
  server.on("/WilgocGleby", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", String(hg).c_str());
  });

  
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
    String inputMessage;
    String inputParam;
    
    if (request->hasParam(PARAM_INT)) {
      inputMessage = request->getParam(PARAM_INT)->value();
    
      Podlewanko = inputMessage.toInt();

      inputParam = PARAM_INT;
      
      
      
    }
    
    else if (request->hasParam(PARAM_INT2)) {
      inputMessage = request->getParam(PARAM_INT2)->value();
      
      Nawilzanko = inputMessage.toInt();

      inputParam = PARAM_INT2;
      
    }
    else{
    inputMessage = "nie wyslano wartosci zmiennej";
    inputParam = "none";
  }
    Serial.println(inputMessage);
    request->send(200,"text/html", "ZAPYTANIE HTTP GET wyslane na esp poprzez pole input ("+ inputParam + ")o wartosci: "+ inputMessage + "<br><a href=\"/\">Powrot do strony glownej</a>");
  });



  server.begin();  
}

void loop() {

moistureValue = analogRead(analogInPin);
mappedValue = map(moistureValue,minADC,maxADC,0,100);



if (mappedValue < Podlewanko && Podlewanko != 0 ){ 
  digitalWrite(relay,LOW);
  delay(150);
}
  else{
  digitalWrite(relay,HIGH);
  delay(150);
}
 
if (h < Nawilzanko && Nawilzanko !=0 ){
digitalWrite(relay2,LOW);
delay(150);
}
  else{
    digitalWrite(relay2,HIGH);
    delay(150);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {  //sprawdza, czy nadszedł czas, aby uzyskać nowe odczyty z czujnika

    

    previousMillis = currentMillis; 
    

Serial.print("analog value: ");
Serial.println(moistureValue);


    float newT = dht.readTemperature();  

    if (isnan(newT)) {
      Serial.println("Błąd zczytywania z czujnika DHT22!!");
    } else {  //jeśli zmienna newT jest prawidłowym odczytem temperatury, aktualizujemy zmienną t.
      t = newT;
      Serial.println(t);
    }

    float newH = dht.readHumidity(); 

    if (isnan(newH)) {
      Serial.println("Błąd zczytywania z czujnika DHT22!!");
    } else {
      h = newH;
      Serial.println(h);
    }


    float newHg = mappedValue; 

    if (isnan(mappedValue)) {
      Serial.println("Błąd zczytywania z czujnika wilgoci GLEBY!!");
    } else {
      hg = newHg;
      Serial.print("Wilgoc gleby: ");
      Serial.println(mappedValue);
    }
    Serial.print("zadana wilgoc gleby = ");
    Serial.println(Podlewanko);
  }
}

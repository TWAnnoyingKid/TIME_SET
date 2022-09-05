#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <Timer.h>

const char *ssid     = "L0220_619";
const char *password = "12345678";
const int output5 = 5;
int T;

ESP8266WebServer server ( 1234 );

const long utcOffsetInSeconds = 28800;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
Timer T1;
Timer T2;
Timer T3;
Timer T4;
Timer T5;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

char htmlResponse[3000];

void handleRoot() {

  snprintf ( htmlResponse, 3000,
"<!DOCTYPE html>\
<html lang=\"en\">\
  <head>\
    <meta charset=\"utf-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  </head>\
  <body>\
          <h1>Time</h1>\
          <h2 id='time'>00:00:00</h2> \
          <input type='text' name='date_hh' id='date_hh' size=3 autofocus> Hr \
          <input type='text' name='date_mm' id='date_mm' size=3 autofocus> Min \
          <input type='text' name='date_ss' id='date_ss' size=3 autofocus> Sec \
          <div>\
          <br><button id=\"save_button\">Save</button>\
          </div>\
    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>\    
    <script>\
      var hh;\
      var mm;\
      var ss;\
      $('#save_button').click(function(e){\
        e.preventDefault();\
        hh = $('#date_hh').val();\
        mm = $('#date_mm').val();\
        ss = $('#date_ss').val();\        
        $.get('/save?hh=' + hh + '&mm=' + mm + '&ss=' + ss, function(data){\
          console.log(data);\
        });\
        document.getElementById('time').textContent = hh + ' : ' + mm + ' : ' + ss ;\
      });\      
    </script>\
  </body>\
</html>"); 

   server.send ( 200, "text/html", htmlResponse );  

}


void handleSave() {
  String A = (server.arg("hh") + ":" + server.arg("mm") + ":" + server.arg("ss"));
  if ((server.arg("hh")!= "") and (server.arg("mm")!= "") and (server.arg("ss")!= "")){
    Serial.println("Time_Set : " + A);
  }
}

void TIME_COUNT(){
  int hh = server.arg("hh").toInt();
  int mm = server.arg("mm").toInt();
  int ss = server.arg("ss").toInt();
  int HH = (timeClient.getHours());
  int MM = (timeClient.getMinutes());
  int SS = (timeClient.getSeconds());
  Serial.print(hh);
  Serial.print(mm);
  Serial.println(ss);
  Serial.print(HH);
  Serial.print(MM);
  Serial.println(SS);

  if ((hh == HH) and (mm == MM) and (ss == SS)){
    T4.pulse(output5, 0, HIGH);
    T5.pulse(output5, 10000, LOW);
    T1.every(86400000,GPIO5on);
    T2.every(86410000,GPIO5off);
  }
}

void GPIO5on() {
  digitalWrite(output5, HIGH);
  Serial.println("GPIO5on");
}
void GPIO5off() {
  digitalWrite(output5, LOW);
  Serial.println("GPIO5off");
}

void setup(){
  Serial.begin(115200);
  pinMode(output5, OUTPUT);
  digitalWrite(output5, LOW);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on ( "/", handleRoot );
  server.on ("/save", handleSave);

  server.begin();
  Serial.println ( "HTTP server started" );
  timeClient.begin();
  T3.every(1000,TIME_COUNT);
}

void loop() {
  timeClient.update();{
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());
  delay(1000);
  }
//  int hh = server.arg("hh").toInt();
//  int mm = server.arg("mm").toInt();
//  int ss = server.arg("ss").toInt();
//  int HH = (timeClient.getHours());
//  int MM = (timeClient.getMinutes());
//  int SS = (timeClient.getSeconds());
  server.handleClient();
//
//  if ((hh == HH) and (mm == MM) and (ss == SS)){
//    T1.every(86400000,GPIO5on);
//    T2.every(86410000,GPIO5off);
//  }
  T1.update();
  T2.update();
  T3.update();
  T4.update();
  T5.update();
  
}

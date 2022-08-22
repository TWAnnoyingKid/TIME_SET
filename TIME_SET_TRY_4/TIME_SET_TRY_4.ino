#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "L0220_619";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

ESP8266WebServer server(1234);

const long utcOffsetInSeconds = 28800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

uint8_t LED1pin = D7;
bool LED1status = LOW;

uint8_t LED2pin = D6;
bool LED2status = LOW;

uint8_t LED3pin = D1;
bool LED3status = LOW;


void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handle_OnConnect);
  server.on ("/save", handleSave);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  timeClient.begin();
}
void loop() {
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}
  
  if(LED2status)
  {digitalWrite(LED2pin, HIGH);}
  else
  {digitalWrite(LED2pin, LOW);}

  if(LED3status)
  {digitalWrite(LED3pin, HIGH);}
  else
  {digitalWrite(LED3pin, LOW);}
  
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
  int hh = server.arg("hh").toInt();
  int mm = server.arg("mm").toInt();
  int ss = server.arg("ss").toInt();
  int HH = (timeClient.getHours());
  int MM = (timeClient.getMinutes());
  int SS = (timeClient.getSeconds());
  server.handleClient();

  if ((hh == HH) and (mm == MM) and (ss == SS)){
    Serial.println("time's up");
    delay(10*1000);
  }
}

void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  LED3status = LOW;
  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status)); 
}

void handleSave() {
  LED3status = HIGH;
  String A = (server.arg("hh") + ":" + server.arg("mm") + ":" + server.arg("ss"));
  if ((server.arg("hh")!= "") and (server.arg("mm")!= "") and (server.arg("ss")!= "")){
    Serial.println("Time_Set : " + A);
    server.send(200, "text/html", SendHTML(LED1status,LED2status,HIGH)); 
  }
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status,LED3status)); 
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status,LED3status)); 
}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("GPIO6 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true,LED3status)); 
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false,LED3status)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat,uint8_t led2stat,uint8_t led3stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h2 {color: #444444;margin-bottom: 50px;font-size: 50px;display: inline} h3 {color: #444444;margin-bottom: 50px;font-size: 50px;display: inline} h4 {color: #444444;margin-bottom: 50px;font-size: 50px;display: inline}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  ptr +="<h2 id='Hr'>00</h2> : \n";
  ptr +="<h3 id='Min'>00</h3> : \n";
  ptr +="<h4 id='Sec'>00</h4><br>\n";
  ptr +="<input type='text' name='date_hh' id='date_hh' size=3 autofocus> Hr\n"; 
  ptr +="<input type='text' name='date_mm' id='date_mm' size=3 autofocus> Min\n";
  ptr +="<input type='text' name='date_ss' id='date_ss' size=3 autofocus> Sec\n";
  ptr +="<div>\n";
  ptr +="<br><button id=\"save_button\">Save</button>\n";
  ptr +="</div>\n";
  ptr +="<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>\n";   
  ptr +="<script>\n";
  ptr +="  var hh;\n";
  ptr +="  var mm;\n";
  ptr +="  var ss;\n";
  ptr +="  $('#save_button').click(function(e){\n";
  ptr +="    e.preventDefault();\n";
  ptr +="    hh = $('#date_hh').val();\n";
  ptr +="    mm = $('#date_mm').val();\n";
  ptr +="    ss = $('#date_ss').val();\n";        
  ptr +="    $.get('/save?hh=' + hh + '&mm=' + mm + '&ss=' + ss, function(data){\n";
  ptr +="      console.log(data);";
  ptr +="    });";
  if(led3stat){
    ptr +="    document.getElementById('Hr').textContent = hh;\n";
    ptr +="    document.getElementById('Min').textContent = mm;\n";
    ptr +="    document.getElementById('Sec').textContent = ss;\n";
  }
  ptr +="  });\n";      
  ptr +="</script>\n";
  
  if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

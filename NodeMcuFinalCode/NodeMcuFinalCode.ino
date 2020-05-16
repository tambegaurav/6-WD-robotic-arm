#include<ESP.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define DIR_FORWARD 1
#define DIR_REVERSE 2
#define DIR_LEFT 3
#define DIR_RIGHT 4



// Pins definition for ESP8266
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define SD3 10
#define SD2 9




int in1A, in1B, EN1, EN2, in2A, in2B;
void mDriverInit(int ina, int inb, int inc, int ind, int en1, int en2)
{
  /*Set pin values*/
  in1A = ina;
  in1B = inb;
  in2A = inc;
  in2B = ind;
  EN1 = en1;
  EN2 = en2;

  /*Configure pins*/
  pinMode(in1A, OUTPUT);
  pinMode(in1B, OUTPUT);
  pinMode(in2A, OUTPUT);
  pinMode(in2B, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  /*initialize by stopping it*/
  mStop();
}

void mStop()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
  digitalWrite(in1A, LOW);
  digitalWrite(in1B, LOW);
  digitalWrite(in2A, LOW);
  digitalWrite(in2B, LOW);
}

void mMove(int direction)
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  switch (direction)
  {
    case DIR_FORWARD:
      digitalWrite(in1A, HIGH);
      digitalWrite(in1B, LOW);
      digitalWrite(in2A, HIGH);
      digitalWrite(in2B, LOW);
      break;

    case DIR_REVERSE:
      digitalWrite(in1A, LOW);
      digitalWrite(in1B, HIGH);
      digitalWrite(in2A, LOW);
      digitalWrite(in2B, HIGH);
      break;

    case DIR_LEFT:
      digitalWrite(in1A, HIGH);
      digitalWrite(in1B, LOW);
      digitalWrite(in2A, LOW);
      digitalWrite(in2B, HIGH);
      break;

    case DIR_RIGHT:
      digitalWrite(in1A, LOW);
      digitalWrite(in1B, HIGH);
      digitalWrite(in2A, HIGH);
      digitalWrite(in2B, LOW);
      break;

    default:
      ;
  }
}




String webpage ="<html> <body> <h1><b> Control Servo Via Web Page</b></h1> <h3>Scroll these sliders to control the servos of the Robotic Arm</h3> <script> function send(servonum, value){ var message=\"\"; message = \"SERVO=\" + servonum + \"&value=\" + value; var xhttp = new XMLHttpRequest(); xhttp.open(\"POST\", \"/?\"+message, true); xhttp.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\"); xhttp.send(); } function mMove(val) { var message=\"\"; message = \"MOTOR\" + \"=\" + val; var xhttp = new XMLHttpRequest(); xhttp.open(\"POST\", \"/?\"+message, true); xhttp.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\"); xhttp.send(); } </script> <input type=\"range\" min=\"0\" max=\"180\" oninput=\"send(1,this.value)\" /> <p>Servo 1</p> <input type=\"range\" min=\"0\" max=\"180\" oninput=\"send(2,this.value)\" /> <p>Servo 2</p> <input type=\"range\" min=\"0\" max=\"180\" oninput=\"send(3,this.value)\" /> <p>Servo 3</p> <input type=\"range\" min=\"0\" max=\"180\" oninput=\"send(4,this.value)\" /> <p>Servo 4</p> <input type=\"range\" min=\"0\" max=\"180\" oninput=\"send(5,this.value)\" /> <p>Servo 5</p> <input type=\"range\" min=\"0\" max=\"180\" oninput=\"send(6,this.value)\" /> <p>Servo 6</p> <div> <br/> <input type = \"button\" value = \"FORWARD\" onClick=\"mMove(1)\"/> <input type = \"button\" value = \"REVERSE\" onClick=\"mMove(2)\"/> <input type = \"button\" value = \"LEFT\" onClick=\"mMove(3)\"/> <input type = \"button\" value = \"RIGHT\" onClick=\"mMove(4)\"/> </div> </body> </html>";



const char* ssid = "NodeMCU";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

ESP8266WebServer server(80);

void WLANinit()
{
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  while (!(WiFi.softAP(ssid, password)));
  while ( !WiFi.softAPgetStationNum()) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
 Serial.println("Server on");
}





void setup() {
  Serial.begin(74880);
  ESP.wdtDisable();
  WLANinit();
  mDriverInit(D2, D3, D4, D5, D6, D7);
}
String prevval="",newval="",prevservo="",newservo="",prevdir="",newdir="";



void loop()
{
  server.handleClient();
  if (server.args() > 0)
  {
    String msg = "";
    int servoargpos, valueargpos, dirargpos;
    msg += "POST Count: ";
    msg += server.args();
    msg += "\n";
    int i = 0;
    for (i = 0; i < server.args(); i++)
    {
      if(server.argName(i)=="SERVO") servoargpos = i;
      if(server.argName(i)=="value") valueargpos = i;
      if(server.argName(i)=="dir") dirargpos = i;
    }
    //Serial.println(msg);
    newservo=server.arg(servoargpos);
    newval = server.arg(valueargpos);
    if((server.argName(servoargpos)=="SERVO")&&(server.argName(valueargpos)=="value")&&((prevservo!=newservo)||(prevval!=newval))) servo(newval.toInt(),newservo.toInt());
    prevservo=newservo;
    prevval=newval;
    servoargpos=valueargpos=dirargpos=0;
  }
  mStop();
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void handle_OnConnect() {
  server.sendHeader("Content-Length",(String)webpage.length());
  server.send(200, "text/html", webpage);

  String servo, val;
  servo = server.arg("SERVO");
  val = server.arg("value");
  if ((servo != "") && (val != ""))Serial.println("SERVO" + servo + ":" + val);
  Serial.println("Connected");
}
void servo(int value, int whichServo)
{
  String message = "SERVO" + (String)whichServo + ":" + (String)value + "\n";
  Serial.print(message);
}

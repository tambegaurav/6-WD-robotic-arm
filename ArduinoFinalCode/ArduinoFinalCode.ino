#include <Servo.h>
String inString;

struct servomotors
{
  int pin;
  int value;
  Servo s;
  int type;
  void begin(int pinno)
  {
    s.attach(pinno);
    s.write(0);
  }
  void write(int val)
  {
    s.write(val);
  }
};

typedef struct servomotors Motors;
const int nummotors = 6;
Motors Motor[nummotors];

void setup()
{
  Serial.begin(74880);
  int i;
  for (i = 0; i < nummotors; i++)
  {
    Motor[i].begin(7 + i); //Assign pins 7 to (7+nummotors-1) for servo motors
  }
}

void loop() 
{
  int servoNum, servoValue;
  while(Serial.available()>0)
  {
    inString = Serial.readString();
  }
  if((inString.indexOf("SERVO")>-1)&&(inString.length()>7))
  {
    servoNum = (inString.charAt(5)-0x30);
    servoValue = (inString.substring(7).toInt());
    if((servoNum<=nummotors)&&(servoNum>0))
    {
      Motor[servoNum-1].write(servoValue);
      //Serial.print("True    ");
    }
    //Serial.println("Servonumber:"+(String)servoNum + "   Servovalue:"+(String)servoValue);
  }
  inString="";
}

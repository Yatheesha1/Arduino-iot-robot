#include <SPI.h>
#include <Ethernet.h>

int m1p1 = 2;
int m1p2 = 3;
int m2p1 = 4;
int m2p2 = 5;

int lm35pin = A2;
int pirpin = 7;
int ldrpin = 8;

const char *stat[]={"DAY TIME","NIGHT TIME"};
const char *motion[]={"NO OBJECT","OBJECT DETECTED"};

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//MAC address
IPAddress ip(192, 168, 137, 20);         // ip in lan (that's what you need to use in your browser. ("192.168.137.20")
//byte gateway[] = { 192, 168, 1, 1 };
//byte subnet[] = { 255, 255, 255, 0 };
EthernetServer server(80);               //server port

String readString;

void setup()
{

  Serial.begin(9600);
  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  pinMode(lm35pin, INPUT);
  pinMode(pirpin, INPUT);
  pinMode(ldrpin, INPUT);
  //Ethernet.begin(mac, ip, gateway, subnet);
  Ethernet.begin(mac, ip);
  server.begin();
  //Serial.print("server is at ");
  //Serial.println(Ethernet.localIP());
  digitalWrite(m1p1, LOW);
  digitalWrite(m1p2, LOW);
  digitalWrite(m2p1, LOW);
  digitalWrite(m2p2, LOW);

}


void loop()
{

  // Create a client connection
  EthernetClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      if (client.available())
      {
        int temp = analogRead(lm35pin);
        double lm35value = (5*17*temp*100/1024.0);         
        int ldrvalue = digitalRead(ldrpin);
        int pirvalue = digitalRead(pirpin);
        char c = client.read();
        //read char by char HTTP request
        if (readString.length() < 100)
        {
          //store characters to string
          readString += c;
        }

        //if HTTP request has ended
        if (c == '\n')
        {
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
          client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
          client.println("<TITLE>Ethernet Test Project</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<br />");
          client.println("<H2>Arduino with Ethernet Shield</H2>");

          client.println("<b>TEMPERATURE:</b>");
          client.println(lm35value,2);
          client.println("  'C");
          client.println("<br />");
        
          client.println("<b>OBJECT:</b>");
          client.println(motion[pirvalue]);
                 
          client.println("<br />");
          client.println("<b>TIME:</b>");
          client.println(stat[ldrvalue]);
          client.println("<br />");
          client.println("<br />");
          client.println("<a href=\"/?button1front\"\">FRONT</a>");
          client.println("<a href=\"/?button1back\"\">BACK</a><br />");
          client.println("<br />");
          client.println("<a href=\"/?button1right\"\">RIGHT</a>");
          client.println("<a href=\"/?button1left\"\">LEFT</a><br />");
          client.println("<br />");
          client.println("<a href=\"/?button1stop\"\">STOP</a><br />");
          client.println("</BODY>");
          client.println("<head>");
          client.println("<meta http-equiv='refresh' content='1'>");
          client.println("</head>");
          client.println("</HTML>");

          //delay(1);
          //stopping client
          client.stop();
          //controls the Arduino if you press the buttons
          if (readString.indexOf("?button1front") > 0 && pirvalue==0)
          {
            digitalWrite(m1p1, HIGH);
            digitalWrite(m1p2, LOW);
            digitalWrite(m2p1, HIGH);
            digitalWrite(m2p2, LOW);
            Serial.println("Front");
          }
          if (readString.indexOf("?button1back") > 0)
          {
            digitalWrite(m1p1, LOW);
            digitalWrite(m1p2, HIGH);
            digitalWrite(m2p1, LOW);
            digitalWrite(m2p2, HIGH);
            Serial.println("back");
          }

          if (readString.indexOf("?button1right") > 0)
          {
            digitalWrite(m1p1, HIGH);
            digitalWrite(m1p2, LOW);
            digitalWrite(m2p1, LOW);
            digitalWrite(m2p2, HIGH);
            Serial.println("Right");
          }

          if (readString.indexOf("?button1left") > 0)
          {
            digitalWrite(m1p1, LOW);
            digitalWrite(m1p2, HIGH);
            digitalWrite(m2p1, HIGH);
            digitalWrite(m2p2, LOW);
            Serial.println("Left");
          }

          if (readString.indexOf("?button1stop") > 0)
          {
            digitalWrite(m1p1, LOW);
            digitalWrite(m1p2, LOW);
            digitalWrite(m2p1, LOW);
            digitalWrite(m2p2, LOW);
            Serial.println("Stop");
          }
          //clearing string for next read
          Serial.println(readString);
          readString = "";
          if(pirvalue==1)
          {
           digitalWrite(m1p1, LOW);
           digitalWrite(m1p2, LOW);
           digitalWrite(m2p1, LOW);
           digitalWrite(m2p2, LOW);
           Serial.println("Stop");
          }
        }
      }
    }
  }
}


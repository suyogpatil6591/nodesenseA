#include <Arduino.h>
#include <DHT.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
 


const char* ssid = "SuyogP";  
const char* password ="say_my_name";


WiFiServer server(8888);


String header;


String alarmstate = "off";
String motorstate = "off";




 int Fire ;





#define DHTPIN 0          
 DHT dht(DHTPIN, DHT11);



unsigned long currentTime = millis();

unsigned long previousTime = 0; 

const long timeoutTime = 16000;

void setup() {
  Serial.begin(115200);
  
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D0,INPUT);
  
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
   dht.begin();

  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){




 float h = dht.readHumidity();
      float t = dht.readTemperature();
      
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                        
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.print("\n");
                             int temp=t;
                             int humi=h;
          
  
  
  delay(1000);

Fire= digitalRead(D0);
  
  
  if (Fire==1)
  {
   digitalWrite(D6,HIGH);
   
   Serial.println("fire");
   alarmstate="on";
   
  }
  
  
  delay(1000);
     



  WiFiClient client = server.available();   

  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();         
      if (client.available()) {            
        char c = client.read();            
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            
            





             if (header.indexOf("GET /6/on") >= 0) {
              Serial.println("alarm on");
              alarmstate = "on";
              digitalWrite(D6, HIGH);
            } else if (header.indexOf("GET /6/off") >= 0) {
              Serial.println("alarm off");
              alarmstate = "off";
              digitalWrite(D6, LOW);
            } else if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("motor on");
              motorstate = "on";
              digitalWrite(D5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("motor off");
              motorstate = "off";
              digitalWrite(D5, LOW);
            }
            
            
            client.println("<html>");
            client.println("<head>");
           
            
           
            client.println("</head>");
            
            
            client.println("<body style=\"background-color:powderblue;\"><center><h1>nodesense agro</h1></center>");
            client.println("<center><p><b>temperature:</b>");
            client.println(temp);
            client.println("</p><p><b>humidity:</b>");
            client.println(humi);
            client.println("</p></center><p><br><center><b>Alarm status: </b> " + alarmstate + "</p></center>");
                  
           if (alarmstate=="off") {
              client.println("<center><p><a href=\"/6/on\"><button class=\"button\">test</button></a></p>");
            } else {
              client.println("<p><a href=\"/6/off\"><button class=\"button button2\">turn OFF</button></a></p></center>");
            } 
               
               
            
            client.println("<center><p><b>Motor State:</b> " + motorstate + "</p>");
                 
             if (motorstate=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">turn ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">turn OFF</button></a></p></center>");
            }
             if (temp<=35){
               client.println("<center><p><b>temperature is normal</b></p></center>");
               }
               else{
                  client.println("<center><p><b>temperature is high turn on motor</b></p></center>");
               }

             if (humi<=60){
               client.println("<center><p><b>high humidity crop might not need water conside turning off motor</b></p></center>");
               }
               else{
                  client.println("<center><p><b>low humidity consider turning on motor</b></p></center>");
               } 
            
            client.println("</body></html>");
            
            
            client.println();
            
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
  
    header = "";
   
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}


 
 
 
 
 

 
 
 
 
 



#include <SoftwareSerial.h>

#define SSID "LAPTOP-GQVDF9MH 6726"
#define PASS "123456789#"

String apiKey = "EY6ENAPVHLEINCE6";

SoftwareSerial SoftSer(12, 11) ;

long startTime = 0 ;
long waitTime = 0 ;
long timeIntervalBetweenWrite = 1000 ;


void setup() {
  startTime = millis();
  Serial.begin(38400);
  
  SoftSer.begin(38400); 
}

void loop() {

  int data1 = 30;
  int data2 = 28;
  int data3 = 60;
  int data4 = 90;
  
waitTime = millis()-startTime; 

if(waitTime > timeIntervalBetweenWrite ) {
   postDataToThingSpeak(data1 , data2 , data3, data4);
   startTime = millis();
}


}


void postDataToThingSpeak(int data1 ,int data2 ,int data3 ,int data4 ){

  /*  String join="AT+CWJAP=\""; // Join accesspoint
  join+=SSID;
  join+="\",\"";
  join+=PASS;
  join+="\"";
  SoftSer.println(join);
  delay(5000);*/



  Serial.println(data1); 
  Serial.println(data2); 
  Serial.println(data3); 
  Serial.println(data4);

  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  SoftSer.println(cmd);

  if (SoftSer.find("Error")) 
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  
  // put together the get string command
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(data1 );
  getStr += "&field2=";
  getStr += String(data2 );
  getStr += "&field3=";
  getStr += String(data3 );
  getStr += "&field4=";
  getStr += String(data4);
  getStr += "\r\n\r\n";


 
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  SoftSer.println(cmd);

  if (SoftSer.find(">")) 
  {
    SoftSer.print(getStr); // Send data.
  }
  else 
  {
    SoftSer.println("AT+CIPCLOSE");
    
    Serial.println("AT+CIPCLOSE"); // If this shows on the serial monitor the data did not send.
  }
  // The AT+RST command resets the ESP8266, I'm doing this because data is uploaded much more reliably. 
  SoftSer.println("AT+RST"); 
  
  
  
}

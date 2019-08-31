// This code consists of my own work and others : James who developed temperature monitoring system code can be found on:
/*************************WIFI SETUP*************************************/
#include <SoftwareSerial.h>

// define the SSID address and passoword for wifi connections
#define SSID "LAPTOP-GQVDF9MH 6726"
#define PASS "123456789#"

// define the write key found on thingspeak IoT server
String apiKey = "EY6ENAPVHLEINCE6";

// define the pins used to communicate with the esp8266  module
SoftwareSerial Esp8266_Serial(12, 11) ;

// define time interval to upload to thingspeak in milliseconds
long startTime = 0 ;
long waitTime = 0 ;
long timeIntervalBetweenWrite = 5000 ; // for this application 5 seconds interval is choosen .

/**********************Senosor Pin Definitions***************************************/
String MQ3  ;                                   // This defines the analog PIN (A0) , which will be connected to MQ3 gas sensor
String MQ5  ;                                   // This defines the analog PIN A1 , which is connected to MQ5 gas sensor
String Temp ;                                   // This defines the analog PIN D4 , which is connected to temperature sensor
String Hum  ;                                   // This defines the analog PIN D4 , which is connected to humidity sensor
int MQ3_CO_PPM ;
int MQ5_LPG_PPM ;
int Temp_VALUE ;
int Hum_VALUE ;
String damageReading ;                          // used to read the first character in the serial which is just for marking 
                                                // start of information

/**********************RGB pin setup***************************************/
int redPin_LPG = 9;
int greenPin_LPG = 10;
int bluePin_LPG = 11;

int redPin_CO = 3;
int greenPin_CO = 5;
int bluePin_CO = 6;


/******************Program Setup *************************************************/
void setup() {
  
  startTime = millis();    // used to ensure that data is posted in interval of 5 seconds or more
  Serial.begin(38400);
  
  Esp8266_Serial.begin(38400);

  // if using the module for the first time uncomment this otherwise the esp8266 module will remeber the previous connection .
    //  while(1){  // iterate until the wifi is connected 
    //  connectEsp8266WIFI();
    //  if(Esp8266_Serial.find("WIFI CONNECTED\r\n")==1){
    //     break ;
    //   }
    //  }
  
  Serial.println("CLEARSHEET"); // clears EXCEL sheet starting at row 1
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Date,Time,LPG,CO,TEMP,HUM"); // labels on excel spreadsheet

}

void loop() {
   
 if( Serial.available() > 0 ){

          //Read the values in the serial buffer
          damageReading = Serial.readStringUntil( '<');                 
          MQ3 = Serial.readStringUntil(',');
          MQ5 = Serial.readStringUntil(',');
          Temp = Serial.readStringUntil(',');
          Hum = Serial.readStringUntil('>');
                  
          //convert the strings to integers
          MQ3_CO_PPM   = MQ3.toInt();
          MQ5_LPG_PPM  = MQ5.toInt();
          Temp_VALUE   = Temp.toInt();
          Hum_VALUE    =  Hum.toInt();
         
          setAppropriateRGBColours() ;
       
          // write the values to excelyufxjgxy
          Serial.println( (String) "DATA,DATE,TIME," +MQ3_CO_PPM + "," + MQ5_LPG_PPM  +  "," + Temp_VALUE + "," + Hum_VALUE +  ",AUTOSCROLL_20" );

   
           waitTime = millis()-startTime; 
           // post the values to thingspeak IoT web server
           if(waitTime > timeIntervalBetweenWrite ) {
                 postDataToThingSpeak(MQ3_CO_PPM  ,  MQ5_LPG_PPM  , Temp_VALUE, Hum_VALUE );
                 startTime = millis();
             }
          

     }

}

void setAppropriateRGBColours(){
  
   //Threshold processes for air quality color system 
        if( MQ3_CO_PPM > 100){
  
            setColor(0, 255, 255);  //red, Dangerous air quality 
 
        }else if(MQ3_CO_PPM< 100  &&  MQ3_CO_PPM> 70 ){
    
            setColor(5,215, 255);  //yellow, Moderate air quality
   
        }else if(MQ3_CO_PPM< 70  &&  MQ3_CO_PPM> 40){
    
            setColor(0, 0, 255);  //yellow, Moderate air quality
   
        }else{
      
            setColor(255,0,255);  //green, Good air quality
        }

             

        if( MQ5_LPG_PPM > 400){
  
            setColor2(0, 255, 255);  //red, Dangerous air quality 
 
         }else if(MQ5_LPG_PPM < 400  &&  MQ5_LPG_PPM > 300){
    
            setColor2(5,215,255);  //ORANGE, Moderate air quality
   
         }else if(MQ5_LPG_PPM < 300  &&  MQ5_LPG_PPM > 200){
    
            setColor2(0, 0, 255);  //yellow, Moderate air quality
   
         }else{
      
             setColor2(255,0,255);//green, Good air quality
        
         }
  
  }



void setColor(int red, int green, int blue)
{
  analogWrite(redPin_LPG, red);
  analogWrite(greenPin_LPG, green);
  analogWrite(bluePin_LPG, blue);  
}

 void setColor2(int red, int green, int blue)
{
  analogWrite(redPin_CO, red);
  analogWrite(greenPin_CO, green);
  analogWrite(bluePin_CO, blue);  
}  


void postDataToThingSpeak(int data1 ,int data2 ,int data3 ,int data4 ){

  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  Esp8266_Serial.println(cmd);

  if (Esp8266_Serial.find("Error")) 
  {
    return;
  }
  
  
  // put together the GET string command
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


  // send the length of the data
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  Esp8266_Serial.println(cmd);

  if (Esp8266_Serial.find(">")) 
  {
    Esp8266_Serial.print(getStr); // Send data.
  }
  else 
  {
    Reset() ;
    Esp8266_Serial.println("AT+CIPCLOSE");
    
    Serial.println("AT+CIPCLOSE"); // If this shows on the serial monitor the data did not send.
  }
  // Resert the esp8266 for reliable data transfer. 
  Esp8266_Serial.println("AT+RST"); 
  
 
  
}

void connectEsp8266WIFI(){
    String join="AT+CWJAP=\""; // Join accesspoint
  join+=SSID;
  join+="\",\"";
  join+=PASS;
  join+="\"";
  Esp8266_Serial.println(join);
  delay(5000);
  
}

void Reset(){
  startTime = millis();
  
  Esp8266_Serial.begin(38400);
}

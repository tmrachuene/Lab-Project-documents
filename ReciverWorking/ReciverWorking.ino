
/*************************WIFI SETUP*************************************/
#include <SoftwareSerial.h>

#define SSID "LAPTOP-GQVDF9MH 6726"
#define PASS "123456789#"

String apiKey = "EY6ENAPVHLEINCE6";

SoftwareSerial SoftSer(12, 11) ;

long startTime = 0 ;
long waitTime = 0 ;
long timeIntervalBetweenWrite = 5000 ;

/**********************Senosor Pin Definitions***************************************/
String MQ3  ;                                   // This defines the analog PIN (A0) , which will be connected to MQ3 gas sensor
String MQ5  ;                                   // This defines the analog PIN A1 , which is connected to MQ5 gas sensor
String Temp ;                                   // This defines the analog PIN D4 , which is connected to temperature sensor
String Hum  ;                                   // This defines the analog PIN D4 , which is connected to humidity sensor
int MQ3_ADC_VALUE;
int MQ5_ADC_VALUE ;
int Temp_ADC_VALUE ;
int Hum_ADC_VALUE ;
String damageReading ;

/**********************RGB pin setup***************************************/
int redPin_LPG = 9;
int greenPin_LPG = 10;
int bluePin_LPG = 11;

int redPin_CO = 3;
int greenPin_CO = 5;
int bluePin_CO = 6;

/******************Program Setup *************************************************/
void setup() {
  
  startTime = millis();
  Serial.begin(38400);
  
  SoftSer.begin(38400);
  
  Serial.println("CLEARSHEET"); // clears EXCEL sheet starting at row 1
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Date,Time,LPG,CO,TEMP,HUM");

}

void loop() {
  

   //Serial.println( (String) "DATA,DATE,TIME," + findPPM_LPG_MQ5() + "," + findPPM_CO_MQ3() + ",AUTOSCROLL_20" );
   
         if( Serial.available() > 0 ){
            
            damageReading = Serial.readStringUntil( '<');                 
             MQ3 = Serial.readStringUntil(',');
             MQ5 = Serial.readStringUntil(',');
            Temp = Serial.readStringUntil(',');
            Hum = Serial.readStringUntil('>');
                  
             MQ3_ADC_VALUE = MQ3.toInt();
             MQ5_ADC_VALUE = MQ5.toInt();
             Temp_ADC_VALUE = Temp.toInt();
             Hum_ADC_VALUE =  Hum.toInt();

          waitTime = millis()-startTime; 

          if(waitTime > timeIntervalBetweenWrite ) {
             postDataToThingSpeak(MQ3_ADC_VALUE , MQ5_ADC_VALUE  , Temp_ADC_VALUE, Hum_ADC_VALUE );
             startTime = millis();
         }

//Threshold processes for air quality color system 
if( MQ3_ADC_VALUE> 400){
  
  setColor(0, 255, 255);  //red, Dangerous air quality 
 
  }else if(MQ3_ADC_VALUE< 400  &&  MQ3_ADC_VALUE> 250 ){
    
   setColor(5,215, 255);  //ORANGE, Moderate air quality
   
    }else if(MQ3_ADC_VALUE< 250  &&  MQ3_ADC_VALUE> 100){
    
   setColor(0, 0, 255);  //yellow, Moderate air quality
   
  }else{
      
        setColor(255,0,255);  //green, Good air quality
      }

             

  if( MQ5_ADC_VALUE> 400){
  
  setColor2(0, 255, 255);  //red, Dangerous air quality 
 
  }else if(MQ5_ADC_VALUE< 400  &&  MQ5_ADC_VALUE> 300){
    
   setColor2(5,215,255);  //ORANGE, Moderate air quality
   
  }else if(MQ5_ADC_VALUE< 300  &&  MQ5_ADC_VALUE> 200){
    
   setColor2(0, 0, 255);  //yellow, Moderate air quality
   
  }else{
      
        setColor2(255,0,255);//green, Good air quality
        
      }

}
   Serial.println( (String) "DATA,DATE,TIME," + MQ3_ADC_VALUE+ "," + MQ5_ADC_VALUE +  "," + Temp_ADC_VALUE + "," + Hum_ADC_VALUE +  ",AUTOSCROLL_20" );

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

  /*  String join="AT+CWJAP=\""; // Join accesspoint
  join+=SSID;
  join+="\",\"";
  join+=PASS;
  join+="\"";
  SoftSer.println(join);
  delay(5000);*/


  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  SoftSer.println(cmd);

  if (SoftSer.find("Error")) 
  {
    //Serial.println("AT+CIPSTART error");
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
    Reset() ;
    SoftSer.println("AT+CIPCLOSE");
    
    Serial.println("AT+CIPCLOSE"); // If this shows on the serial monitor the data did not send.
  }
  // The AT+RST command resets the ESP8266, I'm doing this because data is uploaded much more reliably. 
  SoftSer.println("AT+RST"); 
  
  
  
}

void Reset(){
  startTime = millis();
  
  SoftSer.begin(38400);
}

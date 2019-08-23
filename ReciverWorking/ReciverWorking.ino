#include <SoftwareSerial.h> 

#include <Wire.h>
int ledPin = 13; 

// ThingSpeak Write API key, this can be found in your channel under API Keys and you'll want the "Write API Key". 
String apiKey = "EY6ENAPVHLEINCE6";


#define SSID "con" 
#define PASS "karabeloma0"

// connect pin 12 to TX of the ESP8266 (Yellow wire if you've been following the videos.)
// connect pin 11 to RX of the ESP8266 (Blue wire if you've been following the videos.)
SoftwareSerial SoftSer(12, 11); // Everywhere from here on when you see SoftSer, it is being used as a command
// in the same way you'd use Serial.print() or Serial.begin() only over pins 12 and 11 instead of the USB port.
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

    // Setting pin 13, the LED indicator, to output.
  pinMode(ledPin, OUTPUT);
  
// Setup code here, to run once:
  Serial.begin(38400) ;
    // Begins software serial communication at a baud of 9600.
  SoftSer.begin(38400); 
  
  
  Serial.println("CLEARSHEET"); // clears EXCEL sheet starting at row 1
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Date,Time,LPG,CO,TEMP,HUM");

}

void loop() {

 /* String join="AT+CWJAP=\""; // Join accesspoint
  join+=SSID;
  join+="\",\"";
  join+=PASS;
  join+="\"";
  SoftSer.println(join);
  delay(5000);*/
  //-----------------------------------------------*


  // blink LED on board 
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
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
  

            float temp = MQ3_ADC_VALUE;
            float temp2 = MQ5_ADC_VALUE;
            float temp3 = Temp_ADC_VALUE;
            float temp4 = Hum_ADC_VALUE;



        

          // convert float to string-------------*
  char buf[16]; // buf array can hold up to 16 characters. 
  String strTemp = dtostrf(temp, 4, 1, buf); // "buf" will be the actual name of the array and strTemp is being equalled to it as a string.
    // convert float to string-------------*
  char buf1[16]; // buf array can hold up to 16 characters. 
  String strTemp2 = dtostrf(temp2, 4, 1, buf1); // "buf" will be the actual name of the array and strTemp is being equalled to it as a string.
    // convert float to string-------------*
  char buf2[16]; // buf array can hold up to 16 characters. 
  String strTemp3 = dtostrf(temp3, 4, 1, buf2); // "buf" will be the actual name of the array and strTemp is being equalled to it as a string.
    // convert float to string-------------*
  char buf3[16]; // buf array can hold up to 16 characters. 
  String strTemp4 = dtostrf(temp4, 4, 1, buf3); // "buf" will be the actual name of the array and strTemp is being equalled to it as a string.


  Serial.println(strTemp); // Prints the recorded temperature to the serial print.
  Serial.println(strTemp2); // Prints the recorded temperature to the serial print.
  Serial.println(strTemp3); // Prints the recorded temperature to the serial print.
  Serial.println(strTemp4); // Prints the recorded temperature to the serial print.

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

  // Putting together the GET string, this command structure can be found on your
  // ThingSpeak channel in Data Import/Export, under "Update Channel Feed - GET". 
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(strTemp);
  getStr += "&field2=";
  getStr += String(strTemp2);
  getStr += "&field3=";
  getStr += String(strTemp3);
  getStr += "&field4=";
  getStr += String(strTemp4);
  getStr += "\r\n\r\n";

  // send data length
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
  // ThingSpeak needs on average, a 15 second delay between updates. Change this to whatever suits your application
  // e.g: delay(600000) is ten minutes.
 
  

//Threshold processes for air quality color system 
if( MQ3_ADC_VALUE> 100){
  
  setColor(0, 255, 255);  //red, Dangerous air quality 
 
  }else if(MQ3_ADC_VALUE< 100  &&  MQ3_ADC_VALUE> 70 ){
    
   setColor(5,215, 255);  //yellow, Moderate air quality
   
    }else if(MQ3_ADC_VALUE< 70  &&  MQ3_ADC_VALUE> 40){
    
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


 delay(1000);
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

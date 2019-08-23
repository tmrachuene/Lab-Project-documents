         /**********************Calibration parameters***************************************/
#define Ro_Clean_Air_Factor_MQ3               (60)  // Is the ratio of sensor resistance in clean air and Ro
                                                    // It is found from the datasheet
#define Ro_Clean_Air_Factor_MQ5               (6.5)
 
#define Calibration_Sample                    (15) // The number of times the value of the sensor will be read during calibration
#define Calibration_Interval                  (100)// The time interval between each sample during calibration in miliseconds

/*********************Load resistance of the sonsors*******************************/
#define MQ3_RL                                (1.5) // The load resistance of MQ3 from the mq3 datasheet in kohms
#define MQ5_RL                                (1.5) // The load resistance of MQ5 fronm its datasheet in kohms

/*******************Reading parameters during normal operation ********************/
#define Read_Sample                           (10) // The number of time the sensor read during normal operation
#define Read_Interval                         (40) // Time interval in between the reading in milliseconds

/******************The Ro for LPG(MQ5) and Smoke(MQ3) *****************************/
float Ro_LPG                                  (2.42) ;// initialise to 9
float Ro_CO                                   (0.57) ;// initialise to 9

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
  
// Setup code here, to run once:
  Serial.begin(38400) ;
  
  
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

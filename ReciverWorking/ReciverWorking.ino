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
  
//Calibrate the sensors to obtain Ro
  Ro_LPG = sensorCalibration(MQ5_RL ,Ro_Clean_Air_Factor_MQ5 ,"MQ5");
  Ro_CO = sensorCalibration(MQ3_RL , Ro_Clean_Air_Factor_MQ3 ,"MQ3");
  
  Serial.println("CLEARSHEET"); // clears EXCEL sheet starting at row 1
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Date,Time,LPG,CO,TEMP,HUM");

  Serial.print("Calibration completed") ;
  Serial.print('\n');
  Serial.print("Ro_LPG : ") ;
  Serial.print('\n');
  Serial.print(Ro_LPG ) ;
  Serial.print("\n") ;
  Serial.print("Ro_SMOKE : ") ;
  Serial.print(Ro_CO) ;
  Serial.print("\n");

}

void loop() {
  
   Serial.println( (String) "DATA,DATE,TIME," + findPPM_LPG_MQ5() + "," + findPPM_CO_MQ3() +  "," + Temp_ADC_VALUE + "," + Hum_ADC_VALUE +  ",AUTOSCROLL_20" );
   //Serial.println( (String) "DATA,DATE,TIME," + findPPM_LPG_MQ5() + "," + findPPM_CO_MQ3() + ",AUTOSCROLL_20" );
   /*Serial.print("LPG : "); 
   Serial.print(findPPM_LPG_MQ5( ));
   Serial.print( "ppm" );
   Serial.print(",");  
   Serial.print(MQ5_ADC_VALUE);
   Serial.print(","); 
Serial.print("SMOKE : ");
 Serial.print(findPPM_CO_MQ3());
   Serial.print( "ppm" );
   Serial.print(",");*/
//   Serial.print(MQ3_ADC_VALUE); 
  /* Serial.print(Temp_ADC_VALUE); 
   Serial.print(",");
   Serial.print(Hum_ADC_VALUE); */
  // Serial.print("\n");

}

// This function calculates the sensor response(Rs) using given ADC value and Load resistance.
float  findSensorResistance(int ADC_VALUE , int RL ){
     return((float) (RL*((1023 - ADC_VALUE)/ADC_VALUE ) )) ;
  }


//This function  returns the Ro(The resistence of the sensor for clean air) of the sensor
float  sensorCalibration( float RL ,float Ro_Clean_Air_Factor  , String sensor ){
     float Rs_MQ3 = 0 ;
     float Rs_MQ5 = 0 ;
  
     int index ;
     
     for(index =0 ; index < Calibration_Sample ; index++ ){

         if( Serial.available() > 0 ){

            //Read string from transmitter and cast to integers.
            damageReading = Serial.readStringUntil( '<');                 
             MQ3 = Serial.readStringUntil(',');
             MQ5 = Serial.readStringUntil( '>');

             MQ3_ADC_VALUE = MQ3.toInt();
             MQ5_ADC_VALUE = MQ5.toInt();

             //Sum the calibration samples recived
             if(sensor == "MQ3"){
             Rs_MQ3 +=  findSensorResistance( MQ3_ADC_VALUE , RL  ) ;
             }else{
             Rs_MQ5 +=  findSensorResistance( MQ5_ADC_VALUE , RL  ) ;
             }
           
         }else{   }
        
          delay( Calibration_Interval) ;
      }

//Find average of calibration samples and divide by  clean air quality factor depending of specific sensor
     if(sensor == "MQ3"){
           Rs_MQ3 = Rs_MQ3 /Calibration_Sample ;
 
           Rs_MQ3 = Rs_MQ3/Ro_Clean_Air_Factor  ;
           return Rs_MQ3 ;
      }else{
           Rs_MQ5 = Rs_MQ5 /Calibration_Sample ;
 
           Rs_MQ5 = Rs_MQ5/Ro_Clean_Air_Factor  ;
           
         return Rs_MQ5; // this value represent Rs in clean air which is Ro 
        
        } 
                
  }


//This function returns the adc value of sensor reading from analog pins
float readSensorOutput(  int RL  , String sensor){
    float Rs_MQ3= 0;
    float Rs_MQ5 =0;
    
    for(int index =0 ; index < Read_Sample  ; index++ ){
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
if( MQ3_ADC_VALUE> 131){
  
  setColor(0, 255, 255);  //red, Dangerous air quality 
 
  }else if(MQ3_ADC_VALUE< 131  &&  MQ3_ADC_VALUE> 107 ){
    
   setColor(5,215, 255);  //yellow, Moderate air quality
   
    }else if(MQ3_ADC_VALUE< 107  &&  MQ3_ADC_VALUE> 87){
    
   setColor2(0, 0, 255);  //yellow, Moderate air quality
   
  }else{
      
        setColor(255, 0, 255);  //green, Good air quality
      }

             

  if( MQ5_ADC_VALUE> 594){
  
  setColor2(0, 255, 255);  //red, Dangerous air quality 
 
  }else if(MQ5_ADC_VALUE< 594  &&  MQ5_ADC_VALUE> 495){
    
   setColor2(5,215,255);  //ORANGE, Moderate air quality
   
  }else if(MQ5_ADC_VALUE< 495  &&  MQ5_ADC_VALUE> 395){
    
   setColor2(0, 0, 255);  //yellow, Moderate air quality
   
  }else{
      
        setColor2(255,0,255);//green, Good air quality
        
      }


      //Process value depending on the sensor value recived 
             if(sensor == "MQ3"){
             Rs_MQ3 +=  findSensorResistance( MQ3_ADC_VALUE , RL  ) ;
      
             }else{
             Rs_MQ5 +=  findSensorResistance( MQ5_ADC_VALUE , RL  ) ;

             }
           
         }else{   }
        
       delay(Read_Interval) ;
    }


      if(sensor == "MQ3"){
           Rs_MQ3 = Rs_MQ3 /Read_Sample ;
         
           return Rs_MQ3 ;
      }else{
           Rs_MQ5 = Rs_MQ5 /Read_Sample ;

           
         return Rs_MQ5 ;
        
        }

  }


  // This function calculate the PPM of  LPG gas given the Rs/Ro  
 int findPPM_LPG_MQ5( ){
 
  float ppm ;
  float PPM_In_Log ;
  float RsRoRatio ;
  
  RsRoRatio = readSensorOutput(  MQ5_RL ,"MQ5") / Ro_LPG ; 
  //Serial.println("Ratiolpg"); 
  //Serial.println(RsRoRatio); 
  float log_RsRoRatio = log10(RsRoRatio) ;
  PPM_In_Log = (log_RsRoRatio *( - 2.5445 ))  + 1.879;  // The equation is found by doing regression of sensitity graph of MQ5 for LPG 
  ppm = pow(10, PPM_In_Log  );
  
  return ppm ;    
}

// This function calculate the PPM of  Smoke   given the Rs/Ro  
int findPPM_CO_MQ3(){
  float ppm;
  float RsRoRatio;
  float PPM_In_Log;

  RsRoRatio = readSensorOutput( MQ3_RL , "MQ3")/ Ro_CO;
   // Serial.println("RatioCO"); 
 // Serial.println(RsRoRatio);
  float log_RsRoRatio = log10(RsRoRatio) ;
  PPM_In_Log  = (log_RsRoRatio*( - 3.9651) )  + 8.6491 ;  // The equation is found by doing regression of sensitity graph of MQ3 for SMOKE(CO) 
  ppm = pow(10, PPM_In_Log  )  ;  
 
  return ppm ;  
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

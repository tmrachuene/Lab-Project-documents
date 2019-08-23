
#include <dht11.h>
#define DHT11PIN 4

/**********************Senosor Pin Definitions***************************************/
#define MQ3_PIN                               (0) // This define the analog PIN (A0) , which will be connected to MQ3 gas sensor
#define MQ5_PIN                               (1) // This define the analog PIN A1 , which is connected to MQ5 gas sensor

/**********************Calibration parameters***************************************/
#define Ro_Clean_Air_Factor_MQ3               (60)  // Is the ratio of sensor resistance in clean air and Ro
                                                    // It is found from the datasheet
#define Ro_Clean_Air_Factor_MQ5               (6.5)
 
#define Calibration_Sample                    (3) // The number of times the value of the sensor will be read during calibration
#define Calibration_Interval                  (500) // The time interval between each sample during calibration in miliseconds

/*********************Load resistance of the sonsors*******************************/
#define MQ3_RL                                (1.5) // The load resistance of MQ3 from the mq3 datasheet in kohms
#define MQ5_RL                                (1.5) // The load resistance of MQ5 fronm its datasheet in kohms

/*******************Reading parameters during normal operation ********************/
#define Read_Sample                           (25) // The number of time the sensor read during normal operation
#define Read_Interval                         (10) // Time interval in between the reading in milliseconds

/******************The Ro for LPG(MQ5) and Smoke(MQ3) *****************************/
float Ro_LPG                                  (2.42) ;// initialise to 9
float Ro_Smoke                                (0.57) ; // initialise to 9                                  (0.57) ;// initialise to 9

/*******************RGB parameters**********************************************/
int RED_PIN_CO = 10;
int GREEN_PIN_CO = 9;

/*******************RGB parameters**********************************************/
int RED_PIN_LPG =   5 ;
int GREEN_PIN_LPG = 6 ;
 

int TEM_VALUE       =  0 ;
int HUMIDITY_VALUE  =  0 ;

dht11 DHT11;

bool flag=false;

int  MQ5_ppm = 0 ;
int  MQ3_ppm = 0 ;

void setup() {

  // Default communication rate of the Bluetooth module
  Serial.begin(38400);

  //Calibrate the sensors to obtain Ro
  Ro_LPG = sensorCalibration(MQ5_PIN ,  MQ5_RL ,Ro_Clean_Air_Factor_MQ5  );
  Ro_Smoke = sensorCalibration( MQ3_PIN ,  MQ3_RL , Ro_Clean_Air_Factor_MQ3  ) ;


  Serial.print("Calibration completed") ;
  Serial.print('\n');
  Serial.print("Ro_LPG : ") ;
  Serial.print('\n');
  Serial.print(Ro_LPG ) ;
  Serial.print("\n") ;
  Serial.print("Ro_SMOKE : ") ;
  Serial.print(Ro_Smoke) ;
  Serial.print("\n");
  
  pinMode(RED_PIN_CO, OUTPUT);
  pinMode(GREEN_PIN_CO , OUTPUT);
  pinMode(RED_PIN_LPG , OUTPUT);
  pinMode(GREEN_PIN_LPG , OUTPUT);
}

void loop() {
  
 /*############################################################################*/

   int chk = DHT11.read(DHT11PIN);
 
   TEM_VALUE       =  DHT11.humidity ;
   HUMIDITY_VALUE  =  DHT11.temperature ;

   MQ5_ppm = findPPM_LPG_MQ5( ) ;
   MQ3_ppm = findPPM_SMOKE_MQ3();

/*############################################################################*/
// write high to digital pin 10  when smoke level goes above 60(750ppm)
 if(MQ3_ppm  > 100 ){
    RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,0 , 255 ) ;
    
  
  }else if (MQ3_ppm  >70 && MQ3_ppm  < 100){
     RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,5 , 215 ) ; 
    
  }else if (MQ3_ppm  >40 && MQ3_ppm  < 70){
     RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,0 , 0 ) ; 
    
  }else{
     RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,255 , 0 )  ;
    }

/*############################################################################*/
// write high to digital pin 12  when LPG level goes above 250(50ppm)
 if(MQ5_ppm  > 400 ){
  RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG  ,0 , 255 )  ;
  
  }else if( MQ5_ppm >300 && MQ5_ppm < 400 ){
    RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG ,5 , 215 ) ;
    
  }else if( MQ5_ppm >200 && MQ5_ppm < 300 ){
    RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG ,0 , 0 ) ;
    
  }else{
    RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG ,255 , 0 ) ;
    
  }
  
/*############################################################################*/
 String str1 = String(MQ3_ppm);
 String str2 = String(MQ5_ppm);

 String str3 = String(TEM_VALUE );
 String str4 = String(HUMIDITY_VALUE);
 
 String str5= '<'+str1+','+str2 +','+ str3+','+str4+'>';
 

// Length (with one extra character for the null terminator)
int str_len = str5.length()+1; 
char char_array[str_len];
str5.toCharArray(char_array, str_len); 

for(int i=0;i<str_len;i++){
 
 Serial.print(str5[i]);
 //Serial.write(str4[i]);
  //  delay(5);
  } 
}


/*################################################################################################## */
// This function calculates the sensor response(Rs) using given ADC value and Load resistance.
float  findSensorResistance(int ADC_VALUE , int RL ){
     return((float) (RL*((1023 - ADC_VALUE)/ADC_VALUE ) )) ;
  }

/*################################################################################################## */
//This function  returns the Ro(The resistence of the sensor for clean air) of the sensor
float  sensorCalibration(int MQ_PIN , float RL , float Ro_Clean_Air_Factor ){
     float Rs = 0 ;
     int index ;
     
     for(index =0 ; index < Calibration_Sample ; index++ ){
        
        Rs +=  findSensorResistance(analogRead( MQ_PIN) , RL ) ;
        
        delay( Calibration_Interval) ;
      }
  
     Rs = Rs /Calibration_Sample ;
 
     Rs = Rs/Ro_Clean_Air_Factor  ;
     
     return Rs  ;             // this value represent Rs in clean air which is Ro 
  }

/*################################################################################################## */
// This function returns the adc value of sensor reading from analog pins
float readSensorOutput( int MQ_PIN , int RL ){
    float Rs;
    
    for(int index =0 ; index < Read_Sample  ; index++ ){
       Rs +=   findSensorResistance( analogRead( MQ_PIN) , RL ) ;
       delay(Read_Interval) ;
    }

    Rs = Rs/Read_Sample ;
    return Rs ;                // return the average sensor rsistance or response given the ADC value

  }


/*################################################################################################## */
  // This function calculate the PPM of  LPG gas given the Rs/Ro  
 int findPPM_LPG_MQ5( ){

  float RsRoRatio ;
  RsRoRatio = readSensorOutput(MQ5_PIN ,  MQ5_RL) / Ro_LPG ;

  float PPM_In_Log ;
  float log_RsRoRatio = log10(RsRoRatio) ;
  PPM_In_Log = (log_RsRoRatio *( - 2.5445 ))  + 1.879;  // The equation is found by doing regression of sensitity graph of MQ5 for LPG 
                                                        // Mq5 and Excel are used to do the regression
                                                  
  float ppm ;
  ppm = pow(10, PPM_In_Log  )  ;
  // convert the log output to the normal value

  return ppm ;
    
}


/*################################################################################################## */
// This function calculate the PPM of  Smoke   given the Rs/Ro  
int findPPM_SMOKE_MQ3(){

  float RsRoRatio ;
  RsRoRatio = readSensorOutput(MQ3_PIN , MQ3_RL) / Ro_Smoke ;
  
  float PPM_In_Log  ;
  float log_RsRoRatio = log10(RsRoRatio) ;
 
  PPM_In_Log  = (log_RsRoRatio*( - 3.9651) )  + 8.6491 ;  // The equation is found by doing regression of sensitity graph of MQ3 for SMOKE(CO) 
                                                          // Mq3 and Excel are used to do the regression
  float ppm  ;
  ppm = pow(10, PPM_In_Log  )  ;                          // convert the log output to the normal value
  
  return ppm ;  
}

/*################################################################################################## */
// Is used to right PWM values to RGB pins
void RGB_Color(int Red_pin , int Green_pin ,int red_light_value, int green_light_value)
 {
  analogWrite(Red_pin, red_light_value);
  analogWrite(Green_pin, green_light_value);
}

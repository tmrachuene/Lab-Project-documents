
#include <dht11.h>
#define DHT11PIN 4

/*********************Load resistance of the sonsors*******************************/
#define MQ3_RL                                (1.5) // The load resistance of MQ3 from the mq3 datasheet in kohms
#define MQ5_RL                                (1.5) // The load resistance of MQ5 fronm its datasheet in kohms

/*******************RGB parameters**********************************************/
int RED_PIN_CO = 10;
int GREEN_PIN_CO = 9;

/*******************RGB parameters**********************************************/
int RED_PIN_LPG =   5 ;
int GREEN_PIN_LPG = 6 ;

/*******************Sensor pins declaration ************************************/
int CO_SENSOR_PIN  = A0; 
int LPG_SENSOR_PIN = A1;   

int CO_ANALOG_VALUE   = 0;
int LPG_ANALOG_VALUE  = 0;

int TEM_VALUE       =  0 ;
int HUMIDITY_VALUE  =  0 ;

dht11 DHT11;



bool flag=false;
void setup() {

  // Default communication rate of the Bluetooth module
  Serial.begin(38400);
  
  pinMode(RED_PIN_CO, OUTPUT);
  pinMode(GREEN_PIN_CO , OUTPUT);
  pinMode(RED_PIN_LPG , OUTPUT);
  pinMode(GREEN_PIN_LPG , OUTPUT);
}

void loop() {
  
 CO_ANALOG_VALUE  = analogRead(CO_SENSOR_PIN);
 LPG_ANALOG_VALUE = analogRead(LPG_SENSOR_PIN);

 int chk = DHT11.read(DHT11PIN);
 
 TEM_VALUE       =  DHT11.humidity ;
 HUMIDITY_VALUE  =  DHT11.temperature ;

 
// write high to digital pin 10  when smoke level goes above 60(750ppm)
 if(CO_ANALOG_VALUE  > 131 ){
    RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,0 , 255 ) ;
    
  
  }else if (CO_ANALOG_VALUE  >107 && CO_ANALOG_VALUE  < 131){
     RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,5 , 215 ) ; 
    
  }else if (CO_ANALOG_VALUE  >87 && CO_ANALOG_VALUE  < 107){
     RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,0 , 0 ) ; 
    
  }else{
     RGB_Color(RED_PIN_CO ,GREEN_PIN_CO ,255 , 0 )  ;
    }


// write high to digital pin 12  when LPG level goes above 250(50ppm)
 if(LPG_ANALOG_VALUE  > 594 ){
  RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG  ,0 , 255 )  ;
  
  }else if( LPG_ANALOG_VALUE >495 && LPG_ANALOG_VALUE < 594 ){
    RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG ,5 , 215 ) ;
    
  }else if( LPG_ANALOG_VALUE >395 && LPG_ANALOG_VALUE < 494 ){
    RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG ,0 , 0 ) ;
    
  }else{
    RGB_Color(RED_PIN_LPG ,GREEN_PIN_LPG ,255 , 0 ) ;
    
  }
  

 String str1 = String(CO_ANALOG_VALUE);
 String str2 = String(LPG_ANALOG_VALUE);

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

// This function calculates the sensor response(Rs) using given ADC value and Load resistance.
float  findSensorResistance(int ADC_VALUE , int RL ){
     return((float) (RL*((1023 - ADC_VALUE)/ADC_VALUE ) )) ;
  }

// Is used to right PWM values to RGB pins
void RGB_Color(int Red_pin , int Green_pin ,int red_light_value, int green_light_value)
 {
  analogWrite(Red_pin, red_light_value);
  analogWrite(Green_pin, green_light_value);
}

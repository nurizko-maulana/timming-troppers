#include<Servo.h>
#include <Wire.h>
#include<ESP8266WiFi.h>
#include<BlynkSimpleEsp8266.h>
#include <Adafruit_MLX90614.h>
#include <TM1637Display.h>

#define CLK 12
#define DIO 13

// Create a display object of type TM1637Display
TM1637Display display = TM1637Display(CLK, DIO);

// Create an array that turns all segments ON
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};

// Create an array that turns all segments OFF
const uint8_t allOFF[] = {0x00, 0x00, 0x00, 0x00};

// Create degree celsius symbol
const uint8_t celsius[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Degree symbol
  SEG_A | SEG_D | SEG_E | SEG_F   // C
};
 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


char ssid[]="dlink";
char pass[]="pinkgarden";
char auth[]="un7PHqoKMPMZvjM4IjSbOHZf9stsDPy5";

const int buzzer = 0 ;
const int button = 15 ;
int buttonState = 0; 



void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  Blynk.begin(auth,ssid,pass);
  mlx.begin(); 
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);
}

// Build in LED
BLYNK_WRITE(V5) 
{
  
  int pinValue = param.asInt();
  if(pinValue == 1){
    digitalWrite(2, LOW);
    Serial.println("LED ON");
    buzz();
    readTemp(); 
  }
  else{
    digitalWrite(2, HIGH);
    Serial.println("LED OFF");
  }
}

// Slider
BLYNK_WRITE(V8) 
{
  int pinValue = param.asInt();
  Serial.println("Servo Value = " + pinValue);

}

// Terminal Print
BLYNK_WRITE(V10){
  Serial.println(param.asStr());
}

//Proximity Sensor
BLYNK_WRITE(V11){
  String pinValue = param.asStr();
  Serial.println("Proximity Sensore = " + pinValue);
}

// Accelometer
BLYNK_WRITE(V12){
  String pinValue = param.asStr();
  Serial.println("Accelometer Sensore = " + pinValue);
}

void readTemp(){
  //print temp checking
  Serial.print("Ambient = "); 
  Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); 
  Serial.print(mlx.readObjectTempC()); 
  Serial.println("*C");

  // sned temp check data to blynk server
  Blynk.virtualWrite(V1,mlx.readObjectTempC());

  // pirnt temp check to 4 segment display
  printLED(mlx.readObjectTempC());
  
}

void tempTreshold(double temp){
  if (temp > 35){
    buzzWarning();
    Serial.println("checkTemp");
  }
}

void buzz(){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  Serial.println("BUZZ");
  delay(200);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
}

void buzzWarning(){
  Serial.println("buzz warning");
  for(int i = 0; i<10; i++){
      tone(buzzer, 1000); // Send 1KHz sound signal...
      Serial.println("BUZZ");
      delay(100);        // ...for 1 sec
      noTone(buzzer);     // Stop sound...
      delay(100);
  }
}

void printLED(int temperature){
  // Set the brightness to 5 (0=dimmest 7=brightest)
  display.setBrightness(5);

  // Set all segments ON
  display.setSegments(allON);

  delay(500);
  display.clear();


  //print temp 
  display.showNumberDec(temperature, false, 2, 0);
  display.setSegments(celsius, 2, 2);
  tempTreshold(temperature);

  //Turn off display
  delay(2000);
  display.clear();
}


void loop() {
  Blynk.run(); 
  buttonState = digitalRead(button);
  if(buttonState == 1){
    buzz();
    readTemp(); 
  }
}

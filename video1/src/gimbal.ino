#include<Wire.h>
void read_signals(void);
void calibrate(void);
void mpu_init(void);
#include<Servo.h>
Servo rollServo;
const char adr=0x68;
#define lsb 125
#define sample 2200
unsigned long now;
unsigned long pre=0;

float raw_angle_x,raw_angle_x_c;
void setup() {
  rollServo.attach(5);
  Serial.begin(9600);
 mpu_init();
 calibrate();
 rollServo.write(90);
 delay(20);
 
 
}

void loop() {
 read_signals();
  raw_angle_x-=raw_angle_x_c;
ccr();
}
void read_signals(void){
  Wire.beginTransmission(adr);
  Wire.write(0x1A);
  Wire.write(0x05); //10hz low pass filter
  Wire.endTransmission();
  Wire.beginTransmission(adr);
  Wire.write(0x1B);
  Wire.write(0x08); //125LSB
  Wire.endTransmission(); 
 Wire.beginTransmission(adr);
  Wire.write(0x3B); //we want only  the accc values out for this purpose so the starting address is 0x43
  Wire.endTransmission();
  Wire.requestFrom(adr,6);// 6bytes of acc information
  int16_t acc_x=(Wire.read()<<8)|Wire.read();
  int16_t acc_y=(Wire.read()<<8)|Wire.read();
  int16_t acc_z=(Wire.read()<<8)|Wire.read();
  
  raw_angle_x=atan2(-acc_x, sqrt((long)acc_y * acc_y + (long)acc_z * acc_z))* 180 / PI;
}
void calibrate(void){
  Serial.println("don't move the robot for first 4-5 seconds it is calibrating.....");
  for(int i=0;i<sample;i++){
    
    read_signals();
    //don't move the robot for first 4-5 seconds it is calibrating
    raw_angle_x_c+=raw_angle_x;
   
    delay(1);
  }
  raw_angle_x_c/=sample;
  
  
}
void mpu_init(void){
 
  Wire.setClock(400000);
  Wire.begin();
  delay(50);
  Wire.beginTransmission(adr);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  
}
void ccr(){
  rollServo.write(map(raw_angle_x,-90,90,180,0));
  delay(1);
}

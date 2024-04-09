
//This creates a bridge between Serial and Classical Bluetooth (SPP)

#include "BluetoothSerial.h" // including bluetooth lib
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;// creat bluetooth obj
char cmd='0';// command var 
char R_Data;// recevied data from pc var
int f; // flag var
// defines pins numbers
const int stepPin = 27;
const int dirPin = 26;
float waitTime = 0.003;// the time between pulses
int accelerationTerm = 4000;
int roundedWaitTime;
unsigned long timer1;
unsigned long timer2;
unsigned long difference;
uint64_t i = 0;
//0.000150 to increas speed make the number smaller
#define speed1 0.000150 // speed1 150 the low speed
#define speed2 0.000130 // // speed2 130 the high speed
//-------------------------------------------------------
//tb2=angel/cons*td1+(angel/td1) ---> td2=1/(v+1/td2)
//function to return the next delay value for positive acceleration
float positiveAcceleration(float waitTime,float speed_duration) {
  float dVelocity = waitTime * accelerationTerm; // the diff of vlocity = time * acceleration
  waitTime = 1 / (dVelocity + 1 / waitTime); // sum the td2
  if (waitTime < speed_duration) { // if the time == the maxmum time
    waitTime = speed_duration;
  }
  return waitTime;
}

//function to return the next delay value for negative acceleration
float negativeAcceleration(float waitTime) {
  float dVelocity = waitTime * -1 * accelerationTerm;
  waitTime = 1 / (dVelocity + 1 / waitTime);
  if (waitTime > 0.003) {
    waitTime = 0.003;
  }
  return waitTime;
}
void setup() {
  Serial.begin(115200);
  SerialBT.begin("Motor_driver"); //Bluetooth device name
  pinMode(2, OUTPUT); // pin of led
  pinMode(stepPin, OUTPUT); //pin step
  pinMode(dirPin, OUTPUT); // pin diraction
  digitalWrite(dirPin, HIGH);//set the dir to 1
}

void loop() {

  //SerialBT.println("Hello World");

  if (SerialBT.available())// check bluetooth 
  {
    
    R_Data = SerialBT.read();// get the data from blutooth
    if(R_Data=='1'||R_Data=='0'||R_Data=='2')//make sure that the data is valid
    cmd=R_Data;
    Serial.println(cmd);
  }
  if (cmd == '1')
  {
    digitalWrite(2, 1); // turn on led

  if (f == 0)
  {
    for (i = 0; i < 15000; i++) {      // this block for start only
      waitTime = positiveAcceleration(waitTime,speed1);
      //Serial.println(waitTime);
      roundedWaitTime = round(waitTime * 1000000); // make withe in dec number to use it with Microseconds fun
      //Serial.println(roundedWaitTime);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(roundedWaitTime);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(roundedWaitTime);

      f = 1;
    }
  }
  else {// normal speed
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(roundedWaitTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(roundedWaitTime);
  }
  }

 else if (cmd == '2')// speed2 block
  {
    digitalWrite(2, 1);// turn on led

  if (f == 0)
  {
    for (i = 0; i < 15000; i++) { // this block for start only
      waitTime = positiveAcceleration(waitTime,speed2);
      //Serial.println(waitTime);
      roundedWaitTime = round(waitTime * 1000000);
      //Serial.println(roundedWaitTime);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(roundedWaitTime);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(roundedWaitTime);

      f = 1;
    }
  }
  else {// normal speed
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(roundedWaitTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(roundedWaitTime);
  }
  }

else if (cmd == '0')
{
  digitalWrite(2, LOW);// turn off led
  if (f == 1)
  {
    for ( i = 0; i < 10000; i++) {// this block for smooth stop
      waitTime = negativeAcceleration(waitTime);
      roundedWaitTime = round(waitTime * 1000000);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(roundedWaitTime);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(roundedWaitTime);
      if(roundedWaitTime ==3000 )
      {
        break;
        }
      
    }
    f = 0;
  }
  digitalWrite(stepPin, LOW); // stop motor
}
else
{
  }
}

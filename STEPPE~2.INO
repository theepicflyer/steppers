#include <AccelStepper.h>

#define dirPin 2
#define stepPin 3
#define enPin 4
#define motorInterfaceType 1
int steps = 2000 ; //3800-4000 seems to be the working range, floor unknown but estimated to be approx 1000
int counter = 0 ;
int rev = 0 ;
int interval = 2 ;
int interval2 = 0;
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);


void setup() {
  //Serial.begin(9600);
  pinMode(enPin, OUTPUT);
  stepper.setMaxSpeed(5000); //our usual speed is 500!
  stepper.setSpeed(200); // steps per second, set to 200 for 1 rps, set to 30 for 0.57 rpm, 10 for 0.19 rpm
}

void loop() {
  if (rev == interval & steps >1200) {
    interval2++;
    steps = steps*0.88; //every interval
    if (interval2 % 2 == 0){ //so that interval only increases every 2 cycles
      interval+=2;
    }
    rev = 0 ;
  } 
  for (int i = 0; i <= 801; i++) {
    stepper.setSpeed(800);
    stepper.moveTo(stepper.currentPosition()+1);
    stepper.runSpeedToPosition();
  }
  
  if (counter < steps)//this counter determines the amt of steps/revolutions. 
    {
      counter++;
    }
  else
    {
      rev++ ;
      digitalWrite(enPin, HIGH);
      //delay(60000);//delay for 40/80 minutes, which is 2,400,000 ms/4,800,000 ms//delay 7 min, 420000 ms
      //delay(10000); //for testing purposes ONLY
      counter = 0;
      digitalWrite(enPin, LOW);
    }
}

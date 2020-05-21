// LRA motor driver pin assignment
const int motor1_F = 2;
const int motor1_R = 3;
const int motor2_F = 6;
const int motor2_R = 7;
const int motor3_F = 10;
const int motor3_R = 11;
const int motor4_F = 12;
const int motor4_R = 13;

bool stringComplete = false;
char inData[1000];
int dataIdx = 0;
bool motorOn[9] = {false, false, false, false, false, false, false, false, false};

int curTime = 0; // Timer starts from 0 when pattern starts

void setup() {
  pinMode (motor1_F, OUTPUT);
  pinMode (motor1_R, OUTPUT);
  pinMode (motor2_F, OUTPUT);
  pinMode (motor2_R, OUTPUT);
  pinMode (motor3_F, OUTPUT);
  pinMode (motor3_R, OUTPUT);
  pinMode (motor4_F, OUTPUT);
  pinMode (motor4_R, OUTPUT);

  Serial.begin(115200);
  while (! Serial);
  Serial.println("Vibration test platform activated...");
}

void loop() {
  loopSerial();
  loopMotorOnOff();
}

void getSerial()
{
  while(Serial.available() && stringComplete == false)
  {
    char inChar = Serial.read();
    inData[dataIdx++] = inChar;

    if(inChar == '\n')
    {
      dataIdx = 0;
      stringComplete = true;
    }
  }
}

// Function: loopSerial
// Serial input for debug 
void loopSerial()
{
  getSerial();
  if(stringComplete)
  {
    char line[1000];
    int lineIdx = 0;
    // Count command chars & init inData
    while(inData[lineIdx] != '\n' && lineIdx < 100)
    {
      line[lineIdx] = inData[lineIdx];
      inData[lineIdx] = NULL;
      lineIdx++;
    }
    
    char c1 = line[0], c2 = line[1], c3 = line[2], c4 = line[3];
    int pokeNum = 0;
    int motorNum = 0;
    
    switch(c1)
    {
      case 'm':
        motorNum = (int)c2 - 49;
        if(0 <= motorNum && motorNum < 9)
        {
          motorOn[motorNum] = !motorOn[motorNum];
          Serial.print("Motor");
          Serial.print(c2);
          if(motorOn[motorNum])
          {
            Serial.print(": ON\n");
          }
          else
          {
             Serial.print(": OFF\n");
          }
          Serial.flush();
        }
        break;
      case 'a':
        motorNum = (int)c2 - 49;
        if(0 <= motorNum && motorNum < 9)
        {
          Serial.print("Pulse");
          Serial.println(c2);
          motorPulse(motorNum);
          Serial.flush();
        }
        break;
      case 's':
        // sweeping
        Serial.println("Sweeping");
        sweeping();
        break;
      case 'r':
        // cutaneous rabbit
        Serial.println("Cutaneous rabbit");
        cut_rabbit();
        break;
      case 'p':
        // physical rabbit
        Serial.println("Physical rabbit");
        phy_rabbit();
        break;
      case 'z':
        Serial.println("Stop all");
        for(int i=0;i<9;i++)
        {
          motorOn[i] = false;
        }
        Serial.flush();
        break;
      default:
        break;
    }
    stringComplete = false;
  }
}

// Function: loopMotorOnOff
// Turn on LRA if true
void loopMotorOnOff ()
{
  digitalWrite(motor1_F, LOW);
  digitalWrite(motor1_R, LOW);
  digitalWrite(motor2_F, LOW);
  digitalWrite(motor2_R, LOW);
  digitalWrite(motor3_F, LOW);
  digitalWrite(motor3_R, LOW);
  digitalWrite(motor4_F, LOW);
  digitalWrite(motor4_R, LOW);
  
  //Forward
  if(motorOn[0])
  {
    digitalWrite(motor1_F, HIGH);
    digitalWrite(motor1_R, LOW);
  }
  if(motorOn[1])
  {
    digitalWrite(motor2_F, HIGH);
    digitalWrite(motor2_R, LOW);
  }
  if(motorOn[2])
  {
    digitalWrite(motor3_F, HIGH);
    digitalWrite(motor3_R, LOW);
  }
  if(motorOn[3])
  {
    digitalWrite(motor4_F, HIGH);
    digitalWrite(motor4_R, LOW);
  }
     
  delayCount(3);
  
  if(motorOn[0])
  {
    digitalWrite(motor1_F, LOW);
    digitalWrite(motor1_R, LOW);
  }
  if(motorOn[1])
  {
    digitalWrite(motor2_F, LOW);
    digitalWrite(motor2_R, LOW);
  }
  if(motorOn[2])
  {
    digitalWrite(motor3_F, LOW);
    digitalWrite(motor3_R, LOW);
  }
  if(motorOn[3])
  {
    digitalWrite(motor4_F, LOW);
    digitalWrite(motor4_R, LOW);
  }
  
  //delayCount(2);
  
  //Reverse
  if(motorOn[0])
  {
    digitalWrite(motor1_F, LOW);
    digitalWrite(motor1_R, HIGH);
  }
  if(motorOn[1])
  {
    digitalWrite(motor2_F, LOW);
    digitalWrite(motor2_R, HIGH);
  }
  if(motorOn[2])
  {
    digitalWrite(motor3_F, LOW);
    digitalWrite(motor3_R, HIGH);
  }
  if(motorOn[3])
  {
    digitalWrite(motor4_F, LOW);
    digitalWrite(motor4_R, HIGH);
  }
  
  delayCount(3);
  
  if(motorOn[0])
  {
    digitalWrite(motor1_F, LOW);
    digitalWrite(motor1_R, LOW);
  }
  if(motorOn[1])
  {
    digitalWrite(motor2_F, LOW);
    digitalWrite(motor2_R, LOW);
  }
  if(motorOn[2])
  {
    digitalWrite(motor3_F, LOW);
    digitalWrite(motor3_R, LOW);
  }
  if(motorOn[3])
  {
    digitalWrite(motor4_F, LOW);
    digitalWrite(motor4_R, LOW);
  }

  //delayCount(2);
}

void motorPulse(int motor_Num)
{
  byte motor_F, motor_R;

  switch(motor_Num)
  {
    case 0:
      motor_F = motor1_F;
      motor_R = motor1_R;
      break;
    case 1:
      motor_F = motor2_F;
      motor_R = motor2_R;
      break;
    case 2:
      motor_F = motor3_F;
      motor_R = motor3_R;
      break;
    case 3:
      motor_F = motor4_F;
      motor_R = motor4_R;
      break;
    default:
      break;
  }
  
  digitalWrite(motor_F, LOW);
  digitalWrite(motor_R, LOW);
  
  //Forward
  digitalWrite(motor_F, HIGH);
  digitalWrite(motor_R, LOW);
  
  delayCount(3);
  
  digitalWrite(motor_F, LOW);
  digitalWrite(motor_R, LOW);

  //delayCount(2);
  
  //Reverse
  digitalWrite(motor_F, LOW);
  digitalWrite(motor_R, HIGH);

  delayCount(3);

  digitalWrite(motor_F, LOW);
  digitalWrite(motor_R, LOW);

  //delayCount(2);
}

void sweeping()
{
  int term = 300;
  for(int i=0;i<5;i++)
  {
    motorPulse(0);
    delayCount(term);
    motorPulse(1);
    delayCount(term);
    motorPulse(2);
    delayCount(term);
    motorPulse(3);
    delayCount(term);
    motorPulse(2);
    delayCount(term);
    motorPulse(1);
    delayCount(term);
  }
}

void cut_rabbit()
{
  int term = 80;
  delayCount(2000);
  motorPulse(0);
  delayCount(term);
  motorPulse(0);
  delayCount(term);
  motorPulse(0);
  delayCount(term);
  motorPulse(0);
  delayCount(term);
  motorPulse(3);
  delayCount(term);
  motorPulse(3);
  delayCount(term);
  motorPulse(3);
  delayCount(term);
  motorPulse(3);
}

void phy_rabbit()
{
  int term = 80;
  delayCount(2000);
  motorPulse(0);
  delayCount(term);
  motorPulse(0);
  delayCount(term);
  motorPulse(1);
  delayCount(term);
  motorPulse(1);
  delayCount(term);
  motorPulse(2);
  delayCount(term);
  motorPulse(2);
  delayCount(term);
  motorPulse(3);
  delayCount(term);
  motorPulse(3);
}

// Function: delayCount
// Delay time and count up currTime
void delayCount(int time)
{
  curTime = curTime + time;
  delay(time);
}

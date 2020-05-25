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
int act_order[4] = {-1, -1, -1, -1};
float delay_list[4] = {4000, 4000, 4000, 4000}; // 4000 means unactivated.

struct motor_unit
{
  float magnitude = 0; // from 0~1
  unsigned int delay = 4000; // delay = 3ms * magnitude
  int pin_F;
  int pin_R;
  bool onoff = false;
};
motor_unit motors[4];

void setup() {
  pinMode (motor1_F, OUTPUT);
  pinMode (motor1_R, OUTPUT);
  pinMode (motor2_F, OUTPUT);
  pinMode (motor2_R, OUTPUT);
  pinMode (motor3_F, OUTPUT);
  pinMode (motor3_R, OUTPUT);
  pinMode (motor4_F, OUTPUT);
  pinMode (motor4_R, OUTPUT);

  motors[0].pin_F = motor1_F;
  motors[0].pin_R = motor1_R;
  motors[1].pin_F = motor2_F;
  motors[1].pin_R = motor2_R;
  motors[2].pin_F = motor3_F;
  motors[2].pin_R = motor3_R;
  motors[3].pin_F = motor4_F;
  motors[3].pin_R = motor4_R;

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
    int motorMag = 0;

    switch(c1)
    {
      case 'm':
        motorNum = (int)c2 - 49;
        motorMag = ((int)c3 - 48) * 10 + (int)c4 - 48;
        if(0 <= motorNum && motorNum < 4)
        {
          Serial.print("Motor");
          Serial.print(c2);
          Serial.print(": ");
          Serial.println(motorMag);
          motorActivate(motorNum, motorMag);
          Serial.flush();
        }
        break;
      case 'a':
        motorNum = (int)c2 - 49;
        if(0 <= motorNum && motorNum < 4)
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
        for(int i=0;i<4;i++)
        {
          motorActivate(i, 0);
        }
        Serial.flush();
        break;
      default:
        break;
    }
    stringComplete = false;
  }
}

void reorder_delay()
{
  for(int i=0;i<4;i++)
  {
    delay_list[i] = motors[i].delay;
    act_order[i] = i;
  }

  for(int i=0;i<3;i++)
  {
    for(int j=i+1;j<4;j++)
    {
      if(delay_list[i] > delay_list[j])
      {
        float tmp_delay = delay_list[i];
        delay_list[i] = delay_list[j];
        delay_list[j] = tmp_delay;

        float tmp_order = act_order[i];
        act_order[i] = act_order[j];
        act_order[j] = tmp_order;
      }
    }
  }

  //Serial.println("reorder result");
  for(int i=0;i<4;i++)
  {
    if(motors[act_order[i]].onoff == false)
    {
      act_order[i] = -1;
    }
    else if(i > 0)
    {
      delay_list[i] = motors[act_order[i]].delay - motors[act_order[i-1]].delay;
    }

    /*
    Serial.print("M");
    Serial.print(act_order[i]);
    Serial.print(": ");
    Serial.println(delay_list[i]);
    */
  }
}

void motorActivate(int motor_num, int mag)
{
  if(mag > 0)
  {
    motors[motor_num].magnitude = (float)mag / 100.0;
    motors[motor_num].delay = (unsigned int)(2000.0 * (float)mag / 100.0);
    motors[motor_num].onoff = true;
  }
  else
  {
    motors[motor_num].magnitude = 0;
    motors[motor_num].delay = 4000;
    motors[motor_num].onoff = false;
  }
  reorder_delay();
}


// Function: loopMotorOnOff
// Turn on LRA if true
void loopMotorOnOff ()
{
  for(int i=0;i<4;i++)
  {
    digitalWrite(motors[i].pin_F, LOW);
    digitalWrite(motors[i].pin_R, LOW);
  }

  // Forward
  for(int i=0;i<4;i++)
  {
    if(motors[i].onoff)
    {
      digitalWrite(motors[i].pin_F, HIGH);
      digitalWrite(motors[i].pin_R, LOW);
    }
  }

  // Stand-by
  for(int i=0;i<4;i++)
  {
    if(act_order[i] != -1)
    {
      delayMicroseconds(delay_list[i]);
      digitalWrite(motors[act_order[i]].pin_F, LOW);
      digitalWrite(motors[act_order[i]].pin_R, LOW);
    }
    else
    {
      if(i > 0)
      {
        delayMicroseconds(2000 - motors[act_order[i-1]].delay);
      }
      else
      {
        delayMicroseconds(2000);
      }
      break;
    }
  }

  //Reverse
  for(int i=0;i<4;i++)
  {
    if(motors[i].onoff)
    {
      digitalWrite(motors[i].pin_F, LOW);
      digitalWrite(motors[i].pin_R, HIGH);
    }
  }

  // Stand-by
  for(int i=0;i<4;i++)
  {
    if(act_order[i] != -1)
    {
      delayMicroseconds(delay_list[i]);
      digitalWrite(motors[act_order[i]].pin_F, LOW);
      digitalWrite(motors[act_order[i]].pin_R, LOW);
    }
    else
    {
      if(i > 0)
      {
        delayMicroseconds(2000 - delay_list[i-1]);
      }
      else
      {
        delayMicroseconds(2000);
      }
      break;
    }
  }
}

void motorPulse(int motor_Num)
{
  byte motor_F, motor_R;

  motor_F = motors[motor_Num].pin_F;
  motor_R = motors[motor_Num].pin_R;

  digitalWrite(motor_F, LOW);
  digitalWrite(motor_R, LOW);

  //Forward
  digitalWrite(motor_F, HIGH);
  digitalWrite(motor_R, LOW);

  delayCount(2);

  digitalWrite(motor_F, LOW);
  digitalWrite(motor_R, LOW);

  //delayCount(2);

  //Reverse
  digitalWrite(motor_F, LOW);
  digitalWrite(motor_R, HIGH);

  delayCount(2);

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
  delay(time);
}

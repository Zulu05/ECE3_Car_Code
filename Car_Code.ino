#include <ECE3.h>

uint16_t sensorValues[8];

int min_vals[8] = {687, 687, 710, 664, 828, 641, 687, 687}; //maybe its hte 828 causing issue? //measured 
// int min_vals[8] = {687, 687, 710, 664, 664, 641, 687, 687}; //maybe its hte 828 causing issue?
// int min_vals[8] = {814, 630, 607.8, 585, 608, 621, 652, 699};
int max_vals[8] = {1743, 1813, 1790, 1836, 1672, 1859, 1813, 1813}; //measured 
// int max_vals[8] = {1743, 1813, 1790, 1836, 1836, 1859, 1813, 1813};
// int max_vals[8] = {1686, 1487, 1541, 974, 1091, 1620, 1489, 1798}; //maybe is the 1836
int weight_vals[8] = {-16, -14, -10, -4, 4, 10, 14, 16};
const int left_nslp_pin=31; // nslp ==> awake & ready for PWM
const int left_dir_pin=29;
const int left_pwm_pin=40;
const int right_nslp_pin=11; // nslp ==> awake & ready for PWM
const int right_dir_pin=30;
const int right_pwm_pin=39;

int base_speed = 180;
float prev_error = NULL;
bool firstDonut = false;
bool secondDonut = false;

void setup() {
  // put your setup code here, to run once:
  ECE3_Init();
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);
  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  digitalWrite(right_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);
  digitalWrite(right_nslp_pin,HIGH);

  // float margin = .75;
  // int max_error = 2800;
  // float correction = min(base_speed, 255 - base_speed); 
  // correction *= margin;
  // kp = correction /= max_error;
  // kp *= -1;

  Serial.begin(9600); // set the data rate in bits per second for serial data transmission
  delay(2000);
}

void loop() {
  // digitalWrite(right_dir_pin,LOW);
  // analogWrite(left_pwm_pin,180);
  // analogWrite(right_pwm_pin,180);
  // delay(200);
  // analogWrite(left_pwm_pin,0);
  // analogWrite(right_pwm_pin,0);
  // digitalWrite(right_dir_pin,HIGH);
  // delay(150);
  // analogWrite(left_pwm_pin,255);
  // analogWrite(right_pwm_pin,255);
  // delay(265);
  // analogWrite(left_pwm_pin,0);
  // analogWrite(right_pwm_pin,0);
  // delay(5000);
  // return;
  // digitalWrite(right_dir_pin,LOW);
  // analogWrite(left_pwm_pin,150);
  // analogWrite(right_pwm_pin,150);
  // delay(200);
  ECE3_read_IR(sensorValues);
  // put your main code here, to run repeatedly:
  // Take the average of 5 consecutive values for each sensor
  int Normalized_values[8] = {0};
  float weight = 0;
  float curr_error = 0;
  int donut = 0;
  

  // Print average values (average value = summed_values / number_samples
  for (unsigned char i = 0; i < 8; i++) {
    donut += sensorValues[i];
    Normalized_values[i] = max(0, sensorValues[i]-min_vals[i]);
    Normalized_values[i] *= 1000;
    Normalized_values[i] /= max_vals[i];
    weight += Normalized_values[i] * weight_vals[i];
  }
  if (donut > 18000)
  {
    if(firstDonut && !secondDonut)
    {
      analogWrite(left_pwm_pin,0);
      analogWrite(right_pwm_pin,0);
      digitalWrite(right_dir_pin,HIGH);
      delay(150);
      analogWrite(left_pwm_pin,255);
      analogWrite(right_pwm_pin,255);
      delay(265);
      analogWrite(left_pwm_pin,0);
      analogWrite(right_pwm_pin,0);
      delay(100);
      digitalWrite(right_dir_pin,LOW);
      analogWrite(left_pwm_pin,150);
      analogWrite(right_pwm_pin,150);
      delay(200);
      secondDonut = true;
      firstDonut = false;
      return;
    }
    else if (firstDonut && secondDonut)
    {
      analogWrite(left_pwm_pin,0);
      analogWrite(right_pwm_pin,0);
      delay(500);
      digitalWrite(left_nslp_pin,LOW);
      digitalWrite(right_nslp_pin,LOW); 
    }
    else
    {
      firstDonut = true;
    }
  }
  else
  {
    firstDonut = false;
  }
  Serial.println();
  int leftSpd = base_speed;
  int rightSpd = base_speed;
  curr_error = weight / 2;
  int kp_change = (.0095)*curr_error;
  leftSpd -= kp_change;
  rightSpd += kp_change;
  if(prev_error == NULL)
  {
    prev_error = curr_error;
  }
  int kd_change = (-.095) * (curr_error - prev_error);
  prev_error = curr_error;
  // Serial.print(kp_change);
  // Serial.print(":");
  // Serial.print(kd_change);
  // Serial.print(":");
  // Serial.print(curr_error);
  // Serial.println();
  // Serial.println();
  // delay(500);
  leftSpd += kd_change;
  rightSpd -= kd_change;
  leftSpd = constrain(leftSpd, 0, 255);
  rightSpd = constrain(rightSpd, 0, 255);
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}

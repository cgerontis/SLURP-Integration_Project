/***************************
 * Aperture Control
 * 0 is 0 and 15000 is 75mm, so 200 ticks is 1mm
 * ratio for 50mm aperture is -243
 ***************************/
#include <ax12.h>
#include <BioloidController.h>

double apRatio50 = -243;
double apRatio75 = 200;

void moveAperture(int id, double desiredPosition, int diam)
{
  int apRatio;
  if(diam == 75)
  {
    apRatio = apRatio75;
  }else
  {
    apRatio = apRatio50;
  }
  
  if(desiredPosition > 80)
  {
    Serial.println("Value too high");
    return;
  }else if (desiredPosition < 0)
  {
    Serial.println("Value too low");
    return;
  }
  
  Serial.print("Moving to ");
  Serial.print(desiredPosition);
  Serial.println("mm");
  
  desiredPosition = desiredPosition*apRatio;  //Converting the commanded position from mm to values, which can be sent to servo

  ax12SetRegister2 (id , AX_GOAL_POSITION_L, desiredPosition);
  
  delay(10);

}



/***************************
 * Aperture Calibration
 * 
 * This calibration function runs the aperture servo until it senses a certain amount of 
 * torque
 * 
 * int origin(optional) is the value added to the origin after hitting the 
 * end. This was added because of the aperture is flexible when closing. I've found
 * that 600 works well
 * 
 * 
 * 200 values is 1mm
 ***************************/
#include <ax12.h>
#include <BioloidController.h>

double calibrate(int id,int diam)
{

  
    double offset;      //Offset of the servo position
    double pos;         //Later on saves servo position
    int load = 0;           //Later on saves servo load
    int maxLoad = 60;   //Maximum load servo applies before giving up and declaring origin
    int fast = 0;
    int apRatio = 0;
    if(diam == 75)
    {
      apRatio = apRatio75;
      fast = 1800;
    }else
    {
      apRatio = apRatio50;
      fast = 800;
    }

    ax12SetRegister2(id, 20, 0);
    ax12SetRegister2 (id , AX_CW_ANGLE_LIMIT_L, 0);
    ax12SetRegister2 (id , AX_CCW_ANGLE_LIMIT_L, 0);  
    ax12SetRegister2 (id , AX_GOAL_SPEED_L, fast); 
    ax12SetRegister2 (id , AX_TORQUE_LIMIT_L,250); 

    delay(200);
    
    while(load < maxLoad)
    {
      load = ax12GetRegister(id , AX_PRESENT_LOAD_L,1);
      pos = GetPosition(id);
//      Serial.print("Position: ");
//      Serial.println(pos);
//      Serial.print("Load: ");
//      Serial.println(load);
      delay(20);
    }
    Serial.println("Out");
    delay(50);
    ax12SetRegister2(id, AX_TORQUE_ENABLE, 0);
    delay(100);
    Serial.println("Aperture calibrated");
    delay(500);
    ax12SetRegister2(id,AX_CCW_ANGLE_LIMIT_L,4095);
    ax12SetRegister2(id,AX_CW_ANGLE_LIMIT_L,4095);
    delay(100);
    ax12SetRegister2(id, AX_TORQUE_ENABLE, 1);
    delay(1000);
    pos = GetPosition(id);
    delay(500);
    offset = (-1*pos)+(1.8*apRatio);
    if(diam = 75)
    {
      offset = -1*(pos+600);
      delay(50);
    }
    delay(50);
    Serial.print("offset: ");
    Serial.println(offset);
    ax12SetRegister2(id , 20, offset);
    delay(50);
    if(diam = 75)
    {
      ax12SetRegister2 (2 , AX_GOAL_POSITION_L, 0);
    }else
    {
      ax12SetRegister2 (2 , AX_GOAL_POSITION_L, apRatio*1.8);
    }
    delay(500);
    ax12SetRegister2 (id , AX_TORQUE_LIMIT_L,1000); 
    delay(50);
    return offset;
    
}

double getAperturePosition(int id,int diam)
{
  int apRatio;
  if(diam == 75)
  {
    apRatio = apRatio75;
  }else
  {
    apRatio = apRatio50;
  }
  return (GetPosition(id)/apRatio50);
}





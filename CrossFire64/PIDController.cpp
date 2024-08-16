#include "PIDController.h"

PIDController::PIDController()
  :kp_(0.0f), ki_(0.0f), kd_(0.0f), target_(0.0f), last_err_(0.0f), acc_err_(0.0f)
{
}

void PIDController::setTarget(float target)
{
  //acc_err_ = 0.0f;
  //last_err_ = 0.0f;
  target_ = target;
}

float PIDController::run(float value)
{
  float error = target_ - value;
  acc_err_ += error;
  float reuslt = kp_ * error + ki_ * acc_err_ + kd_ * (error - last_err_ == 0.0f ? error : last_err_);
  last_err_ = error;
  return reuslt;
}
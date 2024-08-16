#pragma once
class PIDController
{
public:
  PIDController();

  void setTarget(float target);
  void setProportion(float p) { kp_ = p; };
  void setIntegral(float i) { ki_ = i; };
  void setDifferential(float d) { kd_ = d; };

  float run(float value);

private:
  float kp_;
  float ki_;
  float kd_;
  float last_err_;
  float acc_err_;
  float target_;
};

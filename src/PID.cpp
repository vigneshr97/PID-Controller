#include "PID.h"
#include <iostream>
using namespace std;
/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

PID::PID() 
{
  // p_error = 1;
  // i_error = 1;
  // d_error = 1;
  // Kp = 0;
  // Ki = 0;
  // Kd = 0;
  // err = 0;
}

PID::~PID() 
{
  cout<<"destructor ran"<<endl;
}

void PID::Init(double Kp_, double Ki_, double Kd_) 
{
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */
  Kp = Kp_;
  Ki = Ki_;
  Kd = Kd_;
  p_error = 0;
  i_error = 0;
  d_error = 0;
}

void PID::UpdateError(double cte) 
{
  /**
   * TODO: Update PID errors based on cte.
   */
  d_error = cte - p_error;
  p_error = cte;
  i_error += cte;
}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  double err = -1*(Kp*p_error+Ki*i_error+Kd*d_error);
  return err;  // TODO: Add your total error calc here!
}
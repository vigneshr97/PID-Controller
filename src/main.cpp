#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;
using namespace std;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() 
{
  uWS::Hub h;

  PID pid;
  bool twiddle = false;
  double p[3];
  double dp[3] = {0.01,0.00002,0.3};
  double total_cte = 0;
  double tol = 0.2;
  int n = 0;
  int max_n = 1000;
  double error = 32000;
  double best_err = 32000;
  double best_p[3] = {p[0], p[1], p[2]};
  int param_iter = 0;
  int iter = 0;
  if (twiddle == true)
  {
    p[0] = 0.05;
    p[1] = 0.0001;
    p[2] = 1.5;
  }
  else
  {
    // p[0] = 0.2;
    // p[1] = 0.004;
    // p[2] = 3.0;
    // p[0] = 0.13;
    // p[1] = 0.0003;
    // p[2] = 3;
    p[0] = 0.12;
    p[1] = 0.0002;
    p[2] = 6;
  }
  pid.Init(p[0], p[1], p[2]);
  /**
   * TODO: Initialize the pid variable.
   */

  h.onMessage([&pid, &p, &dp, &total_cte, &twiddle, &tol, &n, &max_n, &error, &best_err, &best_p, &param_iter, &iter]
    (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) 
  {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    pid.length = length;

    if (length && length > 2 && data[0] == '4' && data[1] == '2') 
    {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") 
      {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") 
        {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          cout<<p[0]<<' '<<p[1]<<' '<<p[2]<<endl;
          pid.UpdateError(cte);
          double steer_value = pid.TotalError();
          double throttle_val = 0.3;
          json msgJson;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */

          // if (twiddle)
          // {
          //   total_cte += cte*cte;
          //   n += 1;
          //   if (dp[0]+dp[1]+dp[2] > tol)
          //   {
          //     p[0] += dp[0];
          //     p[1] += dp[1];
          //     p[2] += dp[2];
          //     if(n < 2*max_n)
          //     {
          //       pid.UpdateError(cte);
          //       steer_value = pid.TotalError();
          //       if (n > max_n)
          //       {
          //         total_cte += cte*cte;
          //       }
          //     }
          //     else
          //     {

          //     }
          //   }
          //   n += 1;
          // }
          // else
          // {
          //   pid.UpdateError(cte);
          //   steer_value = pid.TotalError();
          // }
          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value 
                    << std::endl;

          msgJson["steering_angle"] = steer_value;
          //msgJson["throttle"] = (1 - std::abs(steer_value)) * 0.5 + 0.2;
          msgJson["throttle"] = throttle_val;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage


  // We don't need this since we're not using HTTP but if it's removed the
  // program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1) {
      res->end(s.data(), s.length());
    } else {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}

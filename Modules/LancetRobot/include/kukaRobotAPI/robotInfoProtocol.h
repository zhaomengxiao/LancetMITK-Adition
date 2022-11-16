
#ifndef ROBOTINFOPROTOCOL_H
#define ROBOTINFOPROTOCOL_H
#include <array>
#include <Poco/JSON/Object.h>

class RobotInfoProtocol
{
public:
  double joint1;
  double joint2;
  double joint3;
  double joint4;
  double joint5;
  double joint6;
  double joint7;

  double Flange1;
  double Flange2;
  double Flange3;
  double Flange4;
  double Flange5;
  double Flange6;

  void FromJsonObj(Poco::JSON::Object obj)
  {
    joint1 = obj.get("joint1");
    joint2 = obj.get("joint2");
    joint3 = obj.get("joint3");
    joint4 = obj.get("joint4");
    joint5 = obj.get("joint5");
    joint6 = obj.get("joint6");
    joint7 = obj.get("joint7");

    Flange1 = obj.get("Flange1");
    Flange2 = obj.get("Flange2");
    Flange3 = obj.get("Flange3");
    Flange4 = obj.get("Flange4");
    Flange5 = obj.get("Flange5");
    Flange6 = obj.get("Flange6");
  }
};
#endif // ROBOTINFOPROTOCOL_H

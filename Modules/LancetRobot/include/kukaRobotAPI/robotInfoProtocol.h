#ifndef ROBOTINFOPROTOCOL_H
#define ROBOTINFOPROTOCOL_H

#include <array>
#include <Poco/JSON/Object.h>

// class RobotInfoProtocol
// {
// public:
//   double joint1;
//   double joint2;
//   double joint3;
//   double joint4;
//   double joint5;
//   double joint6;
//   double joint7;
//
//   double Flange1;
//   double Flange2;
//   double Flange3;
//   double Flange4;
//   double Flange5;
//   double Flange6;
//
//   void FromJsonObj(Poco::JSON::Object obj)
//   {
//     joint1 = obj.get("joint1");
//     joint2 = obj.get("joint2");
//     joint3 = obj.get("joint3");
//     joint4 = obj.get("joint4");
//     joint5 = obj.get("joint5");
//     joint6 = obj.get("joint6");
//     joint7 = obj.get("joint7");
//
//     Flange1 = obj.get("Flange1");
//     Flange2 = obj.get("Flange2");
//     Flange3 = obj.get("Flange3");
//     Flange4 = obj.get("Flange4");
//     Flange5 = obj.get("Flange5");
//     Flange6 = obj.get("Flange6");
//   }
// };

class TrackingFramesProtocol
{
public:
  std::string name{"null"};
  bool isMotionFrame{false};
  std::array<double, 6> position{};

  bool FromJsonObj(Poco::JSON::Object obj)
  {
    auto b = obj.get("isMotionFrame");
    auto positionArray = obj.getArray("position");
	auto n = obj.get("name");
    if (b.isEmpty()||positionArray.isNull()|| n.isEmpty())
    {
      return false;
    }
	name = n.toString();
    isMotionFrame = b;
    for (int i = 0; i < 6; i++)
    {
      position[i] = positionArray->getElement<double>(i);
    }
    return true;
  }
};

class RobotInformationProtocol
{
public:
  std::array<double, 7> joints{};
  std::array<double, 6> forcetorque{};
  std::vector<TrackingFramesProtocol> frames{};

  bool FromJsonObj(Poco::JSON::Object obj)
  {
    auto jointsArray = obj.getArray("joints");
    auto forcetorqueArray = obj.getArray("forcetorque");
    auto framesArray = obj.getArray("frames");
    if (jointsArray.isNull()|| forcetorqueArray.isNull()|| framesArray.isNull())
    {
      return false;
    }
    for (int i = 0; i < 7; i++)
    {
      joints[i] = jointsArray->getElement<double>(i);
    }

    for (int i = 0; i < 6; i++)
    {
      forcetorque[i] = forcetorqueArray->getElement<double>(i);
    }
    //forcetorqueArray->getObject()
    for (int i =0; i < framesArray->size(); i++)
    {
      TrackingFramesProtocol trackingFrames;
      if (!trackingFrames.FromJsonObj(*framesArray->getObject(i)))
      {
        return false;
      }
      frames.push_back(trackingFrames);
    }
    return true;
  }
};

#endif
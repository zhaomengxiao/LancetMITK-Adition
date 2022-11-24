#ifndef DEFAULTPROTOCOL_H
#define DEFAULTPROTOCOL_H
#include <array>
#include <Poco/JSON/Object.h>

class Param
{
public:
  std::string target{ "null" };
  double x{};
  double y{};
  double z{};
  double a{};
  double b{};
  double c{};

  Poco::JSON::Object ToJsonObj() const
  {
    Poco::JSON::Object res;
    res.set("target", this->target);
    res.set("x", this->x);
    res.set("y", this->y);
    res.set("z", this->z);
    res.set("a", this->a);
    res.set("b", this->b);
    res.set("c", this->c);

    return res;
  }

  void FromArray(std::array<double,6> array)
  {
    x = array[0];
    y = array[1];
    z = array[2];
    a = array[3];
    b = array[4];
    c = array[5];
  }
};
class DefaultProtocol
{
public:
  std::string operateType{ "null" };
  Param param;
  Param param2;
  int timestamp{};
  std::array<double, 7> jointPos{};

  Poco::JSON::Object ToJsonObj()
  {
    Poco::JSON::Object res;
    res.set("operateType", this->operateType);
    res.set("param", this->param.ToJsonObj());
    res.set("param2", this->param2.ToJsonObj());
    res.set("timestamp", this->timestamp);
    Poco::JSON::Array array;
    array.add(jointPos[0]);
    array.add(jointPos[1]);
    array.add(jointPos[2]);
    array.add(jointPos[3]);
    array.add(jointPos[4]);
    array.add(jointPos[5]);
    array.add(jointPos[6]);

    res.set("jointPos", array);

    return res;
  }
};
#endif // DEFAULTPROTOCOL_H

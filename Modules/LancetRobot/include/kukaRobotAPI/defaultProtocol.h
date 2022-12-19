#ifndef DEFAULTPROTOCOL_H
#define DEFAULTPROTOCOL_H
#include <array>
#include <Poco/JSON/Object.h>

class Param
{
public:
  std::string target{"null"};
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

  void FromArray(std::array<double, 6> array)
  {
    x = array[0];
    y = array[1];
    z = array[2];
    a = array[3];
    b = array[4];
    c = array[5];
  }

  void FromJsonObj(Poco::JSON::Object obj)
  {
    target = obj.get("target").toString();
    x = obj.get("x");
    y = obj.get("y");
    z = obj.get("z");
    a = obj.get("a");
    b = obj.get("b");
    c = obj.get("c");
  }
};

class DefaultProtocol
{
public:
  std::string operateType{"null"};
  Param param{};
  Param param2{};
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

class ResultProtocol
{
public:
  int resultCode;
  std::string resultMsg;
  std::string operateType;
  Param param;

  bool FromJsonObj(Poco::JSON::Object obj)
  {
    auto var_resultCode = obj.get("resultCode");
    auto var_resultMsg = obj.get("resultMsg");
    auto var_operateType = obj.get("operateType");
    auto obj_param = obj.getObject("param");
    if (var_resultCode.isEmpty() || var_resultMsg.isEmpty() || var_operateType.isEmpty()|| obj_param.isNull())
    {
      return false;
    }
    resultCode = var_resultCode;
    resultMsg = var_resultMsg.toString();
    operateType = var_operateType.toString();
    param.FromJsonObj(*obj_param);
    
    return true;
  }

  Poco::JSON::Object ToJsonObj()
  {
	  Poco::JSON::Object res;
	  res.set("operateType", this->operateType);
	  res.set("param", this->param.ToJsonObj());
	  res.set("resultMsg", this->resultMsg);
	  res.set("resultCode", this->resultCode);

	  return res;
  }

  std::string ToString() 
  {
	  auto jsonObj = this->ToJsonObj();
	  std::stringstream strStream;
	  jsonObj.stringify(strStream);
	  return strStream.str();
  }
};
#endif // DEFAULTPROTOCOL_H

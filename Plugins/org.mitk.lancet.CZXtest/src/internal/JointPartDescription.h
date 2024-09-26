#pragma once
#include <string>
class JointPartDescription
{
public:
	std::string Name;
	std::string FileName;
	double Range[2];
	double DH[4];
};


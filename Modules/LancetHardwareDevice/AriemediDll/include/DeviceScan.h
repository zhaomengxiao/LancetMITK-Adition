#ifndef DEVICESCAN_H
#define DEVICESCAN_H

#ifdef ARMDCOMBINEDAPI_EXPORTS
#define ARMDCOMBINED_API __declspec(dllexport)
#else
#define ARMDCOMBINED_API __declspec(dllimport)
#endif

#include "UdpSearchInfo.h"

#include <map>
#include <string>
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <windows.h>

class ARMDCOMBINED_API DeviceScan
{
public:
	explicit DeviceScan();
	~DeviceScan();

	/**
	* @brief: update reachable device hostname and IP 
	*/
	void updateDeviceInfo();

	/**
	* @brief: get reachable device hostname and IP  
	* @return: map of hostname and IP (key: hostname, value:IP)
	*/
	std::map<std::string, std::string> getDeviceInfo();

private:
	UdpSearchInfo* UdpSearcher;

	std::map<std::string, std::string> mapHostIP;
};
#endif
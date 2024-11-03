#ifndef TRACKING_HPP
#define TRACKING_HPP

#include "UdpReader.h"
#include "ToolRegi.h"
#include "SystemCRC.h"

#include <bitset> 

namespace TrackingReplyOption
{
	//! The reply options used by BX and TX commands
	enum value { TransformData = 0x0001, ToolAndMarkerData = 0x0002, SingleStray3D = 0x0004, Tool3Ds = 0x0008, AllTransforms = 0x0800, PassiveStrays = 0x1000 };
}

class Tracking
{
public:
	Tracking(std::string hostname, bool autoIllum, bool errorPrint);
	~Tracking();

	//get connection statues
	int getConnectionStatues();

	//get connection IP
	std::vector<std::string> getIPs();

	//clear buffer
	void clearBuffer();

	//clear last command to initialize
	int initialize();

	//get parameter
	std::vector<double> getParameter();

	//start and end tracking
	int startTraking();
	int endTracking();

	//start and end imaging
	int startImaging();
	int endImaging();

	//start and end reconstructing
	int startRecImaging();
	int endRecImaging();
	void setReconstructArea(short width, short height);
	int startReconstructing(short leftX, short leftY, short rightX, short rightY);
	int endReconstructing();
	int reconstructingReady();

	//update tracking information
	int trackingBX(std::vector<std::string>& warnMessage, std::vector<MarkerPosition>& Cordis, char* leftimg, char* rightimg, int version);
	int receiveReconstructImages(int comm, std::vector<char> &leftimg, std::vector<char> &rightimg);

	//read firmware version
	std::vector<int> readFirmwareVersion();

	//send command to device
	int sendCommand(std::string command) const;
	int sendAN5642Command(char* command) const;

	//display area on the images
	void drawArea(short width, short height, short leftX, short leftY, short rightX, short rightY);
	void hideArea();

	//check ingormation
	void systemCheck(std::string& warnMsg);
	void messageCheck(std::string& msg);

	//close connection
	int closeUDP();

private:
	bool autoExposure;

	bool areaDisplay;
	std::vector<int> displayArea;

	int reconstructAreaWidth;
	int reconstructAreaHeight;

	UdpConnection* connection_;
	SystemCRC* crcValidator_;
	UdpReader reader;
};
#endif TRACKING_HPP
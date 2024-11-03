#ifndef RRTC_H
#define RRTC_H

#include "iostream"

// eigen
#include "Eigen/Core"
#include "Eigen/SVD"
#include "Eigen/Geometry"
#include "Eigen/StdVector"

// vtk
//#include <vtk-8.1/vtkPoints.h>
//#include <vtk-8.1/vtkSmartPointer.h>
//#include "vtk-8.1/vtkMatrix4x4.h"

#define d2r 57.2957795130

class RRTC	// Robot Registration and Tool Calibration class
{
public:
	bool hasRegistrationPara;
	Eigen::Matrix3d Ro;
	Eigen::Vector3d To;
	Eigen::Vector3d Tcp;
	double RobotRegistrationRMS;
	double ToolCalibrationRMS;
	std::vector<double> MatricToAngle_xyz(const double matrix3x3[3][3]);
	// initialize needed before start robot registration / tool calibration every time, Verify NO need!
	void init();
	
	// t_num: the number of robot translational-motion(keep the robot flange pose(rx,ry,rz) has no
	// changed) points-group(three points:robot flange position&robot flange rotation&tool-marker position from vega),
	// t_num need >=4 && <=10; then the rest points-group are robot translational&rotational motion,points-group number need >=3 && <=10
	void setInitialPoints(std::vector<Eigen::Vector3d> p, int t_num);
	
	// same as function setInitialPoints(std::vector<Eigen::Vector3d> p, int t_num); t_num >=1 && <=3
	// keep the SAME robot flange pose(rx,ry,rz) with the robot translational-motion points-group in function setInitialPoints()
	// the rest robot translational&rotational motion points-group number need >=1 && <=3
	void setVerifyPoints(std::vector<Eigen::Vector3d> p, int t_num);
	
	// set tcp data
	void setTcpdata(double x, double y, double z);
	
	// calculate Ro To TCP RMS
	void calculateResult();
	
	// update Ro To TCP RMS
	void updateCalculateResult();

	// calculate Robot Rxyz
	Eigen::Matrix3d calculateR(Eigen::Vector3d rxyz);

	// given the target position in NDI-vega Coordinate, calculate the target position in Robot-base Coordinate which need the robot to move to.
	// !use this function need flag:hasRegistrationPara to be true
	Eigen::Vector3d calculateRobotPositionInRobotbase(Eigen::Vector3d targetPositionInNDI, Eigen::Vector3d targetPoseInRobotbase);

	// given the current position in Robot-base Coordinate, calculate the current position in NDI-vega Coordinate realtime.
	// !use this function need flag:hasRegistrationPara to be true
	Eigen::Vector3d calculateRobotPositionInNDIvega(Eigen::Vector3d currentPositionInRobotbase, Eigen::Vector3d currentPoseInRobotbase);

private:
	bool hasInitialPoints;
	bool hasVerifyPoints;
	bool hasSetTcpdata;
	int  verify_t_num;
	Eigen::Matrix3f R;

	std::vector<Eigen::Vector3d> Pr_p;			// points position at robot base
	std::vector<Eigen::Vector3d> Pr_r;			// points rotation at robot base
	std::vector<Eigen::Vector3d> Pv_p;			// tool marker position at vega base
	std::vector<Eigen::Vector3d> Tcpr_p;
	std::vector<Eigen::Vector3d> Tcpr_r;
	std::vector<Eigen::Vector3d> Tcpv_p;
	std::vector<Eigen::Vector3d> Verifyr_p;
	std::vector<Eigen::Vector3d> Verifyr_r;
	std::vector<Eigen::Vector3d> Verifyv_p;
	std::vector<Eigen::Matrix3d> R4r;
	std::vector<Eigen::Matrix3d> R4tcp;
	std::vector<Eigen::Matrix3d> R4verify;
	
	Eigen::Matrix<float, 3, 700> AMax;			// support robot translational-motion points-group number 20 at most
	Eigen::Matrix<float, 3, 700> BMax;	

	void InitialTestPoints();
	void calculateRX(bool isVerifyflag);
	void calculateR0();
	void calculateAMax();
	void calculateBMax();
	void calculateTcp();
	void calculateT();
	//void calculateRobotRegistrationRMS();
	void calculateToolCalibrationRMS();
	void incrementHandle();
	int  calculateRowCol(int p);
};
#endif //RRTC_H

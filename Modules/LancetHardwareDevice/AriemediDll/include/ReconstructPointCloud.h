#ifndef RECONSTRUCTPOINTCLOUD_H
#define RECONSTRUCTPOINTCLOUD_H

#ifdef ARMDCOMBINEDAPI_EXPORTS
#define ARMDCOMBINED_API __declspec(dllexport)
#else
#define ARMDCOMBINED_API __declspec(dllimport)
#endif

#include <vector>
#include <iostream>
#include <fstream>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

class ReconstructPointCloud
{
public:
	ReconstructPointCloud(std::vector<double> infor);
	~ReconstructPointCloud();

	/**
	* @brief: input data for reconstruction
	* @param: data for reconstruction
	*/
	void setReconstructData(std::vector<std::vector<char>> data);

	/**
	* @brief: get reconstruct pointcloud
	* @return: pointcloud
	*/
	std::vector<std::vector<float>> getPointCloud();

	/**
	* @brief: save reconstructed pointcloud
	* @param1: save path
	* @param2: pointcloud with the float type
	* @return: save success(true) or failure(false)
	*/
	bool savePLY(std::string filename, std::vector<std::vector<float>> cloud);

	/**
	* @brief: save reconstructed pointcloud
	* @param1: save path
	* @param2: pointcloud with the cv::Point3f type
	* @return: save success(true) or failure(false)
	*/
	bool savePLY(std::string filename, std::vector<cv::Point3f> cloud);

private:
	cv::Mat
		KK_L, KK_R,
		RadialDistortion_L, RadialDistortion_R,
		TangentialDistortion_L, TangentialDistortion_R,
		R, T, E, F, error, Dist_L, Dist_R;

	cv::Mat rmap_L[2], rmap_R[2];
	cv::Mat R_L, R_R, P_L, P_R;
	cv::Rect validROI_L, validROI_R;
	cv::Mat Q;
	cv::Size imageSize, winSize;
	int wh;
	float pha_dif;
	double correctMat[16];
	double epiLength;

	cv::Mat m_maskLeft;
	cv::Mat m_maskRight;
	std::vector<cv::Mat> m_imgL;
	std::vector<cv::Mat> m_imgR;

	//reconstruct method
	bool rectify(cv::Mat& L, cv::Mat& R, cv::Mat& L_des, cv::Mat& R_des, cv::Mat& L_mask, cv::Mat& R_mask, cv::Rect& L_rect, cv::Rect& R_rect);
	bool init(int win_size, float pha_dif);
	bool phaseMatch(cv::Mat& phaL, cv::Mat& phaR, std::vector<std::pair<cv::Point2f, cv::Point2f>>& cps);
	int phaseSearch(cv::Mat& BOXA_L, cv::Mat& phaR, int Y_R, int X_R_Start);
	double searchSubPixel(cv::Mat& BOX_L, cv::Mat& phaR, int XR, int YR);
	bool calcDistance(cv::Mat& dif_map, cv::Mat& depth_map, std::vector<std::pair<cv::Point2f, cv::Point2f>>& cps, cv::Mat Q, cv::Rect rectL, cv::Rect rectR);
};
#endif // RECONSTRUCTPOINTCLOUD_H
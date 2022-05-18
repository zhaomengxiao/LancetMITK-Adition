#include "leastsquaresfit.h"
#include <iostream>
#include <Eigen/Eigen>

namespace lancetAlgorithm
{
	void helloLib()
	{
		std::cout << "hello from Lancet Algorithm lib" << std::endl;
	}

	void lancetAlgorithm::fit_circle_2d(std::vector<double>& inp_x, std::vector<double>& inp_y, double& outp_Cx,
		double& outp_Cy, double& outp_R)
	{
		if (inp_x.size() != inp_y.size())
		{
			std::cout << "fit_circle err: input x y must have same length" << std::endl;
			return;
		}

		//建立矩阵
		int length = inp_x.size();
		Eigen::MatrixXd A;
		Eigen::VectorXd b;
		Eigen::Vector3d C;
		A.resize(length, 3);
		b.resize(length);
		C.resize(3);

		//input A b
		for (int i = 0; i < length; i++)
		{
			A(i, 0) = inp_x[i];
			A(i, 1) = inp_y[i];
			A(i, 2) = 1;

			b(i) = pow(inp_x[i], 2) + pow(inp_y[i], 2);
		}

		C = (A.transpose() * A).ldlt().solve(A.transpose() * b);
		outp_Cx = C[0] / 2;
		outp_Cy = C[1] / 2;
		outp_R = sqrt(C[2] + pow(outp_Cx, 2) + pow(outp_Cy, 2));
	}


	bool fit_circle_3d(std::vector<double>& inp_pointset, std::array<double, 3>& outp_center, double& outp_radius,
		std::array<double, 3>& outp_normal)
	{
		auto len = inp_pointset.size() / 3;
		if (len < 3)
		{
			return false;
		}
		//Store input point set into a 3xN eigen Matrix, N is the size of input point set
		Eigen::MatrixXd pointSet;
		pointSet.resize(3, len);
		pointSet.setZero();

		for (int i = 0; i < len; i++)
		{
			pointSet(0, i) = inp_pointset[3 * i + 0];
			pointSet(1, i) = inp_pointset[3 * i + 1];
			pointSet(2, i) = inp_pointset[3 * i + 2];
		}

		//Centralize data
		auto p_mean = pointSet.rowwise().mean().eval();
		pointSet = pointSet.colwise() - p_mean;


		//Using SVD to get local coordinate, Which can describe the attitude of the point set
		//Rg2l = U
		Eigen::JacobiSVD<Eigen::MatrixXd> svd(pointSet, Eigen::ComputeThinU | Eigen::ComputeThinV);
		Eigen::Matrix3d Rg2l = svd.matrixU();
		//auto V = svd.matrixV();
		//auto A = svd.singularValues();


		/*Eigen::Vector3d local_x = U.col(0);
		Eigen::Vector3d local_y = U.col(1);
		Eigen::Vector3d local_z = U.col(2);

		Eigen::Matrix3d Rg2l;
		Rg2l << local_x[0], local_y[0], local_z[0],
				local_x[1], local_y[1], local_z[1],
				local_x[2], local_y[2], local_z[2];*/


		//calculate point position in local coordinates; P_local = Rl2g * P_global = Rg2l.transpose() * P_global
		pointSet = Rg2l.transpose().eval() * pointSet;


		//Using the points in local coordinates xy plane to fit the circle, ignore the z-axis information
		std::vector<double> x;
		std::vector<double> y;

		for (int i = 0; i < pointSet.cols(); i++)
		{
			x.push_back(pointSet.col(i)[0]);
			y.push_back(pointSet.col(i)[1]);
		}

		double cx{ 0 }, cy{ 0 }, r{ 0 };
		fit_circle_2d(x, y, cx, cy, r);

		//Convert the circle center back to the global coordinates;
		Eigen::Vector3d center;
		center << cx, cy, 0;
		center = Rg2l * center;

		center = center + p_mean;

		//output
		outp_center[0] = center[0];
		outp_center[1] = center[1];
		outp_center[2] = center[2];

		outp_radius = r;

		outp_normal[0] = Rg2l(0, 2);
		outp_normal[1] = Rg2l(1, 2);
		outp_normal[2] = Rg2l(2, 2);

		return true;
	}


	bool fit_sphere(std::vector<double>& inp_x, std::vector<double>& inp_y, std::vector<double>& inp_z,
		double& outp_cx, double& outp_cy, double& outp_cz, double& outp_R)
	{
		if (inp_x.size() != inp_y.size() || inp_x.size() != inp_z.size() || inp_y.size() != inp_z.size())
		{
			std::cout << "fit_circle err: input x y must have same length" << std::endl;
			return false;
		}

		//建立矩阵
		int length = inp_x.size();
		Eigen::MatrixXd A;
		Eigen::VectorXd b;
		Eigen::Vector4d C;
		A.resize(length, 4);
		b.resize(length);
		C.resize(4);

		//input A b
		for (int i = 0; i < length; i++)
		{
			A(i, 0) = inp_x[i];
			A(i, 1) = inp_y[i];
			A(i, 2) = inp_z[i];
			A(i, 3) = 1;

			b(i) = pow(inp_x[i], 2) + pow(inp_y[i], 2) + pow(inp_z[i], 2);
		}

		C = (A.transpose() * A).ldlt().solve(A.transpose() * b);
		outp_cx = C[0] / 2;
		outp_cy = C[1] / 2;
		outp_cz = C[2] / 2;
		outp_R = sqrt(C[3] + pow(outp_cx, 2) + pow(outp_cy, 2) + pow(outp_cz, 2));

		return true;
	}

	bool fit_sphere(std::vector<double>& inp_pSet, std::array<double, 3>& outp_center, double& outp_r)
	{
		auto length = inp_pSet.size() / 3;
		if (inp_pSet.size() % 3 != 0)
		{
			std::cout << "fit_sphere err: input point set must be Multiple of 3" << std::endl;
			return false;
		}

		Eigen::MatrixXd A;
		Eigen::VectorXd b;
		Eigen::Vector4d C;
		A.resize(length, 4);
		b.resize(length);
		C.resize(4);

		//input A b
		for (int i = 0; i < length; i++)
		{
			A(i, 0) = inp_pSet[0 + 3 * i];
			A(i, 1) = inp_pSet[1 + 3 * i];
			A(i, 2) = inp_pSet[2 + 3 * i];
			A(i, 3) = 1;

			b(i) = pow(inp_pSet[0 + 3 * i], 2) + pow(inp_pSet[1 + 3 * i], 2) + pow(inp_pSet[2 + 3 * i], 2);
		}

		C = (A.transpose() * A).ldlt().solve(A.transpose() * b);
		outp_center[0] = C[0] / 2;
		outp_center[1] = C[1] / 2;
		outp_center[2] = C[2] / 2;

		outp_r = sqrt(C[3] + pow(outp_center[0], 2) + pow(outp_center[1], 2) + pow(outp_center[2], 2));

		return true;
	}

	bool fit_sphere(std::vector<std::array<double, 3>>& inp_pSet, std::array<double, 3>& outp_center, double& outp_r)
	{
		auto length = inp_pSet.size();
		if (inp_pSet.size() < 4)
		{
			std::cout << "fit_sphere err: not enough points" << std::endl;
			return false;
		}

		Eigen::MatrixXd A;
		Eigen::VectorXd b;
		Eigen::Vector4d C;
		A.resize(length, 4);
		b.resize(length);
		C.resize(4);

		//input A b
		for (int i = 0; i < length; i++)
		{
			A(i, 0) = inp_pSet[i][0];
			A(i, 1) = inp_pSet[i][1];
			A(i, 2) = inp_pSet[i][2];
			A(i, 3) = 1;

			b(i) = pow(inp_pSet[i][0], 2) + pow(inp_pSet[i][1], 2) + pow(inp_pSet[i][2], 2);
		}

		C = (A.transpose() * A).ldlt().solve(A.transpose() * b);
		outp_center[0] = C[0] / 2;
		outp_center[1] = C[1] / 2;
		outp_center[2] = C[2] / 2;

		outp_r = sqrt(C[3] + pow(outp_center[0], 2) + pow(outp_center[1], 2) + pow(outp_center[2], 2));

		return true;
	}

	bool fit_sphere_fixR(const std::vector<double>& inp_x, const std::vector<double>& inp_y, const std::vector<double>& inp_z, const double inp_r,
	                     double& outp_cx, double& outp_cy, double& outp_cz)
	{
		if (inp_x.size() != inp_y.size() || inp_x.size() != inp_z.size() || inp_y.size() != inp_z.size())
		{
			std::cout << "fit_circle err: input x y must have same length" << std::endl;
			return false;
		}

		int length = inp_x.size();
		Eigen::MatrixXd A;
		Eigen::VectorXd b;
		Eigen::Vector4d C;
		A.resize(length, 4);
		b.resize(length);
		C.resize(4);

		//input A b
		for (int i = 0; i < length; i++)
		{
			A(i, 0) = inp_x[i];
			A(i, 1) = inp_y[i];
			A(i, 2) = inp_z[i];
			A(i, 3) = 1;

			b(i) = pow(inp_x[i], 2) + pow(inp_y[i], 2) + pow(inp_z[i], 2) - pow(inp_r, 2);
		}

		C = (A.transpose() * A).ldlt().solve(A.transpose() * b);
		outp_cx = C[0] / 2;
		outp_cy = C[1] / 2;
		outp_cz = C[2] / 2;

		return true;
	}

	bool fit_plane(const std::vector<double>& inp_pSet, std::array<double, 3>& outp_center, std::array<double, 3>& outp_normal)
	{
		auto len = inp_pSet.size() / 3;
		if (len < 3)
		{
			return false;
		}
		//Store input point set into a 3xN eigen Matrix, N is the size of input point set
		Eigen::MatrixXd pointSet;
		pointSet.resize(3, len);
		pointSet.setZero();

		for (int i = 0; i < len; i++)
		{
			pointSet(0, i) = inp_pSet[3 * i + 0];
			pointSet(1, i) = inp_pSet[3 * i + 1];
			pointSet(2, i) = inp_pSet[3 * i + 2];
		}

		//Centralize data
		auto p_mean = pointSet.rowwise().mean().eval();
		pointSet = pointSet.colwise() - p_mean;


		//Using SVD to get local coordinate, Which can describe the attitude of the point set
		//Rg2l = U
		Eigen::JacobiSVD<Eigen::MatrixXd> svd(pointSet, Eigen::ComputeThinU | Eigen::ComputeThinV);
		Eigen::Matrix3d Rg2l = svd.matrixU();

		outp_center[0] = p_mean[0];
		outp_center[1] = p_mean[1];
		outp_center[2] = p_mean[2];

		outp_normal[0] = Rg2l(0, 2);
		outp_normal[1] = Rg2l(1, 2);
		outp_normal[2] = Rg2l(2, 2);

		return true;
	}

	bool fit_rectangle(const std::vector<double>& inp_pSet,
		std::array<double, 3>& outp_center,
		std::array<double, 3>& outp_normal,
		std::array<double, 3>& outp_x,
		std::array<double, 3>& outp_y,
    double &length,
    double &width)
  {
	  auto len = inp_pSet.size() / 3;
	  if (len < 3)
	  {
		  return false;
	  }
	  //Store input point set into a 3xN eigen Matrix, N is the size of input point set
	  Eigen::MatrixXd pointSet;
	  pointSet.resize(3, len);
	  pointSet.setZero();

	  for (int i = 0; i < len; i++)
	  {
		  pointSet(0, i) = inp_pSet[3 * i + 0];
		  pointSet(1, i) = inp_pSet[3 * i + 1];
		  pointSet(2, i) = inp_pSet[3 * i + 2];
	  }

	  //Centralize data
	  auto p_mean = pointSet.rowwise().mean().eval();
	  pointSet = pointSet.colwise() - p_mean;


	  //Using SVD to get local coordinate, Which can describe the attitude of the point set
	  //Rg2l = U
	  Eigen::JacobiSVD<Eigen::MatrixXd> svd(pointSet, Eigen::ComputeThinU | Eigen::ComputeThinV);
	  Eigen::Matrix3d Rg2l = svd.matrixU();

	  outp_center[0] = p_mean[0];
	  outp_center[1] = p_mean[1];
	  outp_center[2] = p_mean[2];

	  outp_normal[0] = Rg2l(0, 2);
	  outp_normal[1] = Rg2l(1, 2);
	  outp_normal[2] = Rg2l(2, 2);

	  outp_x[0] = Rg2l(0, 0);
	  outp_x[1] = Rg2l(1, 0);
	  outp_x[2] = Rg2l(2, 0);

	  outp_y[0] = Rg2l(0, 1);
	  outp_y[1] = Rg2l(1, 1);
	  outp_y[2] = Rg2l(2, 1);

	  //calculate point position in local coordinates; P_local = Rl2g * P_global = Rg2l.transpose() * P_global
	  pointSet = Rg2l.transpose().eval() * pointSet;


	  //Using the points in local coordinates xy plane to cal width and length, ignore the z-axis information
	  //std::vector<double> x;
	  //std::vector<double> y;

	  for (int i = 0; i < pointSet.cols(); i++)
	  {
		  //x.push_back(pointSet.col(i)[0]);
		  //y.push_back(pointSet.col(i)[1]);
		  length = pointSet.row(0).maxCoeff() - pointSet.row(0).minCoeff();
		  width = pointSet.row(1).maxCoeff() - pointSet.row(1).minCoeff();
	  }

	  return true;
  }
}

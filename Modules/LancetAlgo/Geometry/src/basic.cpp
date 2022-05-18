#include "Eigen/Eigen"
#include "basic.h"
#define ppoint 0
#define pdir 1

namespace lancetAlgorithm
{
	std::array<double, 3> GetMidPoint(std::array<double, 3> point1, std::array<double, 3> point2)
	{
		Eigen::Vector3d p1, p2, pMid;
		p1 << point1[0], point1[1], point1[2];
		p2 << point2[0], point2[1], point2[2];

		pMid =( p1 + p2 ) / 2;

		return std::array<double, 3> { pMid[0],pMid[1] ,pMid[2] };
	}

	std::array<double, 3> Normalize(std::array<double, 3> inp_v)
	{
		Eigen::Vector3d v{inp_v.data()};
		v.normalize();

		return std::array<double, 3>{v[0], v[1], v[2]};
	}

	double DistanceOfTwoPoints(const double p1[3], const double p2[3])
	{
		return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2) + pow(p1[2] - p2[2], 2));
	}

	double DistanceFromPointToLine1(const double PointOutsideLine[3], const double Point1OnLine[3], const double Point2OnLine[3])
	{
		Eigen::Vector3d xp1, p1p2;
		double proj;

		for (int i = 0; i < 3; i++)
		{
			xp1[i] = PointOutsideLine[i] - Point1OnLine[i];
			p1p2[i] = Point2OnLine[i] - Point1OnLine[i];
		}

		if (p1p2.norm() != 0.0)
			p1p2.normalize();
		else
		{
			return xp1.norm();
		}

		//calculate the dot product
		proj = xp1.dot(p1p2);
		return sqrt(xp1.dot(xp1) - proj * proj);
	}

	double DistanceFromPointToLine1(const double PointOutsideLine[3], const double Point1OnLine[3], const double Point2OnLine[3], double *fop)
	{
		Eigen::Vector3d xp1, p1p2;
		double num;

		for (int i = 0; i < 3; i++)
		{
			xp1[i] = PointOutsideLine[i] - Point1OnLine[i];
			p1p2[i] = Point2OnLine[i] - Point1OnLine[i];
		}
		
		if (p1p2.norm() != 0.0)
		{
			p1p2.normalize();
			num = xp1.dot(p1p2);
		}
		else
			num = 0.0;
		for (int i = 0; i < 3; i++)
		{
			fop[i] = num * p1p2[i] + Point1OnLine[i];
		}

		return sqrt(pow(fop[0] - PointOutsideLine[0], 2) + pow(fop[1] - PointOutsideLine[1], 2) + pow(fop[2] - PointOutsideLine[2], 2));
	}

	double DistanceFromPointToLine2(const double PointOutsideLine[3], const double PointOnLine[3], const double direction[3])
	{
		Eigen::Vector3d xp1, p1p2;
		double proj;

		for (int i = 0; i < 3; i++)
		{
			xp1[i] = PointOutsideLine[i] - PointOnLine[i];
			p1p2[i] = direction[i];
		}

		if (p1p2.norm() != 0.0)
			p1p2.normalize();
		else
		{
			return xp1.norm();
		}

		//calculate the dot product
		proj = xp1.dot(p1p2);
		return sqrt(xp1.dot(xp1) - proj * proj);
	}


	double DistanceFromPointToLine2(const double PointOutsideLine[3], const double PointOnLine[3], const double direction[3], double * fop)
	{
		Eigen::Vector3d xp1, p1p2;
		double num;

		for (int i = 0; i < 3; i++)
		{
			xp1[i] = PointOutsideLine[i] - PointOnLine[i];
			p1p2[i] = direction[i];
		}

		if (p1p2.norm() != 0.0)
		{
			p1p2.normalize();
			num = xp1.dot(p1p2);
		}
		else
			num = 0.0;
		for (int i = 0; i < 3; i++)
		{
			fop[i] = num * p1p2[i] + PointOnLine[i];
		}

		return sqrt(pow(fop[0] - PointOutsideLine[0], 2) + pow(fop[1] - PointOutsideLine[1], 2) + pow(fop[2] - PointOutsideLine[2], 2));
	}

	double DistanceFromPointToPlane(const double x[3], const double n[3], const double p0[3])
	{
		return abs(n[0] * (x[0] - p0[0]) + n[1] * (x[1] - p0[1]) + n[2] * (x[2] - p0[2])) / sqrt(pow(n[0], 2) + pow(n[1], 2) + pow(n[2], 2));
	}

	double AngleBetween2Vector(const double vec1[3], const double vec2[3], bool radian)
	{
		Eigen::Vector3d v1(vec1);
		Eigen::Vector3d v2(vec2);
		auto cross = v1.cross(v2);

		if (radian)
		{
			return atan2(cross.norm(), v1.dot(v2));
		}
		else
		{
			auto DegreesFromRadians = [](double x) -> double { return x * 57.2957795131; };
			double degreeAngle = DegreesFromRadians(atan2(cross.norm(), v1.dot(v2)));
			return degreeAngle;
		}
	}
	double AngleBetween2Vector(const double vec1[3], const double vec2[3], const double normal[3])
	{
		Eigen::Vector3d v1(vec1);
		Eigen::Vector3d v2(vec2);
		Eigen::Vector3d norm(normal);
		auto cross = v1.cross(v2);
		if (cross.dot(norm) >= 0)
			return atan2(cross.norm(), v1.dot(v2));
		else
			return -atan2(cross.norm(), v1.dot(v2));
	}
	double AngleBetweenLineAndPlane(double *p1, double *p2, double *normal, bool radian )
	{
		auto DegreesFromRadians = [](double x) -> double { return x * 57.2957795131; };
		auto RadiansFromDegrees = [](double x) -> double { return x * 0.017453292; };

		auto AngleBetweenVectors = [](Eigen::Vector3d &v1, Eigen::Vector3d &v2)
		{
			auto cross = v1.cross(v2);
			return atan2(cross.norm(), v1.dot(v2));
		};

		Eigen::Vector3d LineVector(p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]);
		Eigen::Vector3d PlaneNormal(normal);

		//平行的时候，向量方向相同则为0， 相反则为180
		auto radianAngle = AngleBetweenVectors(LineVector, PlaneNormal);
		double degreeAngle = DegreesFromRadians(radianAngle);
		double result;
		double diff = fabs(degreeAngle - 90);
		if (diff < 1e-6)
		{
			//直线与平面平行
			return 0;
		}
		else
		{
			if (degreeAngle > 90)
			{
				result = degreeAngle - 90;
			}
			else
			{
				result = 90 - degreeAngle;
			}
		}
		if (radian)
		{
			return RadiansFromDegrees(result);
		}
		return result;
	}

	double DirectedAngleBetweenVectorAndPlane(double * vector, double * normal, bool radian)
	{
		auto DegreesFromRadians = [](double x) -> double { return x * 57.2957795131; };
		auto RadiansFromDegrees = [](double x) -> double { return x * 0.017453292; };

		auto AngleBetweenVectors = [](Eigen::Vector3d &v1, Eigen::Vector3d &v2)
		{
			auto cross = v1.cross(v2);
			return atan2(cross.norm(), v1.dot(v2));
		};

		Eigen::Vector3d LineVector(vector);
		Eigen::Vector3d PlaneNormal(normal);

		//平行的时候，向量方向相同则为0， 相反则为180
		auto radianAngle = AngleBetweenVectors(LineVector, PlaneNormal);
		double degreeAngle = DegreesFromRadians(radianAngle);
		double result;
		double diff = fabs(degreeAngle - 90);//
		if (diff < 1e-6)
		{
			//直线与平面平行
			return 0;
		}
		else
		{
			if (degreeAngle > 90)
			{
				result = degreeAngle - 90;
			}
			else
			{
				result = 90 - degreeAngle;
			}
		}
		if (LineVector.dot(PlaneNormal) < 0)
		{
			result = -result;
		}
		if (radian)
		{
			return RadiansFromDegrees(result);
		}
		return result;
	}

	void projectToPlane(const double x[3], const double origin[3], const double normal[3], double xproj[3])
	{
		double t, xo[3];

		xo[0] = x[0] - origin[0];
		xo[1] = x[1] - origin[1];
		xo[2] = x[2] - origin[2];

		Eigen::Vector3d nl(normal);
		nl.normalize();
		Eigen::Vector3d xv(xo);

		t = xv.dot(nl);

		xproj[0] = x[0] - t * nl[0];
		xproj[1] = x[1] - t * nl[1];
		xproj[2] = x[2] - t * nl[2];
	}

	bool IsPointOnRay(const double point[3], const double ray_source[3], double ray_direction[3], double tolerance)
	{
		double vec1[3]{ point[0] - ray_source[0],point[1] - ray_source[1] ,point[2] - ray_source[2] };

		if (AngleBetween2Vector(vec1, ray_direction) >90)
		{
			return false;
		}
		if (DistanceFromPointToLine2(point,ray_source,ray_direction)>tolerance)
		{
			return false;
		}
		return true;
	}

	void CommonVerticalVector(const double vec1[3], const double vec2[3], double vertical_vec[3])
	{
		vertical_vec[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
		vertical_vec[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
		vertical_vec[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
	}

	bool IsSameDirection(const double vec1[3], const double vec2[3])
	{
		Eigen::Vector3d vector1{ vec1 };
		Eigen::Vector3d vector2{ vec2 };

		if (vector1.dot(vector2) < 0)
			return false;
		else
			return true;
	}

	/*void RotateVectorToPlane(const double vec[3], const double rotateAxis[3], const double offsetAxis[3], double output[3])
	{
		Eigen::Vector3d OriginVec{ vec };
		Eigen::Vector3d RotateVec{ rotateAxis };

		double len = OriginVec.dot(RotateVec);
		double height = sqrt(1 - len * len);

		double RotateProject[3] = { rotateAxis[0] * len,rotateAxis[1] * len,rotateAxis[2] * len };
		double OffsetProject[3] = { offsetAxis[0] * height,offsetAxis[1] * height,offsetAxis[2] * height };

		for (int i = 0; i < 3; i++)
		{
			output[i] = RotateProject[i] + OffsetProject[i];
		}
	}*/

	double AngleBetween2Line(double *p11, double *p12, double *p21, double *p22)
	{
		double vec1[3] = { p12[0] - p11[0], p12[1] - p11[1], p12[2] - p11[2] };
		double vec2[3] = { p22[0] - p21[0], p22[1] - p21[1], p22[2] - p21[2] };

		double angle = AngleBetween2Vector(vec1, vec2);
		if (angle > 90)
		{
			return 180 - angle;
		}
		else
		{
			return angle;
		}
	}
}

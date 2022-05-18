#ifndef __BASIC_H
#define __BASIC_H


#include <vector>
#include <array>

namespace lancetAlgorithm
{
	std::array<double, 3> GetMidPoint(std::array<double, 3> point1, std::array<double, 3> point2);

	std::array<double, 3> Normalize(std::array<double, 3> inp_v);

	double DistanceOfTwoPoints(const double p1[3], const double p2[3]);

	/**
	*calculate the distance from a point to a line determined by two points and return the distance.
	 *
	 * @param PointOutsideLine [Input]The coordinates of the point outside the line .
	 * @param PointOnLine1 [Input]The coordinates of the first point which determine the line.
	 * @param PointOnLine2 [Input]The coordinates of the second point which determine the line.
	*/
	double DistanceFromPointToLine1(const double PointOutsideLine[3], const double Point1OnLine[3], const double Point2OnLine[3]);

	/**
	*calculate the distance from a point to a line determined by two points and return the distance.
	*
	* @param PointOutsideLine [Input]The coordinates of the point outside the line.
	* @param PointOnLine1 [Input]The coordinates of the first point which determine the line.
	* @param PointOnLine2 [Input]The coordinates of the second point which determine the line.
	* @param fop [Input]foot of perpendicular.To save the coordinates of the intersection between the given line and a line through the given point that vertical to it
	*/
	double DistanceFromPointToLine1(const double PointOutsideLine[3], const double Point1OnLine[3], const double Point2OnLine[3], double *fop);

	/**
	*calculate the distance from a point to a line determined by a point and its direction.Then return the distance.
	 *
	 * @param PointOutsideLine [Input]The coordinates of the point outside the line .
	 * @param PointOnLine1 [Input]The coordinates of the point which determine the line.
	 * @param direction [Input]The direction of the line.
	*/
	double DistanceFromPointToLine2(const double PointOutsideLine[3], const double PointOnLine[3], const double direction[3]);

	/**
	*calculate the distance from a point to a line determined by a point and its direction.Then return the distance.
	*
	* @param PointOutsideLine [Input]The coordinates of the point outside the line .
	* @param PointOnLine1 [Input]The coordinates of the point which determine the line.
	* @param direction [Input]The direction of the line.
	* @param fop [Input]foot of perpendicular.To save the coordinates of the intersection between the given line and a line through the given point that vertical to it
	*/
	double DistanceFromPointToLine2(const double PointOutsideLine[3], const double PointOnLine[3], const double direction[3],double *fop);

	/**
	*calculate the distance from a point to a plane determined by a point and the normal.
	*
	* @param x [Input]The coordinates of the point.
	* @param n [Input]the normal of the plane.
	* @param p0 [Input]The coordinates of the point on the plane.
	*/
	double DistanceFromPointToPlane(const double x[3], const double n[3], const double p0[3]);

	/**
	 * compute the angle between two vectors
	 *
	 * @param vec1 [Input] the first vector.
	 * @param vec2 [Input] the second vector.
	 * @param radian [Input] The angle type, default:false, return degree;set true return the radian type
	 * @return  angle between two vectors, [0, 180] degree
	 */
	double AngleBetween2Vector(const double vec1[3], const double vec2[3], bool radian = false);
	/**
	 * compute the angle between two vectors
	*
	* @param vec1 [Input] the first vector.
	* @param vec2 [Input] the second vector.
	* @param normal [Input] the positive normal of the plane(parallel to coordinate axis).
	* @return  angle between two vectors, [-180, 180] degree
	*/
	double AngleBetween2Vector(const double vec1[3], const double vec2[3], const double normal[3]);
	/**
	 * compute the angle between line and plane
	 *
	 * @param p11 [Input]The coordinates of the point in line1.
	 * @param p12 [Input]The coordinates of the point in line1.
	 * @param p21 [Input]The coordinates of the point in line2.
	 * @param p22 [Input]The coordinates of the point in line2.
	 * @return  angle between line and plane, [0, 90] degree2
	 */
	double AngleBetween2Line(double *p11, double *p12, double *p21, double *p22);
	/**
	 * compute the angle between line and plane
	 *
	 * @param p1 [Input]The coordinates of the point in line.
	 * @param p2 [Input]The coordinates of the point in line.
	 * @param normal [Input] The normal of the plane.
	 * @param radian [Input] The angle type, default:false, return degree;set true return the radian type
	 * @return  angle between line and plane, [0, 90] degree
	 */
	double AngleBetweenLineAndPlane(double *p1, double *p2, double *normal, bool radian = false);
	/**
	* compute the angle between line and plane
	*
	* @param p1 [Input]The coordinates of the start point in line.
	* @param p2 [Input]The coordinates of the end point in line.
	* @param normal [Input] The normal of the plane.
	* @param radian [Input] The angle type, default:false, return degree;set true return the radian type
	* @return  angle between line and plane, [-90, 90] degree
	*/
	double DirectedAngleBetweenVectorAndPlane(double *vector, double *normal, bool radian = false);
	/**
	 * compute the projected point of x on plane
	 *
	 * @param x [Input] The coordinates of the x point.
	 * @param origin [Input] The coordinates of Any point on the plane.
	 * @param normal [Input] The normal of the plane.
	 * @param xproj [Output] The coordinates of projected point of x on plane
	 */
	void projectToPlane(const double x[3], const double origin[3], const double normal[3], double xproj[3]);

	/**
	 * compute if a point is on a ray within tolerance
	 *
	 * @param point  [Input] The coordinates of the x point.
	 * @param ray_source [Input] The coordinates of the Ray Source Point.
	 * @param ray_direction [Input] The direction vector of the ray.
	 * @param tolerance [Input] mm,if the distance from point to the ray is smaller than tolerance 
	 *
	 * @return true, if point is on a ray
	 */
	bool IsPointOnRay(const double point[3], const double ray_source[3], double ray_direction[3],double tolerance);

	/**
	 * compute common vertical vector of two vectors
	 *
	 * @param point  [Input] The coordinates of the x point.
	 * @param ray_source [Input] The coordinates of the Ray Source Point.
	 * @param ray_direction [Input] The direction vector of the ray.
	 * @param tolerance [Input] mm,if the distance from point to the ray is smaller than tolerance
	 *
	 * @return true, if point is on a ray
	*/
	void CommonVerticalVector(const double vec1[3], const double vec2[3], double vertical_vec[3]);

	bool IsSameDirection(const double vec1[3], const double vec2[3]);

	//void RotateVectorToPlane(const double vec[3], const double rotateAxis[3], const double offsetAxis[3], double output[3]);
}
#endif

#ifndef __LEASTSQUARESFIT_H
#define __LEASTSQUARESFIT_H
#include <vector>
#include <array>

namespace lancetAlgorithm
{
	/**
	 * A simple test function to indicate the lib working well.
	 */
	void helloLib();
	/**
	 * Fitting a circle with a set of points in two dimensions.
	 *
	 * @param inp_x [Input]The x coordinates of the point set.
	 * @param inp_y [Input]The y coordinates of the point set.
	 * @param outp_Cx [Output]The x-coordinate of the center .
	 * @param outp_Cy [Output]The y-coordinate of the center .
	 * @param outp_R  [Output]The radius of the circle.
	 */
	void fit_circle_2d(std::vector<double>& inp_x, std::vector<double>& inp_y, double& outp_Cx, double& outp_Cy, double& outp_R);
	/**
	 * Fitting a circle with a set of points in Three dimensions.
	 *
	 * @param inp_pointset [Input]The x coordinates of the point set.
	 * @param outp_center  [Output]The y coordinates of the point set.
	 * @param outp_radius  [Output]The x-coordinate of the center .
	 * @param outp_normal  [Output]The y-coordinate of the center .
	 */
	bool fit_circle_3d(std::vector<double>& inp_pointset, std::array<double, 3>& outp_center, double& outp_radius,
		std::array<double, 3>& outp_normal);

	/**
	*fitting a set of at least 4 spatial points into a sphere.
	 *
	 * @param inp_x [Input]The x coordinates of the point set.
	 * @param inp_y [Input]The y coordinates of the point set.
	 * @param inp_z [Input]The z coordinates of the point set.
	 * @param outp_cx [Output]The x-coordinate of the center.
	 * @param outp_cy [Output]The y-coordinate of the center.
	 * @param outp_cz [Output]The z-coordinate of the center.
	 * @param outp_R  [Output]The radius of the sphere.
	*/
	bool fit_sphere(std::vector<double>& inp_x, std::vector<double>& inp_y, std::vector<double>& inp_z, double& outp_cx, double& outp_cy, double& outp_cz, double& outp_R);
	bool fit_sphere(std::vector<double>& inp_pSet, std::array<double, 3>& outp_center, double& outp_r);
	bool fit_sphere(std::vector<std::array<double,3>>& inp_pSet, std::array<double, 3>& outp_center, double& outp_r);
	/**
	*fitting a set of at least 4 spatial points into a sphere.
	 *
	 * @param inp_x [Input]The x coordinates of the point set.
	 * @param inp_y [Input]The y coordinates of the point set.
	 * @param inp_z [Input]The z coordinates of the point set.
	 * @param inp_r [Input]The radius of the sphere.
	 * @param outp_cx [Output]The x-coordinate of the center.
	 * @param outp_cy [Output]The y-coordinate of the center.
	 * @param outp_cz [Output]The z-coordinate of the center.
	*/

	bool fit_sphere_fixR(const std::vector<double>& inp_x, const std::vector<double>& inp_y, const std::vector<double>& inp_z, const double inp_r,
		double& outp_cx, double& outp_cy, double& outp_cz);

	bool fit_plane(const std::vector<double>& inp_pSet, std::array<double, 3>& outp_center, std::array<double, 3>& outp_normal);
	bool fit_rectangle(const std::vector<double>& inp_pSet, std::array<double, 3>& outp_center, std::array<double, 3>& outp_normal, std::array<double, 3>& outp_x,
		std::array<double, 3>& outp_y, double& length, double& width);
}




#endif

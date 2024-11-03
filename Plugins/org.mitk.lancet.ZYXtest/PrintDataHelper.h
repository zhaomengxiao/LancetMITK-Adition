#pragma once
#include <QTextBrowser>
#include <vtkMatrix4x4.h>
#include <eigen3/Eigen/Dense>
class PrintDataHelper
{
public:
	static void AppendTextBrowserMatrix(QTextBrowser* browser, const char* matrixName, const double* matrix);
	static void AppendTextBrowserMatrix(QTextBrowser* browser, const char* matrixName, const vtkMatrix4x4* matrix);
	static void AppendTextBrowserArray(QTextBrowser* browser, const char* arrayName, int size, const double* array);
	static void AppendTextBrowserArray(QTextBrowser* browser, const char* arrayName, const std::vector<double> array);
	static void AppendTextBrowserArray(QTextBrowser* browser, const Eigen::Vector3d array, const char* arrayName);

	template <std::size_t N>
	static void AppendTextBrowserArray(QTextBrowser* browser, const char* arrayName, const std::array<double, N>& arr)
	{
		QString str;
		for (const auto& element : arr)
		{
			str += QString::number(element) + " ";
		}
		browser->append(arrayName + str);
	}
};


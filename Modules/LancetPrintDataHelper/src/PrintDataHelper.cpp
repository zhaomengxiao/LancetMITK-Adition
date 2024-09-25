#include "PrintDataHelper.h"


void PrintDataHelper::AppendTextBrowserMatrix(QTextBrowser* browser, const char* matrixName, const double* matrix)
{
	browser->append(matrixName);
	for (int i = 0; i < 4; ++i)
	{
		QString row;
		for (int j = 0; j < 4; ++j)
		{
			row += QString::number(matrix[i * 4 + j]) + " ";
		}
		browser->append(row);
	}
}

void PrintDataHelper::AppendTextBrowserMatrix(QTextBrowser* browser, const char* matrixName, const vtkMatrix4x4* matrix)
{
	auto m = matrix->GetData();
	AppendTextBrowserMatrix(browser, matrixName, m);
}

void PrintDataHelper::PrintMatrix(const char* matrixName, const double* matrix)
{
	printf(matrixName);
	for (int i = 0; i < 4; ++i)
	{
		QString row;
		for (int j = 0; j < 4; ++j)
		{
			row += QString::number(matrix[i * 4 + j]) + " ";
		}
		printf(row.toStdString().c_str());
	}
}

void PrintDataHelper::PrintMatrix(const char* matrixName, const vtkMatrix4x4* matrix)
{
	auto m = matrix->GetData();
	PrintMatrix(matrixName, m);
}

void PrintDataHelper::CoutMatrix(const char* matrixName, const double* matrix)
{
	std::cout << matrixName << std::endl;
	for (int i = 0; i < 4; ++i)
	{
		QString row;
		for (int j = 0; j < 4; ++j)
		{
			row += QString::number(matrix[i * 4 + j]) + " ";
		}
		std::cout << row.toStdString() << std::endl;
	}
}

void PrintDataHelper::CoutMatrix(const char* matrixName, const vtkMatrix4x4* matrix)
{
	auto m = matrix->GetData();
	CoutMatrix(matrixName, m);
}

void PrintDataHelper::AppendTextBrowserArray(QTextBrowser* browser, const char* arrayName, int size, const double* array)
{
	QString str;
	for (int i = 0; i < size; ++i)
	{
		str += QString::number(array[i]) + " ";
	}
	str = QString(arrayName) + " " + str;
	browser->append(str);
}

void PrintDataHelper::AppendTextBrowserArray(QTextBrowser* browser, const char* arrayName, const std::vector<double> array)
{
	QString str;
	for (int i = 0; i < array.size(); ++i)
	{
		str += QString::number(array[i]) + " ";
	}
	str = QString(arrayName) + " " + str;
	browser->append(str);
}

void PrintDataHelper::AppendTextBrowserArray(QTextBrowser* browser, const Eigen::Vector3d array, const char* arrayName)
{
	AppendTextBrowserArray(browser, arrayName, 3, array.data());
}

void PrintDataHelper::CoutArray(const Eigen::Vector3d array, const char* arrayName)
{
	std::string str;
	for (int i = 0; i < array.size(); ++i)
	{
		str += std::to_string(array[i]) + " ";
	}
	cout << arrayName << " " << str << std::endl;
}

void PrintDataHelper::CoutArray(const double* array, int size, const char* arrayName)
{
	std::string str;
	for (int i = 0; i < size; ++i)
	{
		str += std::to_string(array[i]) + " ";
	}
	cout << arrayName << " " << str << std::endl;
}

void PrintDataHelper::AppentTextBrowserVector(QTextBrowser* browser, const char* vecName, const std::vector<double> vec)
{
	QString str;
	for (int i = 0; i < vec.size(); ++i)
	{
		str += QString::number(vec[i]) + " ";
	}
	str = QString(vecName) + " " + str;
	browser->append(str);
}

void PrintDataHelper::CoutVector(const std::vector<double> vec, const char* vecName)
{
	std::string str;
	for (int i = 0; i < vec.size(); ++i)
	{
		str += std::to_string(vec[i]) + " ";
	}
	cout << vecName << " " << str << std::endl;
}

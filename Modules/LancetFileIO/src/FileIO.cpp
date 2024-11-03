/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <itkShiftScaleImageFilter.h>
#include "FileIO.h"

std::vector<std::vector<double>> FileIO::ReadTextFileAsTwoDarray(std::string path)
{
	std::vector<std::vector<double>> data;
	std::string line;
	std::ifstream inputFile(path);
	while (std::getline(inputFile, line)) {
		std::istringstream iss(line);
		std::vector<double> row;
		double value;

		while (iss >> value) {
			row.push_back(value);
		}

		data.push_back(row);
	}

	inputFile.close();
	std::cout << std::fixed << std::setprecision(6);
	for (const auto& row : data) {
		for (double value : row) {
			std::cout << value << " ";
		}
		std::cout << std::endl;
	}
	return data;
}

std::vector<Eigen::Vector3d> FileIO::ReadTextFileAsPoints(std::string path)
{
	std::vector<std::vector<double>> data;
	std::vector<Eigen::Vector3d> result;
	std::string line;
	std::ifstream inputFile(path);
	while (std::getline(inputFile, line)) {
		std::istringstream iss(line);
		std::vector<double> row;
		double value;

		while (iss >> value) {
			row.push_back(value);
		}
		data.push_back(row);
	}

	inputFile.close();
	for (const auto& row : data) {
		if (row.size() >= 3) { // Ensure there are at least 3 elements in the row
			Eigen::Vector3d array(row[0], row[1], row[2]);
			result.push_back(array);
		}
	}
	return result;
}

void FileIO::ReadTextFileAsvtkMatrix(std::string path, vtkMatrix4x4* matrix)
{
	std::ifstream inputFile(path);
	if (inputFile.is_open()) {
		std::string line;
		for (int row = 0; row < 4; ++row) {
			if (std::getline(inputFile, line)) {
				std::stringstream ss(line);
				std::string token;
				for (int col = 0; col < 4; ++col) {
					if (std::getline(ss, token, ' ')) {
						matrix->SetElement(row, col, std::stod(token));
					}
					else {
						std::cerr << "Error: Missing column value at row " << row << " col " << col << std::endl;
						return;
					}
				}
			}
			else {
				std::cerr << "Error: Missing row " << row << std::endl;
				return;
			}
		}
		inputFile.close();
	}
	else {
		std::cout << "cannot open file: " << path << std::endl;
	}
}

std::string FileIO::GetLastNParts(const std::string& path, std::size_t n)
{
	std::filesystem::path p(path);
	std::filesystem::path result;
	auto it = p.end();

	for (std::size_t i = 0; i < n && it != p.begin(); ++i) {
		--it;
		result = *it / result;
	}

	return result.string();
}

std::vector<std::string> FileIO::GetPathFilesWithFileType(std::filesystem::path path, std::string fileType)
{
	std::vector<std::string> files;

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
		std::cout << "目录不存在或不是目录: " << path << std::endl;
		return files;
	}

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_regular_file() && entry.path().extension() == fileType) {
			files.push_back(entry.path().filename().string());
		}
	}
	for (const auto& fileName : files) {
		std::cout << fileName << std::endl;
	}
	return files;
}

std::vector<std::string> FileIO::GetListSubdirectories(std::string path)
{
	std::vector<std::string> result;
	try {
		// Iterate over the directory
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (entry.is_directory()) {
				std::cout << entry.path() << std::endl;
				result.push_back(entry.path().string());
				std::cout << "Directory found: " << entry.path().filename() << std::endl;
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error accessing directory: " << e.what() << std::endl;
	}
	return result;
}


std::string FileIO::GetLastCharaters(const std::filesystem::path path, size_t n)
{
	std::string filePathStr = path.string();
	if (n >= filePathStr.size()) {
		return filePathStr; // 如果 x 大于等于路径长度，返回整个路径
	}
	else {
		return filePathStr.substr(filePathStr.size() - n); // 返回末尾 X 个字符
	}
}

std::string FileIO::getFileNameWithoutExtension(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	std::string filename = path.stem().string(); // 获取没有扩展名的文件名
	return filename;
}

void FileIO::SaveMatrix2File(std::string filePath, vtkMatrix4x4* matrix)
{
	//存CameraToImage
	auto data = matrix->GetData();
	std::ofstream robotMatrixFile(filePath);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			robotMatrixFile << data[i * 4 + j];
			if (j != 3) {
				robotMatrixFile << " "; // 每个元素之间加空格
			}
		}
		robotMatrixFile << std::endl; // 每行结束后换行
	}
	robotMatrixFile.close();
	std::cout << "save matrix in " + filePath << std::endl;
}

void FileIO::WritePolyDataAsSTL(vtkPolyData* polyData, std::string fileName)
{
	vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
	stlWriter->SetInputData(polyData);

	stlWriter->SetFileName(fileName.c_str());
	stlWriter->Update();
	stlWriter->Write();
}

void FileIO::SaveDataNodeAsSTL(mitk::DataNode* aDataNode, std::string aFileName)
{
	mitk::Surface::Pointer surface = dynamic_cast<mitk::Surface*>(aDataNode->GetData());
	if (surface.IsNull())
	{
		MITK_ERROR << "DataNode does not contain a surface!";
		return;
	}
	else
	{
		try
		{
			mitk::IOUtil::Save(surface, aFileName);
			MITK_INFO << "Surface saved to " << aFileName;
		}
		catch (const mitk::Exception& e)
		{
			MITK_ERROR << "Failed to save surface to file: " << e.what();
		}
	}
}


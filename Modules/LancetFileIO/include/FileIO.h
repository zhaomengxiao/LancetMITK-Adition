/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef FILEIO_h
#define FILEIO_h
#include "MitkLancetFileIOExports.h"
#include <vtkPolyData.h>
#include <vtkSTLWriter.h>
#include <filesystem>
#include <eigen3/Eigen/Dense>
#include <vtkMatrix4x4.h>
#include <mitkDataNode.h>
#include <mitkSurface.h>
#include <mitkIOUtil.h>

class MITKLANCETFILEIO_EXPORT FileIO
{
public:
    template<typename... Args>
    static std::filesystem::path CombinePath(const std::string& first, const Args&... args)
    {
        std::filesystem::path fullPath(first);
        (fullPath /= ... /= args);
        return fullPath.lexically_normal();
    }

    static std::vector<std::vector<double>> ReadTextFileAsTwoDarray(std::string path);

    static std::vector<Eigen::Vector3d> ReadTextFileAsPoints(std::string path);

    static void ReadTextFileAsvtkMatrix(std::string path, vtkMatrix4x4* matrix);

    static std::string GetLastNParts(const std::string& path, std::size_t n);

    static std::vector<std::string> GetPathFilesWithFileType(std::filesystem::path path, std::string fileType);

    static std::vector<std::string> GetListSubdirectories(std::string path);

    static void ReadPKAImplantJson(std::filesystem::path filePath);

    static std::string GetLastCharaters(const std::filesystem::path path, size_t n);

    static std::string getFileNameWithoutExtension(const std::string& filePath);

    static void SaveMatrix2File(std::string filePath, vtkMatrix4x4* matrix);

    static void WritePolyDataAsSTL(vtkPolyData* polyData, std::string fileName);

    static void SaveDataNodeAsSTL(mitk::DataNode* aDataNode, std::string aFileName);

};
#endif


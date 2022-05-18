/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

// std includes
#include <algorithm>
#include <string>

// CTK includes
#include "mitkCommandLineParser.h"

// MITK includes
#include <leastsquaresfit.h>
#include <mitkIOUtil.h>
#include <mitkPointSet.h>



/** \brief MiniApp that read a pointSet file and return a sphere that fit
 *
 * This MiniApp provides the capability to convert a pointSet file to fit a sphere.
 * return its center and radius
 *
 * Supported file extensions are .txt and .example.
 *
 * You will need to specify an input file path. The -v flag is optional and will
 * produce additional output.
 */

//! [main]
int main(int argc, char *argv[])
{
  //! [main]

  //! [create parser]
  mitkCommandLineParser parser;

  // set general information about your MiniApp
  parser.setCategory("LancetAlgorithm-Geometry");
  parser.setTitle("Fit Sphere");
  parser.setDescription("MiniApp that read a pointSet file and return a sphere that fit.");
  parser.setContributor("Lancet Algorithm");
  //! [create parser]

  //! [add arguments]
  // how should arguments be prefixed
  parser.setArgumentPrefix("--", "-");
  // add each argument, unless specified otherwise each argument is optional
  // see mitkCommandLineParser::addArgument for more information
  parser.beginGroup(" I/O parameters");
  parser.addArgument(
    "input", "i", mitkCommandLineParser::File, "Input file", "input file (.mps)", us::Any(), false, false, false, mitkCommandLineParser::Input);
  parser.addArgument("output",
                     "o",
                     mitkCommandLineParser::File,
                     "Output file",
                     "where to save the output (.mps)",
                     us::Any(),
                     true, false, false, mitkCommandLineParser::Output);
  parser.endGroup();

  parser.beginGroup("Optional parameters");
  parser.addArgument(
    "verbose", "v", mitkCommandLineParser::Bool, "Verbose Output", "Whether to produce verbose output");
  parser.endGroup();
  //! [add arguments]

  //! [parse the arguments]
  // parse arguments, this method returns a mapping of long argument names and their values
  std::map<std::string, us::Any> parsedArgs = parser.parseArguments(argc, argv);

  if (parsedArgs.size() == 0)
    return EXIT_FAILURE;

  // parse, cast and set required arguments
  std::string inFilename = us::any_cast<std::string>(parsedArgs["input"]);
  std::string outFileName = us::any_cast<std::string>(parsedArgs["output"]);

  // default values for optional arguments
  bool verbose(false);

  // parse, cast and set optional arguments if given
  if (parsedArgs.count("verbose"))
  {
    verbose = us::any_cast<bool>(parsedArgs["verbose"]);
  }
  //! [parse the arguments]

  //! [do processing]
  try
  {
    // verbosity in this example is slightly over the top
    if (verbose)
    {
      MITK_INFO << "Trying to read file.";
    }

    std::vector<mitk::BaseData::Pointer> inVector = mitk::IOUtil::Load(inFilename);
    if (inVector.empty())
    {
      std::string errorMessage = "File at " + inFilename + " could not be read. Aborting.";
      MITK_ERROR << errorMessage;
      return EXIT_FAILURE;
    }
    mitk::BaseData *inBaseData = inVector.at(0);
    mitk::PointSet *inPointSet = dynamic_cast<mitk::PointSet *>(inBaseData);
    if (inPointSet == nullptr)
    {
        std::string errorMessage = "File at " + inFilename + "can not read as Point Set. Aborting.";
        MITK_ERROR << errorMessage;
        return EXIT_FAILURE;
    }

    if (verbose)
    {
      MITK_INFO << "Fitting";
    }
    
    if (verbose)
    {
      MITK_INFO << "Input Point Number " << inPointSet->GetSize();
    }
    if (inPointSet->GetSize() <3)
    {
        std::string errorMessage = "Input Point Less than 3,need more";
        MITK_ERROR << errorMessage;
        return EXIT_FAILURE;
    }
    //std::transform(data.begin(), data.end(), data.begin(), ::toupper);
    std::vector<double> points;
    for (int i=0;i<inPointSet->GetSize();i++)
    {
        auto p = inPointSet->GetPoint(i);
        points.push_back(p[0]);
        points.push_back(p[1]);
        points.push_back(p[2]);
    }
    std::array<double, 3> center{};
    double radius {};
    auto st = lancetAlgorithm::fit_sphere(points, center, radius);
    if (!st)
    {
        std::string errorMessage = "fit_sphere failed";
        MITK_ERROR << errorMessage;
        return EXIT_FAILURE;
    }
    if (verbose)
    {
      MITK_INFO << "Success!" ;
    }
    MITK_INFO << "Center: " << center[0]<<","<<center[1]<<","<<center[2] << "  radius: " << radius;
    mitk::PointSet::Pointer outCenter = mitk::PointSet::New();
    outCenter->InsertPoint(0,mitk::Point3D{center.data()});

    if (verbose)
    {
      MITK_INFO << "Trying to write to file.";
    }

    mitk::IOUtil::Save(outCenter.GetPointer(), outFileName);

    return EXIT_SUCCESS;
  }
  //! [do processing]

  catch (itk::ExceptionObject& e)
  {
    MITK_ERROR << e;
    return EXIT_FAILURE;
  }
  catch (std::exception& e)
  {
    MITK_ERROR << e.what();
    return EXIT_FAILURE;
  }
  catch (...)
  {
    MITK_ERROR << "Unexpected error encountered.";
    return EXIT_FAILURE;
  }
}

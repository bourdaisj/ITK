/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkOptImageToImageMetricsTest2.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkImage.h"
#include "itkTranslationTransform.h"
#include "itkAffineTransform.h"
#include "itkRigid2DTransform.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkLinearInterpolateImageFunction.h"

#include "itkMeanSquaresImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkMutualInformationImageToImageMetric.h"

#include "itkOptImageToImageMetricsTest2.h"

int itkOptImageToImageMetricsTest2(int , char* argv[])
{
  std::cout << "OPTIMIZED ON" << std::endl;

  std::cout << "Default number of threads : "
            << itk::MultiThreader::GetGlobalDefaultNumberOfThreads()
            << std::endl;

  typedef itk::Image< unsigned int > FixedImageType;
  typedef itk::Image< unsigned int > MovingImageType;

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //  First run the experiments with the default number of threads,
  //  as set from the command line arguments, the system defaults
  //  or the ITK environment variable:
  //  ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
  std::cout << "Now Running tests with : " << std::endl;
  std::cout << "\t Global Default Number of Threads " << itk::MultiThreader::GetGlobalDefaultNumberOfThreads() << std::endl;
  std::cout << "\t Global Maximum Number of Threads " << itk::MultiThreader::GetGlobalMaximumNumberOfThreads() << std::endl;
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
  std::cout << std::endl;

  itk::BSplineLinearTest( fixedImageReader.GetPointer(),
                       movingImageReader.GetPointer() );

  return(EXIT_SUCCESS);
}

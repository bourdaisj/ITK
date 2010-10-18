/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkWeightedCentroidKdTreeGeneratorTest8.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkVector.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkListSample.h"
#include "itkKdTree.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include <fstream>

// Generate Weighed centroid Kd tree generator using FixedArray
int itkWeightedCentroidKdTreeGeneratorTest8(int argc , char * argv [] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing parameters" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " numberOfDataPoints numberOfTestPoints bucketSize [graphvizDotOutputFile]" << std::endl;
    return EXIT_FAILURE;
    }

  // Random number generator
  typedef itk::Statistics::MersenneTwisterRandomVariateGenerator NumberGeneratorType;

  NumberGeneratorType::Pointer randomNumberGenerator = NumberGeneratorType::New();
  randomNumberGenerator->Initialize();

  const unsigned int measurementVectorSize = 2;

  typedef itk::FixedArray< double, measurementVectorSize> MeasurementVectorType;

  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  SampleType::Pointer sample = SampleType::New();

  //
  // Generate a sample of random points
  //
  const unsigned int numberOfDataPoints = atoi( argv[1] );
  MeasurementVectorType mv;
  for (unsigned int i = 0; i < numberOfDataPoints; ++i )
    {
    mv[0] = randomNumberGenerator->GetNormalVariate( 0.0, 1.0 );
    mv[1] = randomNumberGenerator->GetNormalVariate( 0.0, 1.0 );
    sample->PushBack( mv );
    }

  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< SampleType > TreeGeneratorType;
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
  std::cout << treeGenerator->GetNameOfClass() << std::endl;
  treeGenerator->Print( std::cout );

  const unsigned int bucketSize = atoi( argv[3] );

  treeGenerator->SetSample( sample );
  treeGenerator->SetBucketSize( bucketSize );
  treeGenerator->Update();

  typedef TreeGeneratorType::KdTreeType TreeType;

  typedef TreeType::NearestNeighbors NeighborsType;

  typedef TreeType::KdTreeNodeType NodeType;

  TreeType::Pointer tree = treeGenerator->GetOutput();

  MeasurementVectorType queryPoint;

  unsigned int numberOfNeighbors = 1;
  TreeType::InstanceIdentifierVectorType neighbors;

  MeasurementVectorType result;
  MeasurementVectorType test_point;
  MeasurementVectorType min_point;
  min_point.Fill(0.0);

  unsigned int numberOfFailedPoints = 0;

  const unsigned int numberOfTestPoints = atoi( argv[2] );

  //
  //  Check that for every point in the sample, its closest point is itself.
  //
  typedef itk::Statistics::EuclideanDistanceMetric< MeasurementVectorType > DistanceMetricType;
  DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();
  bool testFailed = false;
  DistanceMetricType::OriginType origin;
  ::itk::NumericTraits<DistanceMetricType::OriginType>::SetLength( origin,
    measurementVectorSize);

  for( unsigned int k = 0; k < sample->Size(); k++ )
    {

    queryPoint = sample->GetMeasurementVector(k);

    for ( unsigned int i = 0; i < sample->GetMeasurementVectorSize(); ++i )
      {
      origin[i] = queryPoint[i];
      }

    distanceMetric->SetOrigin( origin );

    tree->Search( queryPoint, numberOfNeighbors, neighbors );

    for ( unsigned int i = 0; i < numberOfNeighbors; ++i )
      {
      const double distance =
        distanceMetric->Evaluate( tree->GetMeasurementVector( neighbors[i] ));

      if( distance > vnl_math::eps )
        {
        std::cout << "kd-tree knn search result:" << std::endl
                  << "query point = [" << queryPoint << "]" << std::endl
                  << "k = " << numberOfNeighbors << std::endl;
        std::cout << "measurement vector : distance" << std::endl;
        std::cout << "[" << tree->GetMeasurementVector( neighbors[i] )
                  << "] : "
                  << distance << std::endl;
        testFailed = true;
        }
      }
    }

  if( testFailed )
    {
    std::cout << "Points failed to find themselves as closest-point" << std::endl;
    }

  //
  // Generate a second sample of random points
  // and use them to query the tree
  //
  for (unsigned int j = 0; j < numberOfTestPoints; ++j )
    {

    double min_dist = itk::NumericTraits< double >::max();

    queryPoint[0] = randomNumberGenerator->GetNormalVariate( 0.0, 1.0 );
    queryPoint[1] = randomNumberGenerator->GetNormalVariate( 0.0, 1.0 );

    tree->Search( queryPoint, numberOfNeighbors, neighbors );

    //
    // The first neighbor should be the closest point.
    //
    result = tree->GetMeasurementVector( neighbors[0] );

    //
    // Compute the distance to the "presumed" nearest neighbor
    //
    double result_dist = vcl_sqrt(
          (result[0] - queryPoint[0]) *
          (result[0] - queryPoint[0]) +
          (result[1] - queryPoint[1]) *
          (result[1] - queryPoint[1])
          );

    //
    // Compute the distance to all other points, to verify
    // whether the first neighbor was the closest one or not.
    //
    for( unsigned int i = 0; i < numberOfDataPoints; ++i )
      {
      test_point = tree->GetMeasurementVector( i );

      const double dist = vcl_sqrt(
          (test_point[0] - queryPoint[0]) *
          (test_point[0] - queryPoint[0]) +
          (test_point[1] - queryPoint[1]) *
          (test_point[1] - queryPoint[1])
          );

      if( dist < min_dist )
        {
        min_dist = dist;
        min_point = test_point;
        }
      }

    if( vcl_fabs( min_dist - result_dist ) > vnl_math::eps )
      {
      std::cerr << "Problem found " << std::endl;
      std::cerr << "Query point " << queryPoint << std::endl;
      std::cerr << "Reported closest point " << result
                << " distance " << result_dist << std::endl;
      std::cerr << "Actual   closest point " << min_point
                << " distance " << min_dist << std::endl;
      std::cerr << "Difference = " << min_dist - result_dist
                << std::endl;
      std::cerr << std::endl;
      std::cerr << "Test FAILED." << std::endl;
      numberOfFailedPoints++;
      }

    }


  if( argc > 4 )
    {
    //
    // Plot out the tree structure to the console in the format used by Graphviz dot
    //
    std::ofstream plotFile;
    plotFile.open( argv[4] );
    tree->PlotTree( plotFile );
    plotFile.close();
    }


  if( numberOfFailedPoints )
    {
    std::cerr << numberOfFailedPoints << " failed out of "
              << numberOfTestPoints << " points " << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test PASSED." << std::endl;
  return EXIT_SUCCESS;
}

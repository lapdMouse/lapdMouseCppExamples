/*
Tool to create statistical measurements for labeled regions.

```bash
./imageLabelStatistics m01_AerosolSub2.vtk  m01_TerminalCompartments.nrrd
```

This command line tool reads a labelmap and an intensity image. It then
calculates for each labeled region statistical measurements including
volume, average gray-value, etc. These values will printed to the command line
in a Comma Separated Value (CSV) format.
*/

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkResampleImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkLabelStatisticsImageFilter.h>
#include <iostream>
#include <sstream>

int main(int argc, char**argv)
{
  if (argc!=3)
  {
    std::cerr << "Usage: " << argv[0] << " image labelmap" << std::endl;
    return -1;
  }

  try
  {
    // define types
    typedef itk::Image<double, 3> ImageType;
    typedef itk::Image<unsigned short, 3> LabelMapType;
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typedef itk::ImageFileReader<LabelMapType> LabelMapTypeReaderType;
    typedef itk::LabelStatisticsImageFilter<ImageType, LabelMapType> LabelStatisticsImageFilterType;

    // read intensity image
    ImageReaderType::Pointer imageReader = ImageReaderType::New();
    imageReader->SetFileName( argv[1] );
    imageReader->Update();
    ImageType::Pointer intensityImage = imageReader->GetOutput();

    // read labelmap and resample to resolution of intensity image
    LabelMapTypeReaderType::Pointer labelMapReader = LabelMapTypeReaderType::New();
    labelMapReader->SetFileName( argv[2] );
    typedef itk::ResampleImageFilter< LabelMapType, LabelMapType > ResampleFilterType;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetInput( labelMapReader->GetOutput() );
    resampler->SetOutputParametersFromImage( intensityImage );
    resampler->SetInterpolator( itk::NearestNeighborInterpolateImageFunction< LabelMapType, double >::New() );
    resampler->SetDefaultPixelValue( 0 );
    resampler->Update();
    LabelMapType::Pointer labelMap = resampler->GetOutput();

    // region statistics information
    LabelStatisticsImageFilterType::Pointer labelStatistics = LabelStatisticsImageFilterType::New();
    labelStatistics->SetInput( intensityImage );
    labelStatistics->SetLabelInput( labelMap );
    // note: the histogram is required for median calculation; it's accuracy is
    // limited to the binwidth of the histogram
    labelStatistics->SetHistogramParameters(20000, -20000, 20000);
    labelStatistics->Update();

    LabelMapType::SpacingType spacing = labelMap->GetSpacing();
    double voxelVolume = spacing[0]*spacing[1]*spacing[2];

    typedef LabelStatisticsImageFilterType::ValidLabelValuesContainerType ValidLabelValuesType;
    typedef LabelStatisticsImageFilterType::LabelPixelType                LabelPixelType;

    // print header
    std::cout << "label,volume,mean,sigma,median,min,max,count" << std::endl;

    for(ValidLabelValuesType::const_iterator vIt=labelStatistics->GetValidLabelValues().begin();
        vIt != labelStatistics->GetValidLabelValues().end(); ++vIt)
    {
      if ( labelStatistics->HasLabel(*vIt) )
      {
        LabelPixelType labelValue = *vIt;
        if (labelValue==0)
          continue;

        std::cout << labelValue << ",";
        std::cout << labelStatistics->GetCount(labelValue)*voxelVolume << ",";
        std::cout << labelStatistics->GetMean( labelValue ) << ",";
        std::cout << labelStatistics->GetSigma( labelValue ) << ",";
        std::cout << labelStatistics->GetMedian( labelValue ) << ",";
        std::cout << labelStatistics->GetMinimum( labelValue ) << ",";
        std::cout << labelStatistics->GetMaximum( labelValue ) << ",";
        std::cout << labelStatistics->GetCount( labelValue );
        std::cout << std::endl;
      }
    }

    }
    catch( itk::ExceptionObject & e )
    {
      std::cout << e << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

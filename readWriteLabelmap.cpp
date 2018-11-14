/*
Example how to read and write labelmaps used in lapdMouse project using ITK.

```bash
./readWriteLabelmap m01_NearAcini.nrrd out.nrrd
```
*/

// ITK includes
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

int main(int argc, char**argv)
{
  if (argc!=3)
  {
    std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
    return -1;
  }

  // typedef for volumetric labelmaps used in lapdMouse project
  typedef itk::Image< unsigned short, 3 > LabelmapType;

  // read labelmap
  std::string inputFilename = argv[1];
  typedef itk::ImageFileReader<LabelmapType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFilename.c_str() );
  reader->Update();
  LabelmapType::Pointer labelmap = reader->GetOutput();

  // write labelmap
  std::string outputFilename = argv[2];
  typedef itk::ImageFileWriter<LabelmapType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( labelmap );
  writer->SetFileName( outputFilename.c_str() );
  writer->SetUseCompression( true ); // labelmaps can get compressed efficiently
  writer->Update();
}

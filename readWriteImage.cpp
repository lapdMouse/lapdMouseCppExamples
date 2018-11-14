/*
Example how to read and write intensity images used in lapdMouse project using ITK.

```bash
./readWriteImage m01_AerosolSub2.mha out.mha
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
  
  // typedef for volumetric images used in lapdMouse project
  typedef itk::Image< float, 3 > ImageType;
  
  // read image
  std::string inputFilename = argv[1];
  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFilename.c_str() );
  reader->Update();
  ImageType::Pointer image = reader->GetOutput();
  
  // write image
  std::string outputFilename = argv[2];
  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( image );
  writer->SetFileName( outputFilename.c_str() );
  writer->Update();  
}


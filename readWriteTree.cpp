/*
Example how to read and write tree structures used in lapdMouse project
using ITK.

```bash
./readWriteTree m01_Tree.meta out.meta
```
*/

// ITK includes
#include <itkSpatialObject.h>
#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectWriter.h>

int main(int argc, char**argv)
{
  if (argc!=3)
  {
    std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
    return -1;
  }

  // typedef for tree structures used in lapdMouse project
  typedef itk::SpatialObject<3> SpatialObjectType;

  // read tree
  std::string inputFilename = argv[1];
  typedef itk::SpatialObjectReader<3,float> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFilename );
  reader->Update();
  SpatialObjectType::Pointer tree( reader->GetGroup() );

  // write tree
  std::string outputFilename = argv[2];
  typedef itk::SpatialObjectWriter<3,float> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( tree );
  writer->SetFileName( outputFilename.c_str() );
  writer->Update();
}

/*
Example how to read and write meshes used in lapdMouse project using ITK.

```bash
./readWriteMesh m01_AirwayOutlets.vtk out.vtk
```
*/

// ITK includes
#include <itkMesh.h>
#include <itkMeshFileReader.h>
#include <itkMeshFileWriter.h>

int main(int argc, char**argv)
{
  if (argc!=3)
  {
    std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
    return -1;
  }

  // typedef for meshes used in lapdMouse project
  typedef itk::Mesh< float, 3 > MeshType;

  // read mesh
  std::string inputFilename = argv[1];
  typedef itk::MeshFileReader<MeshType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFilename.c_str() );
  reader->Update();
  MeshType::Pointer mesh = reader->GetOutput();

  // write mesh
  std::string outputFilename = argv[2];
  typedef itk::MeshFileWriter<MeshType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( mesh );
  writer->SetFileName( outputFilename.c_str() );
  writer->Update();
}

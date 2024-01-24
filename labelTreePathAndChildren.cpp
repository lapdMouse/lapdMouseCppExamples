/*
Example showing (a) how to identify and label airway segments in a tree
structure and (b) how to link information between the AirwaySegmentsMesh.vtk and
the AirwayTree.meta.

```bash
./labelTreePathAndChildren m01_AirwaySegments.vtk m01_AirwayTree.meta 673
```

This example reads an AirwaySegmentsMesh.vtk mesh file and an AirwayTree.meta
tree structure. Based on a user specified segmentId, the program identified (a)
the set of all airway segments from the root the specified segment and (b) all
of it's child segments. Then, it assigns appropriate label values to all
associated mesh vertex point in the input mesh. The resulting labeled mesh
is output to highlightedSegmentsMesh.vtk.
*/

#include <itkMesh.h>
#include <itkMeshFileReader.h>
#include <itkMeshFileWriter.h>
#include <itkSpatialObject.h>
#include <itkSpatialObjectReader.h>
#include <set>

int main(int argc, char**argv)
{
  if (argc!=5)
  {
    std::cerr << "Usage: " << argv[0] << " airwaySegmentsMesh airwayTree segmentId highlightedSegmentsMesh" << std::endl;
    return -1;
  }

  unsigned int segmentId = atoi(argv[3]);

  // read airwaySegmentsMesh
  using MeshType = itk::Mesh< float, 3 >;
  std::string segmentMeshFilename = argv[1];
  using MeshReaderType = itk::MeshFileReader<MeshType>;
  MeshReaderType::Pointer meshReader = MeshReaderType::New();
  meshReader->SetFileName( segmentMeshFilename.c_str() );
  meshReader->Update();
  MeshType::Pointer mesh = meshReader->GetOutput();

  // read airwayTree
  using SpatialObjectType = itk::SpatialObject<3>;
  std::string treeFilename = argv[2];
  using TreeReaderType = itk::SpatialObjectReader<3,float>;
  TreeReaderType::Pointer treeReader = TreeReaderType::New();
  treeReader->SetFileName( treeFilename );
  treeReader->Update();
  SpatialObjectType::Pointer tree( treeReader->GetGroup() );

  // search for segment with user specified ID
  using TubeType = itk::TubeSpatialObject<3>;
  TubeType::Pointer selectedSegment;
  SpatialObjectType::ChildrenListType* segments = tree->GetChildren(
    SpatialObjectType::MaximumDepth, (char*)"VesselTubeSpatialObject");
  for (SpatialObjectType::ChildrenListType::iterator segmentIt = segments->begin();
    segmentIt!=segments->end(); ++segmentIt)
  {
    TubeType::Pointer segment( dynamic_cast<TubeType*>(segmentIt->GetPointer()) );
    if (segment->GetId()==segmentId)
      selectedSegment = segment;
  }
  delete segments;

  // verify that user specified segments exists; otherwise abort
  if (selectedSegment.IsNull())
  {
    std::cout << "tree does not contain segment with given id: " << segmentId << std::endl;
    return -1;
  }

  // obtain list of segments from root to user specified segment
  std::set<unsigned int> parentSegmentIds;
  TubeType::Pointer currentSegment = selectedSegment;
  while (currentSegment->GetParent()->GetNameOfClass()==currentSegment->GetNameOfClass())
  {
    TubeType::Pointer parent( dynamic_cast<TubeType*>(currentSegment->GetParent()) );
    parentSegmentIds.insert(parent->GetId());
    currentSegment = parent;
  }

  // obtain list of child segments starting from user specified segment
  std::set<unsigned int> childSegmentIds;
  SpatialObjectType::ChildrenListType* children = selectedSegment->GetChildren(
    SpatialObjectType::MaximumDepth, (char*)"VesselTubeSpatialObject");
  for (SpatialObjectType::ChildrenListType::iterator childIt = children->begin();
    childIt!=children->end(); ++childIt)
  {
    childSegmentIds.insert((*childIt)->GetId());
  }
  delete children;

  // assign labeling to mesh point data:
  // 1: user specified segment
  // 2: segments on path from root
  // 3: child segments
  // 0: other segments
  MeshType::PointDataContainer::Pointer pointData = mesh->GetPointData();
  MeshType::PointDataContainer::iterator pointDataIt = pointData->begin();
  while (pointDataIt!=pointData->end())
  {
    unsigned int currentSegmentId = (unsigned int)*pointDataIt;
    unsigned int label = 0;
    if (currentSegmentId==segmentId)
      label = 1;
    else if (parentSegmentIds.find(currentSegmentId)!=parentSegmentIds.end())
      label = 2;
    else if (childSegmentIds.find(currentSegmentId)!=childSegmentIds.end())
      label = 3;
    *pointDataIt =label;
    ++pointDataIt;
  }

  // write highlightedSegmentsMesh
  std::string outputFilename = argv[4];
  typedef itk::MeshFileWriter<MeshType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( mesh );
  writer->SetFileName( outputFilename.c_str() );
  writer->SetUseCompression( true );
  writer->Update();

  return EXIT_SUCCESS;
}

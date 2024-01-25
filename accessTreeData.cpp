/*
Example how to access data of tree structures used in lapdMouse project using ITK.

```bash
./accessTreeData m01_AirwayTree.meta
```
*/

// ITK includes
#include <itkSpatialObject.h>
#include <itkTubeSpatialObject.h>
#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectWriter.h>

int main(int argc, char**argv)
{
  if (argc!=2)
  {
    std::cerr << "Usage: " << argv[0] << " input" << std::endl;
    return -1;
  }

  // Tree structures in the lapdMouse project are represented in ITK as a
  // hierarchy of `SpatialObjects`.

  // typedef for tree structure used in lapdMouse project
  typedef itk::SpatialObject<3> SpatialObjectType;

  // They can be read in ITK from a `.meta` file
  // using `SpatialObjectReader`.

  // read tree
  std::string inputFilename = argv[1];
  using SpatialObjectType = itk::SpatialObject<3>;
  using ReaderType = itk::SpatialObjectReader<3,float>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFilename );
  reader->Update();
  SpatialObjectType::Pointer tree( reader->GetGroup() );

  // The object returned by the `SpatialObjectReader` is a
  // `GroupSpatialObjects`, which in the lapdMouse project is assigned ID 0 and
  // has exactly one immediate child
  std::cout << "tree root object type: " << tree->GetNameOfClass() << std::endl;
  std::cout << "tree root object Id: " << tree->GetId() << std::endl;
  std::cout << "tree root number of children: " << tree->GetNumberOfChildren() << std::endl;

  // This one child is represents the Trachea and is the lapdMous project
  // assigned ID 1
  SpatialObjectType::ChildrenListType* children = tree->GetChildren();
  SpatialObjectType::Pointer tracheaSO = children->begin()->GetPointer();
  std::cout << "spatial object name: " << tracheaSO->GetProperty().GetName() << std::endl;
  std::cout << "spatial object Id: " << tracheaSO->GetId() << std::endl;

  // Please note that the user is responsible for freeing the list returned by
  // GetChildren, but not the elements of the list.
  delete children;

  // The trachea's immediate subbranches in turn can be accessed in a similar
  // fashion
  SpatialObjectType::ChildrenListType* tracheaChildren = tracheaSO->GetChildren();
  SpatialObjectType::ChildrenListType::iterator segmentIt = tracheaChildren->begin(); 
  SpatialObjectType::Pointer tracheaChild1SO = segmentIt->GetPointer();
  std::cout << "child 1 name: " << tracheaChild1SO->GetProperty().GetName() << std::endl;
  std::cout << "child 1 object Id: " << tracheaChild1SO->GetId() << std::endl;
  segmentIt++; // Go to next child
  SpatialObjectType::Pointer tracheaChild2SO = segmentIt->GetPointer();
  std::cout << "child 2 child name: " << tracheaChild2SO->GetProperty().GetName() << std::endl;
  std::cout << "child 2 object Id: " << tracheaChild2SO->GetId() << std::endl;
  delete tracheaChildren;

  // Each airway segment in the lapdMouse project is stored as a
  // `TubeSpatialObject`, which is a subclass of `SpatialObject`. To
  // utilize `TubeSpatialObject`'s specific methods one needs to first
  // down cast.
  std::cout << "spatial object type: ";
  std::cout << tracheaSO->GetNameOfClass() << std::endl;
  using TubeType = itk::TubeSpatialObject<3>;
  TubeType::Pointer trachea( dynamic_cast<TubeType*>(tracheaSO.GetPointer()) );
  TubeType::Pointer tracheaChild1( dynamic_cast<TubeType*>(tracheaChild1SO.GetPointer()) );
  TubeType::Pointer tracheaChild2( dynamic_cast<TubeType*>(tracheaChild2SO.GetPointer()) );

  // Each of the segments contains a list of centerline points. The segments
  // have unqiue Ids, they may have an assigned name, and a parent.
  const TubeType::TubePointListType& points = tracheaChild1->GetPoints();
  std::cout << "number of centerline points in segment: " << tracheaChild1->GetNumberOfPoints() << std::endl;
  std::cout << "segment object Id: " << tracheaChild1->GetId() << std::endl;
  std::cout << "segment name: " << tracheaChild1->GetProperty().GetName() << std::endl;
  std::cout << "segment parent object Id: " << tracheaChild1->GetParent()->GetId() << std::endl;

  // Each centerline point of the airway segment is assigned a position,
  // radius, and tangent direction.
  for (size_t i=0; i<points.size(); ++i)
  {
    const TubeType::SpatialObjectPointType& point = points[i];
    std::cout << "Point #" << i << std::endl;
    std::cout << "Position: " << point.GetPositionInWorldSpace() << std::endl;
    std::cout << "Radius: " << point.GetRadiusInWorldSpace() << std::endl;
    std::cout << "Tangent: " << point.GetTangentInWorldSpace() << std::endl;
  }

  // Note: more information can be represented but in these classes but not utilzied in the lapdMouse project

  return EXIT_SUCCESS;

}

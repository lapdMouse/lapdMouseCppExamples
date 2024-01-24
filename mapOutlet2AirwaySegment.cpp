/*
Example showing how to link airwayOutlets to airwayTree segments.

```bash
./mapOutlet2AirwaySegment m01_AirwayOutlets.vtk m01_AirwayTree.meta
```

This example reads an AirwayOutletsMesh.vtk and obtains for each labeled outlet
region its center of gravity. Then, these are assigned to the airway segments
in AirwayTree.meta by finding the closest airway segment. The resulting mapping
of outletId to segmentId is printed to the command line in a Comma Separated
Value (CSV) format.
*/

#include <itkMesh.h>
#include <itkMeshFileReader.h>
#include <itkSpatialObject.h>
#include <itkSpatialObjectReader.h>
#include <set>

int main(int argc, char**argv)
{
  if (argc!=3)
  {
    std::cerr << "Usage: " << argv[0] << " airwayOutletsMesh airwayTree" << std::endl;
    return -1;
  }

  // read airwayOutletsMesh
  using MeshType = itk::Mesh< float, 3 >;
  std::string outletMeshFilename = argv[1];
  using MeshReaderType = itk::MeshFileReader<MeshType>;
  MeshReaderType::Pointer meshReader = MeshReaderType::New();
  meshReader->SetFileName( outletMeshFilename.c_str() );
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

  // iterate over all mesh points and find for each outlet region the set of
  // accociated points
  using PointType = MeshType::PointType;
  using MeshPointSet = std::vector<PointType>;
  using OutletPointMap = std::map<unsigned int, MeshPointSet>;
  OutletPointMap outletPoints;
  MeshType::PointsContainer::Pointer points = mesh->GetPoints();
  MeshType::PointsContainer::iterator pointIt = points->begin();
  MeshType::PointDataContainer::Pointer pointData = mesh->GetPointData();
  MeshType::PointDataContainer::iterator pointDataIt = pointData->begin();
  while (pointDataIt!=pointData->end())
  {
    unsigned int outletId = (unsigned int)*pointDataIt;
    if (outletId!=0) // a mesh value of 0 indicates airway wall
      outletPoints[outletId].push_back(*pointIt);
    ++pointIt;
    ++pointDataIt;
  }

  // for each outlet region, calculate a center point
  using OutletCenterMap = std::map<unsigned int, PointType>;
  OutletCenterMap outletCenters;
  for (OutletPointMap::const_iterator it=outletPoints.begin();
    it!=outletPoints.end(); ++it)
  {
    PointType::VectorType center; // the vector representation allows '+' and '/'' operations
    center.Fill(0);
    const MeshPointSet& points = it->second;
    for (MeshPointSet::const_iterator pIt=points.begin();
      pIt!=points.end(); ++pIt)
      center += pIt->GetVectorFromOrigin();
    center /= points.size();
    outletCenters[it->first] = (PointType)center;
  }

  // for each outlet center find the closest airway segment
  using OutletSegmentMap = std::map<unsigned int, unsigned int>;
  OutletSegmentMap outletSegmentMap;
  SpatialObjectType::ChildrenListType* segments = tree->GetChildren(
    SpatialObjectType::MaximumDepth, (char*)"VesselTubeSpatialObject");
  using TubeType = itk::TubeSpatialObject<3>;
  for (OutletCenterMap::const_iterator it=outletCenters.begin();
    it!=outletCenters.end(); ++it)
  {
    PointType outletCenter = it->second;
    double closestPointDistance = itk::NumericTraits<double>::max();
    for (SpatialObjectType::ChildrenListType::iterator segmentIt = segments->begin();
      segmentIt!=segments->end(); ++segmentIt)
    {
      TubeType::Pointer segment( dynamic_cast<TubeType*>(segmentIt->GetPointer()) );
      TubeType::TubePointListType& pointList = segment->GetPoints();
      for (TubeType::TubePointListType::iterator pointIt=pointList.begin();
        pointIt!=pointList.end(); ++pointIt)
      {
        double distance = (pointIt->GetPositionInObjectSpace()-outletCenter).GetNorm();
        if (distance<closestPointDistance)
        {
          closestPointDistance = distance;
          outletSegmentMap[it->first] = segment->GetId();
        }
      }
    }
  }
  delete segments;

  // print mapping
  std::cout << "outletId,segmentId" << std::endl;
  for (OutletSegmentMap::const_iterator it=outletSegmentMap.begin();
    it!=outletSegmentMap.end(); ++it)
    std::cout << it->first << "," << it->second << std::endl;

  return EXIT_SUCCESS;
}

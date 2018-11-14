/*
Tool to convert AirwayTree.meta into a simplified structure
AirwayTreeTable.csv, describing the airway tree as a set of connected
cylindrical elements. Resulting files can be easily read with e.g. Matlab or
Excel.

```bash
./simplfyTree m01_AirwayTree.meta m01_AirwayTreeTable.csv
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

  std::string inputFilename = argv[1];
  std::string outputFilename = argv[2];

  // read spatial objects
  typedef itk::SpatialObject<3> SpatialObjectType;
  typedef itk::SpatialObjectReader<3,float> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFilename );
  reader->Update();
  SpatialObjectType::Pointer tree( reader->GetGroup() );

  // obtain list of tree segments and store in map segmentID -> SpatialObject
  typedef itk::VesselTubeSpatialObject<3> TubeType;
  typedef std::map<int, TubeType::Pointer> SegmentMapType;
  SegmentMapType segmentMap;
  SpatialObjectType::ChildrenListType* segments = tree->GetChildren(
    SpatialObjectType::MaximumDepth, (char*)"VesselTubeSpatialObject");
  for (SpatialObjectType::ChildrenListType::iterator segmentIt = segments->begin();
    segmentIt!=segments->end(); ++segmentIt)
  {
    TubeType::Pointer segment( dynamic_cast<TubeType*>(segmentIt->GetPointer()) );
    segmentMap[segment->GetId()] = segment;
  }

  // open output file for writing
  std::ofstream outfile;
  outfile.open( outputFilename.c_str() );

  // write header
  outfile << "label,parent,length,radius,name,centroidX,"
    << "centroidY,centroidZ,directionX,directionY,directionZ"
    << std::endl;

  for (SegmentMapType::const_iterator it=segmentMap.begin();
    it!=segmentMap.end(); ++it)
  {
    TubeType::Pointer segment = it->second;

    // calculate center/radius/direction of segment based on centerline points
    TubeType::PointListType points = segment->GetPoints();
    // if parent is an airway segment, add connection point to list of the
    // current segment's points; otherwise segments with only one centerline
    // point would have a length of 0
    if (segment->GetParent()->GetNameOfClass()==segment->GetNameOfClass())
    {
      TubeType::Pointer parent(
        dynamic_cast<TubeType*>(segment->GetParent()) );
      points.insert(points.begin(),
        parent->GetPoints()[segment->GetParentPoint()]);
        //parent->GetPoints()[parent->GetNumberOfPoints()-1]);
    }
    typedef TubeType::TubePointType::PointType PointType;
    typedef TubeType::TubePointType::VectorType VectorType;
    PointType startPoint = points[0].GetPosition();
    PointType endPoint = points[points.size()-1].GetPosition();
    PointType center; center.SetToMidPoint(startPoint, endPoint);
    VectorType direction = endPoint-startPoint;
    double length = direction.GetNorm();
    direction /= length;
    double radius = 0;
    for (size_t i=0; i<points.size(); ++i)
      radius += points[i].GetRadius();
    radius /= double(points.size());

    // write segment information
    outfile << segment->GetId() << ","
      << segment->GetParent()->GetId() << ","
      << length << "," << radius << ","
      << segment->GetProperty()->GetName() << ","
      << center[0] << "," << center[1] << "," << center[2] << ","
      << direction[0] << "," << direction[1] << "," << direction[2]
      << std::endl;
  }

  outfile.close();

  return 0;
}

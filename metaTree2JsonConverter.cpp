/*
Tool to convert AirwayTree.meta into a Java Script Object Notation (JSON)
format. Resulting files can be easily read with e.g. python or matlab.

```bash
./metaTree2JsonConverter m01_AirwayTree.meta m01_AirwayTree.json
```

```python
import json
segments=json.load(open('m01_AirwayTree.json'))
len(segments)
segments[0]['ID']
segments[0]['Name']
segments[0]['Children']
```
*/

#include <itkSpatialObject.h>
#include <itkSpatialObjectReader.h>
#include <fstream>

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

  // open output file for writing
  std::ofstream outfile;
  outfile.open( outputFilename.c_str() );
  outfile << "[" << std::endl;

  // find all tree segments which utilize type VesselTubeSpatialObject
  SpatialObjectType::ChildrenListType* segments =
    tree->GetChildren(SpatialObjectType::MaximumDepth, (char*)"VesselTubeSpatialObject");

  // iterate over all segments
  SpatialObjectType::ChildrenListType::iterator segmentIt = segments->begin();
  while (segmentIt!=segments->end())
  {
    // cast spatial object to correct subtype
    typedef itk::VesselTubeSpatialObject<3> TubeType;
    TubeType::Pointer segment( dynamic_cast<TubeType*>(segmentIt->GetPointer()) );

    outfile << "  {" << std::endl;

    // output ID of segment
    outfile << "    \"ID\": " << segment->GetId() << "," << std::endl;

    // output name of segment if specified
    if (segment->GetProperty()->GetName()!="")
      outfile << "    \"Name\": \"" << segment->GetProperty()->GetName() << "\"," << std::endl;

    // output IDs of children
    SpatialObjectType::ChildrenListType* children = segment->GetChildren();//, 0, (char*)"TubeSpatialObject");
    outfile << "    \"Children\": [";
    for (SpatialObjectType::ChildrenListType::iterator it=children->begin(); it!=children->end(); ++it)
    {
      if (it!=children->begin())
        outfile << ", ";
      outfile << (*it)->GetId();
    }
    outfile << "]," << std::endl;
    delete children;

    // get list of centerline points
    TubeType::PointListType& pointList = segment->GetPoints();

    // output coordinates of centerline points
    outfile << "    \"Coordinates\": [";
    for (TubeType::PointListType::iterator it=pointList.begin(); it!=pointList.end(); ++it)
    {
      if (it!=pointList.begin())
        outfile << ", ";
      outfile << "["
        << it->GetPosition()[0] << ", "
        << it->GetPosition()[1] << ", "
        << it->GetPosition()[2] << "]";
    }
    outfile << "]," << std::endl;

    // output radii associated with centerline points
    outfile << "    \"Radii\": [";
    for (TubeType::PointListType::iterator it=pointList.begin(); it!=pointList.end(); ++it)
    {
      if (it!=pointList.begin())
        outfile << ", ";
      outfile << it->GetRadius();
    }
    outfile << "]" << std::endl;

    ++segmentIt;
    outfile << "  }" << (segmentIt!=segments->end() ? ", " : "") << std::endl;
  }
  delete segments;

  outfile << "]" << std::endl;
  outfile.close();

  return 0;
}

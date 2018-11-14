/*
Example partitioning the lung's Lobes.nrrd into disjoint compartments based the
distance to AirwayTree.meta terminal segments. The obtained
compartmentalization is output to terminalCompartments.

```bash
./partitionLobesIntoTerminalCompartments m01_Lobes.nrrd m01_AirwayTree.meta m01_TerminalCompartments.nrrd
```
*/

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkShrinkImageFilter.h>
#include <itkSpatialObject.h>
#include <itkSpatialObjectReader.h>
#include <itkPriorityQueueContainer.h>
#include <itkNeighborhoodIterator.h>

int main(int argc, char**argv)
{
  if (argc!=4)
  {
    std::cerr << "Usage: " << argv[0] << " lobes airwayTree terminalCompartments" << std::endl;
    return -1;
  }

  // read lobe labelmap and shrink it for faster processing
  typedef itk::Image< unsigned short, 3 > LabelmapType;
  typedef LabelmapType::PointType PointType;
  typedef LabelmapType::IndexType IndexType;
  std::string lobesFilename = argv[1];
  typedef itk::ImageFileReader<LabelmapType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( lobesFilename.c_str() );
  typedef itk::ShrinkImageFilter< LabelmapType, LabelmapType > ShrinkImageFilterType;
  ShrinkImageFilterType::Pointer shrinkFilter = ShrinkImageFilterType::New();
  unsigned int shrinkfactor[3] = {8,8,8};
  shrinkFilter->SetShrinkFactors( shrinkfactor );
  shrinkFilter->SetInput( reader->GetOutput() );
  shrinkFilter->Update();
  LabelmapType::Pointer lobes = shrinkFilter->GetOutput();

  // read airwayTree
  typedef itk::SpatialObject<3> SpatialObjectType;
  std::string treeFilename = argv[2];
  typedef itk::SpatialObjectReader<3,float> TreeReaderType;
  TreeReaderType::Pointer treeReader = TreeReaderType::New();
  treeReader->SetFileName( treeFilename );
  treeReader->Update();
  SpatialObjectType::Pointer tree( treeReader->GetGroup() );

  // search terminal airway segments and use their end points as seeds points
  // to partition the lobes into compartments
  typedef std::map<unsigned int, PointType> TerminalSeedMapType;
  TerminalSeedMapType terminalSeedMap;
  SpatialObjectType::ChildrenListType* segments = tree->GetChildren(
    SpatialObjectType::MaximumDepth, (char*)"VesselTubeSpatialObject");
  typedef itk::VesselTubeSpatialObject<3> TubeType;
  for (SpatialObjectType::ChildrenListType::iterator segmentIt = segments->begin();
    segmentIt!=segments->end(); ++segmentIt)
  {
    TubeType::Pointer segment( dynamic_cast<TubeType*>(segmentIt->GetPointer()) );
    if (segment->GetNumberOfChildren()==0) // terminal segment
    {
      PointType segmentEndPoint =
        segment->GetPoint(segment->GetNumberOfPoints()-1)->GetPosition();
      terminalSeedMap[segment->GetId()] = segmentEndPoint;
    }
  }
  delete segments;

  // initalize compartment image
  LabelmapType::Pointer compartments = LabelmapType::New();
  compartments->CopyInformation( lobes );
  compartments->SetRegions( lobes->GetLargestPossibleRegion() );
  compartments->Allocate();
  compartments->FillBuffer(0);

  // expand terminal compartment regions starting from seed points based on
  // their distance to the seed point using a priority queue.
  typedef std::pair<unsigned int, LabelmapType::IndexType> PQDataType;
  std::map<size_t, PQDataType> PQDataMap;
  size_t PQDataMapElementId = 0;
  std::map<unsigned int, LabelmapType::PixelType> terminalLobeMap;
  typedef itk::MinPriorityQueueElementWrapper< size_t, double, itk::IdentifierType > PQElementType;
  typedef itk::PriorityQueueContainer< PQElementType, PQElementType, double, itk::IdentifierType > PQType;
  PQType::Pointer priorityQueue = PQType::New( );

  // initialize priority queue
  for (TerminalSeedMapType::const_iterator it=terminalSeedMap.begin();
    it!=terminalSeedMap.end(); ++it)
  {
    const unsigned int terminalId = it->first;
    const PointType& seedPosition = it->second;
    IndexType index;
    PointType indexPosition;
    if (lobes->TransformPhysicalPointToIndex(seedPosition, index))
      terminalLobeMap[terminalId] = lobes->GetPixel(index);
    lobes->TransformIndexToPhysicalPoint(index, indexPosition);
    double distance = (indexPosition-seedPosition).GetNorm();
    PQDataMap[++PQDataMapElementId] = PQDataType(terminalId, index);
    priorityQueue->Push( PQElementType(PQDataMapElementId, distance) );
  }

  typedef itk::NeighborhoodIterator< LabelmapType > NeighborhoodIteratorType;
  NeighborhoodIteratorType nIterator;
  NeighborhoodIteratorType::RadiusType radius;
  radius.Fill( 1 );
  const size_t numNeighbors = 27;//3^ImageDimension;
  const size_t center = 13;//(3^ImageDimension-1)/2;
  nIterator.Initialize( radius, compartments,
    compartments->GetLargestPossibleRegion() );

  while( !priorityQueue->Empty() )
  {
    size_t pqDataId = priorityQueue->Peek().m_Element;
    PQDataType pqData = PQDataMap[pqDataId];
    PQDataMap.erase(PQDataMap.find(pqDataId));
    priorityQueue->Pop();
    if (compartments->GetPixel(pqData.second)==0)
    {
      unsigned int terminalId =  pqData.first;
      compartments->SetPixel(pqData.second, terminalId);
      const PointType terminalSeed = terminalSeedMap[terminalId];
      LabelmapType::PixelType terminalLobe = terminalLobeMap[terminalId];
      nIterator.SetLocation(pqData.second);
      for (size_t neighbor=0; neighbor<numNeighbors; ++neighbor)
      {
        if (neighbor==center) continue;
        const IndexType& nIndex = nIterator.GetIndex( neighbor );
        if (compartments->GetLargestPossibleRegion().IsInside(nIndex) &&
          compartments->GetPixel(nIndex)==0 &&
          lobes->GetPixel(nIndex)==terminalLobe)
        {
          PointType voxelCenterPoint;
          compartments->TransformIndexToPhysicalPoint(nIndex, voxelCenterPoint);
          double distance = (voxelCenterPoint-terminalSeed).GetNorm();
          PQDataMap[++PQDataMapElementId] = PQDataType(terminalId, nIndex);
          priorityQueue->Push( PQElementType(PQDataMapElementId, distance) );
        }
      }
    }
  }

  // write terminal compartment labelmap
  std::string outputFilename = argv[3];
  typedef itk::ImageFileWriter<LabelmapType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( compartments );
  writer->SetFileName( outputFilename.c_str() );
  writer->SetUseCompression( true );
  writer->Update();

  return EXIT_SUCCESS;
}

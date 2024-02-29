# lapdMouseCppExamples

**lapdMouseCppExamples** is a collection of open source software tools
and code examples that demonstrate how to work with data from the [Lung
anatomy + particle deposition (lapd) mouse
archive](https://doi.org/10.25820/9arg-9w56).  For more information
about the lapdMouse project and available data visit our archive at
[NIEHS](https://lapdmouse.iibi.uiowa.edu). More specifically, the tools
and examples demonstrate how to:

  * Load volumetric images, label maps, meshes, tree structures
  * Link information from multiple files
  * Derive new information

## Getting Started

**lapdMouseCppExamples** are C++ examples using the [Insight Segmentation and
Registration Toolkit](https://itk.org) (ITK). Developers are advised to first
familiarize themselves with C++ and the basics of ITK. We recommend the [ITK
Software Guide](https://itk.org/ITK/help/documentation.html).

**Prerequesite**: **lapdMouseCppExamples** requires installed versions of:

  * `C++ compiler` (such as VisualStudio or gcc)
  * `CMake` (https://cmake.org/)
  * `ITK` (https://itk.org)

Please install the tools suitable for your operating system. The examples have
only been tested on:

  * MacOS with Apple clang v 15.0.0 `CMake` version 3.28.1, `ITK`
    version 5.3

**Compilation of lapdMouseCppExamples**: First, download and unzip
`lapdMouseCppExamples`. Then simply configure and build the
CMake project following the usual procedure for your operating system. E.g. on
Linux/Mac:

```sh
cd lapdMouseCppExamples
mkdir build
cd build
cmake ..
make
```

**Example data**: The data utilized in the examples below can be downloaded from
the [lapdMouse Project](https://lapdmouse.iibi.uiowa.edu)


## Tools and Examples

Below follow short descriptions of the provided tools and examples. The actual
source files contain additional comments describing the usage of the data
structures and the program flow in more detail.

Examples demonstrating basic reading and writing of common data files used in
the **lapdMouse** project:

  * [`readWriteImage`](#readWriteImage)
  * [`readWriteLabelmap`](#readWriteLabelmap)
  * [`readWriteMesh`](#readWriteMesh)
  * [`readWriteTree`](#readWriteTree)

Tools and Examples demonstrating how to work with airway tree's structure stored in
`AirwayTree.meta` files:

  * [`accessTreeData`](#accessTreeData)
  * [`metaTree2JsonConverter`](#metaTree2JsonConverter)
  * [`simplifyTree`](#simplifyTree)

Tools and Examples demonstrating how to link information and derive new information

  * [`mapOutlet2AirwaySegment`](#mapOutlet2AirwaySegment)
  * [`labelTreePathAndChildren`](#labelTreePathAndChildren)
  * [`partitionLobesIntoTerminalCompartments`](#partitionLobesIntoTerminalCompartments)
  * [`imageLabelStatistics`](#imageLabelStatistics)

### readWriteImage

`readWriteImage.cpp` shows how to read and write intensity images used in the
**lapdMouse** project with proper data type.

Example usage: `./readWriteImage m01_AerosolSub2.mha out.mha`

### readWriteLabelmap

`readWriteLabelmap.cpp` shows how to read and write labelmap images used in the
**lapdMouse** project with proper data type.

Example usage: `./readWriteLabelmap m01_NearAcini.nrrd out.nrrd`

### readWriteMesh

`readWriteMesh.cpp` shows how to read and write meshes used in the
**lapdMouse** project with proper data type.

Example usage: `./readWriteMesh m01_AirwayOutlets.vtk out.vtk`

### readWriteTree

`readWriteTree.cpp` shows how to read and write tree structures used in the
**lapdMouse** project with proper data type.

Example usage: `./readWriteTree m01_Tree.meta out.meta`

### accessTreeData

`accessTreeData.cpp` shows how to access information from the tree structures
used in the **lapdMouse** project, including accessing segments, finding child
segments, identifying segment names, iterating over a segment's centerline with
their coordinates and radius information.

Example usage: `./accessTreeData m01_AirwayTree.meta`

### metaTree2JsonConverter

`metaTree2JsonConverter.cpp` is a command line tool to convert `AirwayTree.meta`
files used in the **lapdMouse** project into a Java Script Object Notation
(JSON) format. Resulting files can be easily read with e.g. Python.

Example usage: `./metaTree2JsonConverter m01_AirwayTree.meta m01_AirwayTree.json`

Example how to read with resulting `json` file and print some basic information
with [Python](https://www.python.org):

```py
import json # import json library
segments=json.load(open('m01_AirwayTree.json')) # read json file
len(segments) # get number of segments
segments[0]['ID'] # get ID of segment
segments[0]['Name'] # get name of segment
segments[0]['Children'] # get child segments
```

### simplifyTree

`simplifyTree.cpp` is a command line tool used in the **lapdMouse** to convert
`AirwayTree.meta` files to a simplified structure `AirwayTreeTable.csv`.
The simplified structures describing the airway tree as a set of connected
cylindrical elements that are stored in a Comma Separated Value (CSV) table
format. Resulting files can be easily read with e.g. Python or Excel.

Example usage: `./simplfyTree m01_AirwayTree.meta m01_AirwayTreeTable.csv`

Example how to read with resulting `json` file and print some basic information
with [Python](https://www.python.org) using [Pandas](https://pandas.pydata.org):

```py
df = pandas.read_csv(m01_AirwayTreeTable.csv, index_col='label') # read data
df.head(10) # print the first 10 entries
```

### mapOutlet2AirwaySegment

`mapOutlet2AirwaySegment.cpp` shows how to link outlets stored
`AirwaySegments.vtk` to airway segments stored in `AirayTree.meta`. The example
reads an `AirwayOutletsMesh.vtk` and obtains for each labeled outlet region its
center of gravity. Then, these are assigned to the airway segments in
`AirwayTree.meta` by finding the closest airway segment. The resulting mapping
of `outletId` to `segmentId` is printed to the command line in a Comma Separated
Value (CSV) format.

Example usage: `./mapOutlet2AirwaySegment m01_AirwayOutlets.vtk m01_AirwayTree.meta`

### labelTreePathAndChildren

`labelTreePathAndChildren.cpp` shows (a) how to identify and label airway
segments in a tree structure and (b) how to link information between the
`AirwaySegmentsMesh.vtk` and the `AirwayTree.meta`. This example reads an
`AirwaySegmentsMesh.vtk` mesh file and an `AirwayTree.meta` tree structure.
Based on a user specified `segmentId`, the program identified (a) the set of all
 airway segments from the root the specified segment and (b) all of its child
 segments. Then, it assigns appropriate label values to all associated mesh
 vertex point in the input mesh. The resulting labeled mesh is written to
 a new `.vtk` file. The result can get visualized using e.g.
 [3D Slicer](https://www.slicer.org).

Example usage: `./labelTreePathAndChildren m01_AirwaySegments.vtk m01_AirwayTree.meta 673 highlightedSegments.vtk`

### partitionLobesIntoTerminalCompartments

`partitionLobesIntoTerminalCompartments.cpp` partitions the lung's `Lobes.nrrd`
into disjoint compartments based the distance to `AirwayTree.meta` terminal
segments. The example initially reads `AirwayTree.meta` and identifies all end
points of terminal segments. Then it expands from these seed points disjoint
terminal compartment region utilizing a priority queue. During region expansion
lobar boundaries are not crosses, i.e. every terminal compartment is part of one
lobe only. The resulting compartments are stored in `terminalCompartments`. The
result can get visualized using e.g. [3D Slicer](https://www.slicer.org).

Example usage: `./partitionLobesIntoTerminalCompartments m01_Lobes.nrrd m01_AirwayTree.meta m01_TerminalCompartments.nrrd`

### imageLabelStatistics

`imageLabelStatistics.cpp` is a command line tool to calculate statistical
measurements for labeled regions. The program reads a labelmap and an intensity
image. It then calculates for each labeled region statistical measurements
including volume, average gray-value, etc. These values will printed to the
command line in a Comma Separated Value (CSV) format.

Example usage: `./imageLabelStatistics m01_AerosolSub2.mha  m01_TerminalCompartments.nrrd`

## License

**lapdMouseCppExamples** is distributed under [3-clause BSD license](License.txt).

## Acknowledgements

This work was supported by NIH project R01ES023863.

## Support

For support and further information please visit the
"Lung anatomy + particle deposition (lapd) mouse archive"
at <https://doi.org/10.25820/9arg-9w56> or our [data
repository](https://lapdmouse.iibi.uiowa.edu)

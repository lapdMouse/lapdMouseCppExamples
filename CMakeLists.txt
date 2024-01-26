cmake_minimum_required(VERSION 3.18)

PROJECT(LAPDMOUSECPPEXAMPLES)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(readWriteImage readWriteImage.cpp)
TARGET_LINK_LIBRARIES(readWriteImage ${ITK_LIBRARIES})

ADD_EXECUTABLE(readWriteLabelmap readWriteLabelmap.cpp)
TARGET_LINK_LIBRARIES(readWriteLabelmap ${ITK_LIBRARIES})

ADD_EXECUTABLE(readWriteMesh readWriteMesh.cpp)
TARGET_LINK_LIBRARIES(readWriteMesh ${ITK_LIBRARIES})

ADD_EXECUTABLE(readWriteTree readWriteTree.cpp)
TARGET_LINK_LIBRARIES(readWriteTree ${ITK_LIBRARIES})

ADD_EXECUTABLE(simplifyTree simplifyTree.cpp)
TARGET_LINK_LIBRARIES(simplifyTree ${ITK_LIBRARIES})

ADD_EXECUTABLE(metaTree2JsonConverter metaTree2JsonConverter.cpp)
TARGET_LINK_LIBRARIES(metaTree2JsonConverter ${ITK_LIBRARIES})

ADD_EXECUTABLE(accessTreeData accessTreeData.cpp)
TARGET_LINK_LIBRARIES(accessTreeData ${ITK_LIBRARIES})

ADD_EXECUTABLE(mapOutlet2AirwaySegment mapOutlet2AirwaySegment.cpp)
TARGET_LINK_LIBRARIES(mapOutlet2AirwaySegment ${ITK_LIBRARIES})

ADD_EXECUTABLE(labelTreePathAndChildren labelTreePathAndChildren.cpp)
TARGET_LINK_LIBRARIES(labelTreePathAndChildren ${ITK_LIBRARIES})

ADD_EXECUTABLE(partitionLobesIntoTerminalCompartments partitionLobesIntoTerminalCompartments.cpp)
TARGET_LINK_LIBRARIES(partitionLobesIntoTerminalCompartments ${ITK_LIBRARIES})

ADD_EXECUTABLE(imageLabelStatistics imageLabelStatistics.cpp)
TARGET_LINK_LIBRARIES(imageLabelStatistics ${ITK_LIBRARIES})

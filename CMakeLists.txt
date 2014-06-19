CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( osgdbIGES )

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

# Find Open CASCADE header files
IF("$ENV{OCC_INCLUDE_DIR}" STREQUAL "") 
  SET(OCC_INCLUDE_DIR "/usr/include/opencascade/" CACHE FILEPATH "Open CASCADE header files")
  MESSAGE(STATUS "Looking for opencascade include files in: ${OCC_INCLUDE_DIR}")
  MESSAGE(STATUS "Use OCC_INCLUDE_DIR to specify another directory")
ELSE()
  SET(OCC_INCLUDE_DIR $ENV{OCC_INCLUDE_DIR} CACHE FILEPATH "Open CASCADE header files")
  MESSAGE(STATUS "Looking for opencascade include files in: ${OCC_INCLUDE_DIR}")
ENDIF()

FIND_FILE(gp_Pnt_hxx "gp_Pnt.hxx" ${OCC_INCLUDE_DIR} /usr/inc /usr/local/inc /usr/local/include/oce)
IF(gp_Pnt_hxx)
  MESSAGE(STATUS "Header files found")
ELSE()
  MESSAGE(FATAL_ERROR "Unable to find header files, aborting")
ENDIF()

# Find Open CASCADE library files
IF("$ENV{OCC_LIBRARY_DIR}" STREQUAL "") 
  SET(OCC_LIBRARY_DIR "/usr/lib/" CACHE FILEPATH "Open CASCADE library files")
  MESSAGE(STATUS "Looking for opencascade library files in: ${OCC_LIBRARY_DIR}")
  MESSAGE(STATUS "Use OCC_LIBRARY_DIR to specify another directory")
ELSE()
  SET(OCC_LIBRARY_DIR $ENV{OCC_LIBRARY_DIR} CACHE FILEPATH "Open CASCADE library files")
  MESSAGE(STATUS "Looking for opencascade library files in: ${OCC_LIBRARY_DIR}")
ENDIF()

FIND_LIBRARY(libTKernel "TKernel" ${OCC_LIBRARY_DIR} /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64)
IF(libTKernel)
  MESSAGE(STATUS "Library files found")
ELSE()
  MESSAGE(FATAL_ERROR "Unable to find library files, aborting")
ENDIF()

LINK_DIRECTORIES (${LINK_DIRECTORIES} ${OCC_LIBRARY_DIR}) 

# Find OpenSceneGraph 
FIND_PACKAGE(OpenSceneGraph REQUIRED osgDB osgUtil osgGA osgFX osgSim osgText osgViewer)

INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${OPENTHREADS_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${OSG_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${OSGDB_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${OSGUTIL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${OSG_BINARY_DIR}/include ) # For the generated Config file


# - - - - - - - - - - - - - BRepConverter library - - - - - - - - - -

set(OSGOCC_HEADER_FILES 
  src/BRepConverter.h
  src/osgocc.h
)

set(OSGOCC_SOURCE_FILES 
  src/BRepConverter.cpp
  src/osgocc.cpp
)

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${OCC_INCLUDE_DIR})

ADD_LIBRARY(BRepConverter SHARED ${OSGOCC_HEADER_FILES} ${OSGOCC_SOURCE_FILES})

TARGET_LINK_LIBRARIES(BRepConverter TKernel TKMath TKBRep TKGeomBase TKGeomAlgo TKG3d TKG2d TKShHealing TKTopAlgo TKMesh TKPrim TKBool TKBO TKFillet TKSTEP TKSTEPBase TKSTEPAttr TKXSBase TKSTEP209 TKIGES TKOffset TKLCAF TKXCAF TKXDEIGES)

TARGET_LINK_LIBRARIES( BRepConverter ${OPENTHREADS_LIBRARY} )
TARGET_LINK_LIBRARIES( BRepConverter ${OSG_LIBRARY} )
TARGET_LINK_LIBRARIES( BRepConverter ${OSGDB_LIBRARY} )
TARGET_LINK_LIBRARIES( BRepConverter ${OSGUTIL_LIBRARY} )

# - - - - - - - - - - - - - osgdb_iges plugin - - - - - - - - - -

set(OSGDBIGES_HEADER_FILES 
  src/ReaderWriterIGES.h
)

set(OSGDBIGES_SOURCE_FILES 
  src/ReaderWriterIGES.cpp
)

ADD_LIBRARY(osgdb_iges SHARED ${OSGDBIGES_HEADER_FILES} ${OSGDBIGES_SOURCE_FILES})

TARGET_LINK_LIBRARIES( osgdb_iges BRepConverter )

SET_TARGET_PROPERTIES(osgdb_iges PROPERTIES PREFIX "") # Remove the leading 'lib' to match the OSG plugins naming conventions
SET_TARGET_PROPERTIES(osgdb_iges PROPERTIES SUFFIX ".so") # to match the OSG plugins naming conventions (so that osgDB can load the file at runtime)

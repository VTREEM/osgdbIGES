# OSGDB IGES reader

## Overview

osgdbIGES is an [OpenSceneGraph](http://openscenegraph.org) plugin to read IGES files using [OpenCascade](http://www.opencascade.org) to tesselate IGES entities.

## Authors

- Abhishek Bansal, [VizExperts India Pvt. Ltd.](http://www.vizexperts.com) ([initial release](http://lists.openscenegraph.org/pipermail/osg-submissions-openscenegraph.org/2013-December/021795.html))  
- Sylvain MARIE, [VTREEM](http://www.vtreem.com) (refactor, CMake file, update to use OCE 0.15)

## License

 This library is free software; you can redistribute it and/or  
 modify it under the terms of the GNU Lesser General Public  
 License as published by the Free Software Foundation; either  
 version 2.1 of the License, or (at your option) any later version.  
 
 This library is distributed in the hope that it will be useful,  
 but WITHOUT ANY WARRANTY; without even the implied warranty of  
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  
 Lesser General Public License for more details.  
 
 You should have received a copy of the GNU Lesser General Public  
 License along with this library; if not, write to the Free Software  
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  

## Prerequisites

- [OpenSceneGraph](https://github.com/openscenegraph/osg) (tested with commit 8979028)  
- [OpenCascade OCE](https://github.com/tpaviot/oce) (tested with 0.15)  

## How to build

### Building on OS X (RELEASE mode)

    ccmake -DCMAKE_BUILD_TYPE=Release \
      -D CMAKE_OSX_ARCHITECTURES:STRING="x86_64" \
      -D CMAKE_OSX_SYSROOT:STRING="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/" \
      -D CMAKE_INSTALL_PREFIX:STRING="{path to OSG binaries};{path to OSG root directory}"  \
      -D OSG_BINARY_DIR:STRING="{path to OSG binaries}" \
      -D OCC_INCLUDE_DIR:STRING="/usr/local/include/oce" \
      -D OCC_LIBRARY_DIR:STRING="/usr/local/lib" \
      -D CMAKE_CXX_FLAGS:STRING="-std=c++11" \
      -D CMAKE_EXE_LINKER_FLAGS:STRING="-stdlib=libc++" \
      {path to osgDBIGES sources}
    make -j 4

## How to use

Don't forget to add the path to the plugin to the OSG_LIBRARY_PATH environment variable in order the osgDB to find it at runtime.  

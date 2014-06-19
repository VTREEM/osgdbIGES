/*
 * IGES importer for OpenSceneGraph.
 * Copyright (c)2013 VizExperts India Pvt. Ltd.
 * Copyright (c)2014 VTREEM.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "osgocc.h"
#include "BRepConverter.h"

// OpenCascade Headers
#include <XCAFApp_Application.hxx>
#include <IGESCAFControl_Reader.hxx>


/// \brief this function takes a path to some IGES file and returns an OpenSceneGraph Geode
///        which directly can be used anywhere. It calculates normals using osgUtil::smoother
osg::ref_ptr<osg::Geode> igesToOSGGeode(const std::string& filePath)
{
    // XDE: Extended Data Exchange
    // OCAF: OpenCascade Application Technology Framework
    /// Getting an XDE document
    Handle(TDocStd_Document) doc;
    XCAFApp_Application::GetApplication()->NewDocument("MDTV-XCAF", doc);

    IGESCAFControl_Reader reader; 
    reader.SetColorMode(true);
    reader.SetNameMode(true);
    reader.SetLayerMode(true);

    //IGESControl_Reader Reader;
    reader.ReadFile( (Standard_CString)filePath.c_str() );
    /// transfer data from reader to doc
    if(!reader.Transfer(doc))
    {
        cout << "Cannot read any relevant data from the IGES file" << endl;
        return NULL;
    }

    BRepConverter brepConverter;
    return brepConverter.brepToOSGGeode(*doc);
}

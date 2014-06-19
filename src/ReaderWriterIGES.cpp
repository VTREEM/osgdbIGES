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

/// \file ReaderWriterIGES.cpp
/// \brief implementation file for osgdb plugin for IGES format
///        contains implementation of ReaderWriterIGES class
/// \author Abhishek Bansal

#include "ReaderWriterIGES.h"
#include "osgocc.h"

#include <osgDB/Registry>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

osgDB::ReaderWriter::ReadResult 
ReaderWriterIGES::readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
{
    // some error handling
    std::string ext = osgDB::getLowerCaseFileExtension(fileName);
    if (!acceptsExtension(ext)) 
        return ReadResult::FILE_NOT_HANDLED;

    std::string file = osgDB::findDataFile(fileName, options);
    if (file.empty()) 
        return ReadResult::FILE_NOT_FOUND;

    OSG_INFO << "ReaderWriterIGES::readNode(" << file.c_str() << ")\n";

    return igesToOSGGeode(fileName).get();
}

osgDB::ReaderWriter::WriteResult 
ReaderWriterIGES::writeNode(const osg::Node& /*node*/,const std::string& fileName /*fileName*/,const Options*) const
{
    OSG_FATAL << "File Writing not supported yet" << std::endl;
    return WriteResult::FILE_NOT_HANDLED;
}

// register with Registry to instantiate the above writer.
REGISTER_OSGPLUGIN(iges, ReaderWriterIGES)


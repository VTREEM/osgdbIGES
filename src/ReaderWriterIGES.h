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

#ifndef _READERWRITERIGES_H_
#define _READERWRITERIGES_H_

/// \file ReaderWriterIGES.h
/// \brief header file for creating osgdb plugin for IGES format
/// \author Abhishek Bansal, Engineer Graphics, vizExperts India Pvt. Ltd. 


/// \class ReaderWriterIGES
/// \brief contains implementation of reading IGES models
///        depends on OpenCascade library
///        this code was written with version OCE 0.15
/// \todo enabling/disabling Healing can be added as reader writer options

#include <osgDB/ReaderWriter>

class ReaderWriterIGES: public osgDB::ReaderWriter
{
    public:
        /// \brief constructor
        ReaderWriterIGES()
        {
            supportsExtension("IGES","IGES file format");
            supportsExtension("iges","IGES file format");
            supportsExtension("IGS","IGS file format");
            supportsExtension("igs","IGS file format");
        }

        /// \brief returns class name 
        virtual const char* className() const 
        {
            return "IGES Reader";
        }

        virtual osgDB::ReaderWriter::ReadResult 
        readNode(const std::string& fileName, const osgDB::ReaderWriter::Options*) const;

        virtual osgDB::ReaderWriter::WriteResult 
        writeNode(const osg::Node&, const std::string&, const Options* =NULL) const ;

};

#endif // _READERWRITERIGES_H_
/*
 * Conversion routines from OCC to OSG.
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

#include <osg/Geode>
#include <osg/Geometry>

#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <TDocStd_Document.hxx>

#ifndef _BREPCONVERTER_H_
#define _BREPCONVERTER_H_

/// \brief following class will contain all reading related functionality
/// \detail this class uses OCT XDE module to read IGES file. XDE mechanism is needed
///         to find out colors and transformation of sub shapes.
///         normal IGESReader wasn't giving very good results with shapes. Edges weren't sharp enough
///         and also there is no way in which we can get color information with that
/// \Note Go through XDE user guide and IGES User guide supplied with 
/// \todo OSG automatic normal calculation is not working good for few mnodels
///       try to get from XDE document only
class BRepConverter
{
    public:
        /// \brief this function is single point of contact for this class.
        ///        it takes path of OCC XCAF document and returns an OpenSceneGraph Geode
        ///        which directly can be used anywhere. It calculates normals using osgUtil::smoother
        osg::ref_ptr<osg::Geode> brepToOSGGeode(TDocStd_Document& doc);

    private:

        /// \brief recursively traverse opencascade assembly structure and build a osg geode
        ///        this function also finds color for leaf node shapes and calculates transformation from parent 
        ///        to leaf
        /// \param[in] shapeTree its a OCT(OpenCascade Technology) XDE document label which might contain children or referred shapes
        /// \param[in] transformation contains transformation matrix to be applied 
        void _traverse(const TDF_Label &shapeTree, gp_Trsf& transformation);

        /// \brief heals a opencascade shape
        /// \detail http://www.opencascade.org/org/forum/thread_12716/?forum=3
        ///         Usually IGES files suffer from precision problems (when transfering from
        ///         one CAD system to another).It might be the case that faces are not sewed
        ///         properly,  or do not have the right precision, and so the tesselator does
        ///         not treat them like "sewed". this needs to be done for sewing
        /// \param[in,out] shape opencascade shape to be healed
        void _healShape(TopoDS_Shape& shape);

        /// \brief takes and OpenCascadeShape and returns OSG geometry(drawable), which further can be added to a geode
        /// \detail it iterates shape and breaks it into faces, builds vertex list, color list and creates geometry
        ///         transformation is applied to each vertex before storing it into vertex list
        ///         all vertices are assigned same color
        /// \param[in] shape shape to be converted in geometry. Not a const because it needs to be modified if healing
        ///        is enabled
        /// \param[in] color color of geometry
        /// \param[in] transformation matrix with which vertex position has to be transformed
        osg::ref_ptr<osg::Geometry> _createGeometryFromShape(TopoDS_Shape& shape, const osg::Vec3& color, gp_Trsf& transformation);
    
    private:
        
        /// \bried XDE document color tool it stores all colors in color table
        ///        and used to get color from a label
        Handle(XCAFDoc_ColorTool) _colorTool;

        /// \brief geode to contain full model
        osg::ref_ptr<osg::Geode> _modelGeode;

        /// \brief shape tool instance to deal with shapes(simple shapes), referredShape, children etc
        Handle (XCAFDoc_ShapeTool) _assembly;
};


#endif // _BREPCONVERTER_H_
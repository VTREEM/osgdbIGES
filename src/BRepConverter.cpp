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

#include "BRepConverter.h"

// OpenCascade Headers
#include <TopTools_HSequenceOfShape.hxx>
#include <TopExp_Explorer.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_CString.hxx>
#include <Standard_Macro.hxx>

#include <TopoDS_Face.hxx>

#include <BRepMesh.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepBuilderAPI_Sewing.hxx>

#include <ShapeFix_Shape.hxx>

#include <gp_Pnt2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>

#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <IGESControl_Controller.hxx>

#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_DocumentTool.hxx>

#include <XCAFApp_Application.hxx>

#include <IGESCAFControl_Reader.hxx>

#include <TDF_LabelSequence.hxx>
#include <TDF_ChildIterator.hxx>

#include <XSDraw.hxx>
#include <Quantity_Color.hxx>

// osg headers
#include <osg/PrimitiveSet>
#include <osg/MatrixTransform>

#include <osgUtil/TriStripVisitor>
#include <osgUtil/SmoothingVisitor>

/// \brief this function is single point of contact for this class.
///        it takes path of OCC XCAF document and returns an OpenSceneGraph Geode
///        which directly can be used anywhere. It calculates normals using osgUtil::smoother
osg::ref_ptr<osg::Geode> 
BRepConverter::brepToOSGGeode(TDocStd_Document& doc)
{
    
    // To get a node considered as an Assembly from an XDE structure, you can use the Label of the node.
    _assembly = XCAFDoc_DocumentTool::ShapeTool(doc.Main());

    // To query, edit, or initialize a Document to handle Colors of XCAF
    _colorTool = XCAFDoc_DocumentTool::ColorTool(doc.Main());

    // free shape sequence
    // get sequence of free shape lables
    TDF_LabelSequence freeShapes;
    _assembly->GetFreeShapes(freeShapes);

    if(freeShapes.Length() == 0)
    {
        std::cout << "No Shapes found" << std::endl;
        return NULL;
    }
    else
    {
        std::cout << std::endl << "No of Free Shapes: " << freeShapes.Length() << std::endl;
    }

    _modelGeode = new osg::Geode();
    /// send all root nodes for recursive traversing
    /// find transformation as it will be needed for location calculation later
    for (int i = 1; i <= freeShapes.Length(); i++)
    {
        Handle(XCAFDoc_Location) attribute;
        gp_Trsf transformation;
        freeShapes.Value(i).FindAttribute(XCAFDoc_Location::GetID(), attribute);
        if(attribute.IsNull() == Standard_False)
        {
             TopLoc_Location location = attribute->Get();
             transformation = location.Transformation();
        }
        _traverse(freeShapes.Value(i), transformation); 
    }

    /// calculate normals
    OSG_NOTICE << "Calculating Normals" << std::endl;
    
    osgUtil::SmoothingVisitor sv;
    _modelGeode->accept(sv);
    
    return _modelGeode;
}

/// \brief recursively traverse opencascade assembly structure and build a osg geode
///        this function also finds color for leaf node shapes and calculates transformation from parent 
///        to leaf
/// \param shapeTree its a OCT(OpenCascade Technology) XDE document label which might contain children or referred shapes
/// \param transformation contains transformation matrix to be applied 
/// \note Simple Shape: is a shape which is not a compound. Its can be a free or non free shape
/// \note Support Thread: http://www.opencascade.org/org/forum/thread_25512/?forum=3
void 
BRepConverter::_traverse(const TDF_Label &shapeTree, gp_Trsf& transformation)
{
    TDF_Label referredShape;
    /// find if current shape referes some shape. if it does then traverse that
    /// else it is a simple shape and do visualize that simple shape
    if(_assembly->GetReferredShape(shapeTree, referredShape))
    {
        Handle(XCAFDoc_Location) attribute;
        referredShape.FindAttribute(XCAFDoc_Location::GetID(), attribute);
        if(attribute.IsNull() == Standard_False)
        {
             TopLoc_Location location = attribute->Get();
             transformation *= location.Transformation();
        }

        /// if referred shape has children traverse them first else
        /// travese the shape itself
        if(referredShape.HasChild())
        {
            TDF_ChildIterator it;
            for(it.Initialize(referredShape); it.More(); it.Next())
            {
                _traverse(it.Value(), transformation);
            }
        }
        else
        {
            OSG_NOTICE << std::endl << "No children found";
            _traverse(referredShape, transformation);
        }
    }
    else
    {
        /// Find out if this simple shape has any color store that color as color of geometry
        Quantity_Color color;
        osg::Vec3 geomColor = osg::Vec3(.7, .7, .7);
        if(_colorTool->GetColor(shapeTree, XCAFDoc_ColorGen, color) ||
           _colorTool->GetColor(shapeTree, XCAFDoc_ColorSurf, color) ||
           _colorTool->GetColor(shapeTree, XCAFDoc_ColorCurv, color) )
        {
            OSG_NOTICE << std::endl << "Free Shape has a color !! " << color.Red() << " " << color.Green() << " "<< color.Blue();
            geomColor = osg::Vec3(color.Red(),color.Green(), color.Blue());
        }

        TopoDS_Shape shape = _assembly->GetShape(shapeTree);

        Handle(XCAFDoc_Location) attribute;
        shapeTree.FindAttribute(XCAFDoc_Location::GetID(), attribute);
        if(attribute.IsNull() == Standard_False)
        {
             TopLoc_Location location = attribute->Get();
             transformation *= location.Transformation();
        }

        osg::ref_ptr<osg::Geometry> geom = _createGeometryFromShape(shape, geomColor, transformation);
        /// add this geometry to model geode
        if(geom.valid())
        {
            _modelGeode->addDrawable(geom);
        }
        else
        {
            std::cout << std::endl << "Invalid Geometry found !!";
        }
    }
}

/// \brief heals a opencascade shape
/// \detail http://www.opencascade.org/org/forum/thread_12716/?forum=3
///         Usually IGES files suffer from precision problems (when transfering from
///         one CAD system to another).It might be the case that faces are not sewed
///         properly,  or do not have the right precision, and so the tesselator does
///         not treat them like "sewed". this needs to be done for sewing
/// \param shape opencascade shape to be healed
void BRepConverter::_healShape(TopoDS_Shape& shape)
{
    OSG_NOTICE << std::endl << "Going to heal shape!!";

    ShapeFix_Shape fixer(shape);
    fixer.Perform();
    shape = fixer.Shape();

    BRepBuilderAPI_Sewing sew;
    sew.Add(shape);
    sew.Perform();
    shape = sew.SewedShape();
}

/// \brief takes and OpenCascadeShape and returns OSG geometry(drawable), which further can be added to a geode
/// \detail it iterates shape and breaks it into faces, builds vertex list, color list and creates geometry
///         transformation is applied to each vertex before storing it into vertex list
///         all vertices are assigned same color
/// \param shape shape to be converted in geometry. Not a const because it needs to be modified if healing
///        is enabled
/// \param color color of geometry
/// \param transformation matrix with which vertex position has to be transformed
osg::ref_ptr<osg::Geometry> BRepConverter::_createGeometryFromShape(TopoDS_Shape& shape, const osg::Vec3& geomColor, gp_Trsf& transformation)
{
    // vector to save vertices
    osg::ref_ptr<osg::Vec3Array> vertexList = new osg::Vec3Array();
    // vector to save _colorTool
    osg::ref_ptr<osg::Vec3Array> colorList = new osg::Vec3Array();

    // create one osg primitive set
    osg::ref_ptr<osg::DrawElementsUInt> triangleStrip = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    unsigned int noOfTriangles = 0;

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    if(!shape.IsNull())
    {
        // clean any previous triangulation
        BRepTools::Clean(shape);

        //_healShape(shape);

        OSG_NOTICE << std::endl << "Building a Mesh !!" ;

        /// call to incremental mesh on this shape
        /// \todo not sure why this 1 is passed. Its called deflection BTW
        ///       need to find a way to calculate it
        BRepMesh::Mesh(shape, 1);

        ///iterate faces
        // this variable will help in keeping track of face indices
        unsigned int index = 0; 
        for (TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next()) 
        { 
            TopoDS_Face face = TopoDS::Face(ex.Current());
            TopLoc_Location location; 

            /// triangulate current face
            Handle (Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
            if (!triangulation.IsNull())
            {
                int noOfNodes = triangulation->NbNodes();

                // Store vertices. Build vertex array here
                for(int j = 1; j <= triangulation->NbNodes(); j++)
                {
                    // populate vertex list
                    // Ref: http://www.opencascade.org/org/forum/thread_16694/?forum=3
                    gp_Pnt pt = (triangulation->Nodes())(j).Transformed(transformation * location.Transformation());
                    vertexList->push_back(osg::Vec3(pt.X(), pt.Y(), pt.Z()));

                    // populate color list
                    colorList->push_back(geomColor);
                }

                /// now we need to get face indices for triangles
                // get list of triangle first
                const Poly_Array1OfTriangle& triangles = triangulation->Triangles();

                //No of triangles in this triangulation
                noOfTriangles = triangulation->NbTriangles();
                
                Standard_Integer v1, v2, v3;
                for (unsigned int j = 1; j <= noOfTriangles; j++)
                {
                    /// If face direction is reversed then we add verticews in reverse order
                    /// order of vertices is important for normal calculation later
                    if (face.Orientation() == TopAbs_REVERSED)
                    {
                        triangles(j).Get(v1, v3, v2);
                    }
                    else
                    {
                        triangles(j).Get(v1, v2, v3);
                    }
                    triangleStrip->push_back(index + v1 - 1);
                    triangleStrip->push_back(index + v2 - 1);
                    triangleStrip->push_back(index + v3 - 1);
                }
                index = index + noOfNodes;
            }
        }
        
        OSG_NOTICE << "Creating a geometry.." << std::endl;
        
        geom->setVertexArray(vertexList.get());

        geom->setColorArray(colorList.get());
        geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

        OSG_NOTICE << "Adding Primitive set" << std::endl;
        
        geom->addPrimitiveSet(triangleStrip);
    }

    return geom;
}
/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
*
* This library is open source and may be redistributed and/or modified under  
* the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
* (at your option) any later version.  The full license is in LICENSE file
* included with this distribution, and on the openscenegraph.org website.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
* OpenSceneGraph Public License for more details.
*/

#include <stdlib.h>
#include <float.h>
#include <limits.h>

#include <iomanip>
#include <sstream>

#include <osgDB/FileNameUtils>

#include <osg/Version>
#include <osg/Geometry>
#include <osg/TexMat>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/io_utils>

#include <osgViewer/Viewer>
#include "InteractiveImageHandler.h"


InteractiveImageHandler2::InteractiveImageHandler2(osg::Image* image) :
    _image(image),
    _texture(0),
    _fullscreen(false),
    _camera(0)
{
}

InteractiveImageHandler2::InteractiveImageHandler2(osg::Image* image, osg::Texture2D* texture, osg::Camera* camera) :
    _image(image),
    _texture(texture),
    _fullscreen(true),
    _camera(camera)
{

    if (_camera.valid() && _camera->getViewport())
    {
        // Send an initial resize event (with the same size) so the image can
        // resize itself initially.
        double width = _camera->getViewport()->width();
        double height = _camera->getViewport()->height();
		_defaultWidth = width;
		_defaultHeight = height;
        resize(width, height);
    }
}

bool InteractiveImageHandler2::computeIntersections(osgViewer::View* view, float x,float y, const osg::NodePath& nodePath, osgUtil::LineSegmentIntersector::Intersections& intersections,osg::Node::NodeMask traversalMask) const 
{
    float local_x, local_y = 0.0;
    const osg::Camera* camera;
    if (_fullscreen)
    {
        if (!_camera) return false;
        camera = _camera.get();
        local_x = x;
        local_y = y;
    }
    else
    {
        if (!view->getCamera() || nodePath.empty()) return false;
        camera = view->getCameraContainingPosition(x, y, local_x, local_y);
        if (!camera) camera = view->getCamera();
    }

    osg::ref_ptr< osgUtil::LineSegmentIntersector > picker;
    if (_fullscreen)
    {
        picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, local_x, local_y);
    }
    else
    {
        osg::Matrixd matrix;
        if (nodePath.size()>1)
        {
            osg::NodePath prunedNodePath(nodePath.begin(),nodePath.end()-1);
            matrix = osg::computeLocalToWorld(prunedNodePath);
        }

        matrix.postMult(camera->getViewMatrix());
        matrix.postMult(camera->getProjectionMatrix());

        double zNear = -1.0;
        double zFar = 1.0;
        if (camera->getViewport())
        {
            matrix.postMult(camera->getViewport()->computeWindowMatrix());
            zNear = 0.0;
            zFar = 1.0;
        }

        osg::Matrixd inverse;
        inverse.invert(matrix);

        osg::Vec3d startVertex = osg::Vec3d(local_x,local_y,zNear) * inverse;
        osg::Vec3d endVertex = osg::Vec3d(local_x,local_y,zFar) * inverse;

        picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::MODEL, startVertex, endVertex);
    }

    osgUtil::IntersectionVisitor iv(picker.get());
    iv.setTraversalMask(traversalMask);

    if (_fullscreen)
    {
        const_cast<osg::Camera*>(camera)->accept(iv);
    }
    else
    {
        nodePath.back()->accept(iv);
    }

    if (picker->containsIntersections())
    {
        intersections = picker->getIntersections();
        return true;
    }
    else
    {
        intersections.clear();
        return false;
    }
}

bool InteractiveImageHandler2::mousePosition(osgViewer::View* view, osg::NodeVisitor* nv, const osgGA::GUIEventAdapter& ea, int& x, int &y) const
{
    osgUtil::LineSegmentIntersector::Intersections intersections;
    bool foundIntersection = view==0 ? false :
        (nv==0 ? view->computeIntersections(ea.getX(), ea.getY(), intersections) :
                 //view->computeIntersections(ea.getX(), ea.getY(), nv->getNodePath(), intersections));
                 computeIntersections(view, ea.getX(), ea.getY(), nv->getNodePath(), intersections));

    if (foundIntersection)
    {
        osg::Vec2 tc(0.5f,0.5f);

        // use the nearest intersection                 
        const osgUtil::LineSegmentIntersector::Intersection& intersection = *(intersections.begin());
        osg::Drawable* drawable = intersection.drawable.get();
        osg::Geometry* geometry = drawable ? drawable->asGeometry() : 0;
        osg::Vec3Array* vertices = geometry ? dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray()) : 0;
        if (vertices)
        {
            // get the vertex indices.
            const osgUtil::LineSegmentIntersector::Intersection::IndexList& indices = intersection.indexList;
            const osgUtil::LineSegmentIntersector::Intersection::RatioList& ratios = intersection.ratioList;

            if (indices.size()==3 && ratios.size()==3)
            {
                unsigned int i1 = indices[0];
                unsigned int i2 = indices[1];
                unsigned int i3 = indices[2];

                float r1 = ratios[0];
                float r2 = ratios[1];
                float r3 = ratios[2];

                osg::Array* texcoords = (geometry->getNumTexCoordArrays()>0) ? geometry->getTexCoordArray(0) : 0;
                osg::Vec2Array* texcoords_Vec2Array = dynamic_cast<osg::Vec2Array*>(texcoords);
                if (texcoords_Vec2Array)
                {
                    // we have tex coord array so now we can compute the final tex coord at the point of intersection.                                
                    osg::Vec2 tc1 = (*texcoords_Vec2Array)[i1];
                    osg::Vec2 tc2 = (*texcoords_Vec2Array)[i2];
                    osg::Vec2 tc3 = (*texcoords_Vec2Array)[i3];
                    tc = tc1*r1 + tc2*r2 + tc3*r3;
                }
            }

            osg::TexMat* activeTexMat = 0;
            osg::Texture* activeTexture = 0;

            if (drawable->getStateSet())
            {
                osg::TexMat* texMat = dynamic_cast<osg::TexMat*>(drawable->getStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
                if (texMat) activeTexMat = texMat;

                osg::Texture* texture = dynamic_cast<osg::Texture*>(drawable->getStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
                if (texture) activeTexture = texture;
            }

            if (activeTexMat)
            {
                osg::Vec4 tc_transformed = osg::Vec4(tc.x(), tc.y(), 0.0f,0.0f) * activeTexMat->getMatrix();
                tc.x() = tc_transformed.x();
                tc.y() = tc_transformed.y();
            }

            if (dynamic_cast<osg::TextureRectangle*>(activeTexture))
            {
                x = int( tc.x() );
                y = int( tc.y() );
            }
            else if (_image.valid())
            {
                x = int( float(_image->s()) * tc.x() );
                y = int( float(_image->t()) * tc.y() );
            }

            return true;
        }

    }

    return false;
}


bool InteractiveImageHandler2::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor* nv)
{
    if (ea.getHandled()) return false;
    
    if (!_image) return false;

    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::MOVE):
        case(osgGA::GUIEventAdapter::DRAG):
        case(osgGA::GUIEventAdapter::PUSH):
        case(osgGA::GUIEventAdapter::RELEASE):
        {
            osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
            int x,y;
            if (mousePosition(view, nv, ea, x, y))
            {
                return _image->sendPointerEvent(x, y, ea.getButtonMask());
            }
            break;
        }
        case(osgGA::GUIEventAdapter::KEYDOWN):
        case(osgGA::GUIEventAdapter::KEYUP):
        {
            osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
            int x,y;
            bool sendKeyEvent = mousePosition(view, nv, ea, x, y);
        
            if (sendKeyEvent)
            {
                return _image->sendKeyEvent(ea.getKey(), ea.getEventType()==osgGA::GUIEventAdapter::KEYDOWN);
            }          
        }
        case (osgGA::GUIEventAdapter::RESIZE):
        {
            if (_fullscreen && _camera.valid())
            {	
				//不允许缩放，否则会充满屏幕
				//_camera->setViewport(0, 0, _defaultWidth, 768);
    //            resize(_defaultWidth, _defaultHeight);
 /*               _camera->setViewport(0, 0, ea.getWindowWidth(), ea.getWindowHeight());
                resize(ea.getWindowWidth(), ea.getWindowHeight());*/

                return false;
            }
        }

        default:
            return false;
    }
    return false;
}

bool InteractiveImageHandler2::cull(osg::NodeVisitor* nv, osg::Drawable*, osg::RenderInfo*) const
{
    if (_image.valid())
    {
        _image->setFrameLastRendered(nv->getFrameStamp());
    }

    return false;
}

void InteractiveImageHandler2::resize(int width, int height)
{
    if (_image.valid())
    {
        _image->scaleImage(width, height, 1);
    }

    // Make sure the texture does not rescale the image because 
    // it thinks it should still be the previous size...
    if (_texture.valid())
        _texture->setTextureSize(width, height);
}


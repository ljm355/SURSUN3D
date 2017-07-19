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

#ifndef InteractiveImageHandler
#define InteractiveImageHandler 1

#include <osg/AnimationPath>
#include <osgText/Text>
#include <osgGA/GUIEventHandler>
#include <osgGA/AnimationPathManipulator>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>

#include <osgDB/fstream>



class  InteractiveImageHandler2 : public osgGA::GUIEventHandler, public osg::Drawable::CullCallback
{
public:

    /// Constructor to use when the InteractiveImage is in the 3D scene (i.e. not in a fullscreen HUD overlay).
    InteractiveImageHandler2(osg::Image* image);
    /// Constructor to use when the InteractiveImage is in a fullscreen HUD overlay.
    InteractiveImageHandler2(osg::Image* image, osg::Texture2D* texture, osg::Camera* camera);

    META_Object(osgViewer, InteractiveImageHandler2);

    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor* nv);

    virtual bool cull(osg::NodeVisitor* nv, osg::Drawable* drawable, osg::RenderInfo* renderInfo) const;

protected:

    virtual ~InteractiveImageHandler2() {}

    InteractiveImageHandler2() {}

    InteractiveImageHandler2(const InteractiveImageHandler2&,const osg::CopyOp& = osg::CopyOp::SHALLOW_COPY):
        osg::Object(), osgGA::GUIEventHandler(), osg::Drawable::CullCallback(), _fullscreen(false) {}

    bool mousePosition(osgViewer::View* view, osg::NodeVisitor* nv, const osgGA::GUIEventAdapter& ea, int& x, int &y) const;
    bool computeIntersections(osgViewer::View* view, float x,float y, const osg::NodePath& nodePath, osgUtil::LineSegmentIntersector::Intersections& intersections,osg::Node::NodeMask traversalMask = 0xffffffff) const;

    void resize(int width, int height);

    osg::observer_ptr<osg::Image>   _image;
    osg::observer_ptr<osg::Texture2D> _texture;

    bool                            _fullscreen;
    osg::observer_ptr<osg::Camera>  _camera;
	int _defaultWidth;
	int _defaultHeight;

};



#endif

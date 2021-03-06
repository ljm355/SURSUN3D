/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2013 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef OSGEARTHQT_VIEWERWIDGET2_H
#define OSGEARTHQT_VIEWERWIDGET2_H 1

#include <osgEarthQt/Common>

#include <osgEarth/Map>

#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerBase>

#include <QtCore/QTimer>


    using namespace osgEarth;

    /**
     * Qt widget that encapsulates an osgViewer::Viewer.
     */
    class ViewerWidget2 : public osgQt::GLWidget
    {
        Q_OBJECT;

    public:
        /**
         * Constructs a new ViewerWidget2, creating an underlying viewer.
         * @param[in ] scene Scene graph to attach to the viewer (optional)
         */
        ViewerWidget2(osg::Node* scene=0L);

        /**
         * Constructs a new ViewerWidget2, attaching an existing viewer.
         * @param[in ] viewer Viewer to attach to this widget. The widget will install
         *             a new camera in the viewer. (NOTE: this widget does not take
         *             ownership of the Viewer. You are still respsonsile for its
         *             destruction)
         */
        ViewerWidget2(osgViewer::ViewerBase* viewer);

        /**
         * Access the underlying viewer.
         */
        osgViewer::ViewerBase* getViewer() { return _viewer.get(); }

        /**
         * Populates the incoming collection with the views comprising this viewer.
         */
        template<typename T>
        void getViews( T& views ) const {
            osgViewer::ViewerBase::Views temp;
            _viewer->getViews(temp);
            views.insert(views.end(), temp.begin(), temp.end());
        }

        virtual ~ViewerWidget2();

    public slots:
        
        /**
         * Change the underlying timer's interval
         */
        void setTimerInterval( int milliseconds );


    protected:

        QTimer _timer;

        void installFrameTimer();

        void createViewer();
        void reconfigure( osgViewer::View* );
        void paintEvent( QPaintEvent* );

        osg::observer_ptr<osgViewer::ViewerBase> _viewer;
        osg::ref_ptr<osg::GraphicsContext>       _gc;
    };


#endif // OSGEARTHQT_VIEWERWIDGET_H

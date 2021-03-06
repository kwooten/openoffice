/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef INCLUDED_CANVAS_ISURFACEPROXYMANAGER_HXX
#define INCLUDED_CANVAS_ISURFACEPROXYMANAGER_HXX

#include <canvas/rendering/irendermodule.hxx>
#include <canvas/rendering/icolorbuffer.hxx>

#include <boost/shared_ptr.hpp>


namespace canvas
{
    struct ISurfaceProxy;

    /** Manager interface, which handles surface proxy objects.

        Typically, each canvas instantiation has one
        SurfaceProxyManager object, to handle their surfaces. Surfaces
        itself are opaque objects, which encapsulate a framebuffer to
        render upon, plus an optional (possibly accelerated) texture.
     */
    struct ISurfaceProxyManager
    {
        virtual ~ISurfaceProxyManager() {}

        /** Create a surface proxy for a color buffer.

            The whole idea is build around the concept that you create
            some arbitrary buffer which contains the image data and
            tell the texture manager about it.  From there on you can
            draw into this image using any kind of graphics api you
            want.  In the technical sense we allocate some space in
            local videomemory or AGP memory which will be filled on
            demand, which means if there exists any rendering
            operation that needs to read from this memory location.
            This method creates a logical hardware surface object
            which uses the given color buffer as the image source.
            Internally this texture may even be distributed to several
            real hardware surfaces.
        */
        virtual ::boost::shared_ptr< ISurfaceProxy > createSurfaceProxy( 
            const IColorBufferSharedPtr& pBuffer ) const = 0;
    };

    typedef ::boost::shared_ptr< ISurfaceProxyManager > ISurfaceProxyManagerSharedPtr;


    /** Create a surface proxy for the given render module.
     */
	ISurfaceProxyManagerSharedPtr createSurfaceProxyManager( const IRenderModuleSharedPtr& rRenderModule );
}

#endif /* INCLUDED_CANVAS_ISURFACEPROXYMANAGER_HXX */

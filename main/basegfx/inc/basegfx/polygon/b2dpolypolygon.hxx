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



#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#define _BGFX_POLYGON_B2DPOLYPOLYGON_HXX

#include <sal/types.h>
#include <o3tl/cow_wrapper.hxx>
#include <basegfx/range/b2drange.hxx>

// predeclarations
class ImplB2DPolyPolygon;

namespace basegfx
{ 
	class B2DPolygon; 
	class B2DHomMatrix;
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
	class B2DPolyPolygon
	{
    public:
        typedef o3tl::cow_wrapper< ImplB2DPolyPolygon > ImplType;

	private:
        ImplType                                        mpPolyPolygon;

	public:		
		B2DPolyPolygon();
		B2DPolyPolygon(const B2DPolyPolygon& rPolyPolygon);
		explicit B2DPolyPolygon(const B2DPolygon& rPolygon);
		~B2DPolyPolygon();

		// assignment operator
		B2DPolyPolygon& operator=(const B2DPolyPolygon& rPolyPolygon);

        /// unshare this poly-polygon (and all included polygons) with all internally shared instances
        void makeUnique();

		// compare operators
		bool operator==(const B2DPolyPolygon& rPolyPolygon) const;
		bool operator!=(const B2DPolyPolygon& rPolyPolygon) const;

		// polygon interface
		sal_uInt32 count() const;

		B2DPolygon getB2DPolygon(sal_uInt32 nIndex) const;
		void setB2DPolygon(sal_uInt32 nIndex, const B2DPolygon& rPolygon);

		// test for curve
		bool areControlPointsUsed() const;

		// insert/append single polygon
		void insert(sal_uInt32 nIndex, const B2DPolygon& rPolygon, sal_uInt32 nCount = 1);
		void append(const B2DPolygon& rPolygon, sal_uInt32 nCount = 1);

		/** Default adaptive subdivision access

			For details refer to B2DPolygon::getDefaultAdaptiveSubdivision()

            @return
			The default subdivision of this polygon
		*/
        B2DPolyPolygon getDefaultAdaptiveSubdivision() const;

        /** Get the B2DRange (Rectangle dimensions) of this B2DPolyPolygon

			For details refer to B2DPolygon::getB2DRange()

			@return
			The outer range of the bezier curve/polygon
        */
        B2DRange getB2DRange() const;

        // insert/append multiple polygons
		void insert(sal_uInt32 nIndex, const B2DPolyPolygon& rPolyPolygon);
		void append(const B2DPolyPolygon& rPolyPolygon);

		// remove
		void remove(sal_uInt32 nIndex, sal_uInt32 nCount = 1);

		// reset to empty state
		void clear();

		// closed state
		bool isClosed() const;
		void setClosed(bool bNew);

		// flip polygon direction
		void flip();

		// test if PolyPolygon has double points
		bool hasDoublePoints() const;

		// remove double points, at the begin/end and follow-ups, too
		void removeDoublePoints();

		// apply transformation given in matrix form to the polygon
		void transform(const basegfx::B2DHomMatrix& rMatrix);

        // polygon iterators (same iterator validity conditions as for vector)
        const B2DPolygon* begin() const;
        const B2DPolygon* end() const;
        B2DPolygon* begin();
        B2DPolygon* end();
	};

    // typedef for a vector of B2DPolyPolygons
    typedef ::std::vector< B2DPolyPolygon > B2DPolyPolygonVector;

} // end of namespace basegfx

#endif /* _BGFX_POLYGON_B2DPOLYPOLYGON_HXX */

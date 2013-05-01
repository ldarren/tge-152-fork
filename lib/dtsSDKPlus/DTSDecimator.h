//-----------------------------------------------------------------------------
// Decimator 
// based on "A Simple, Fast, and Effective Polygon Reduction Algorithm"
// by Stan Melax from Game Developer Magazine, November 1998
//-----------------------------------------------------------------------------

#ifndef __DTS_DECIMATOR_H
#define __DTS_DECIMATOR_H

#include "DTSMesh.h"
#include "DTSPoint.h"

namespace DTS
{
	/*
	 *  For the polygon reduction algorithm we use data structures
	 *  that contain a little bit more information than the usual
	 *  indexed face set type of data structure.
	 *  From a vertex we wish to be able to quickly get the
	 *  neighboring faces and vertices.
	 */
	class DecimatorTriangle;
	class DecimatorVertex;
   class DecimatorVector;

	class DecimatorTriangle
   {
      public:
		   DecimatorVertex* vertex[3]; // the 3 points that make this tri
		   Point normal;    // unit vector othogonal to this face
         S32 type;

         DecimatorTriangle(DecimatorVertex *v0,DecimatorVertex *v1,DecimatorVertex *v2, S32 _type);
		   ~DecimatorTriangle();

		   void ComputeNormal();
		   void ReplaceVertex(DecimatorVertex *vold,DecimatorVertex *vnew);
		   S32 HasVertex(DecimatorVertex *v);
	};

	class DecimatorVertex
   {
	   public:
		   Point position; // location of point in euclidean space
		   S32 id;       // place of vertex in original list
			std::vector<DecimatorVertex *> neighbor; // adjacent vertices
			std::vector<DecimatorTriangle *> face;     // adjacent triangles
		   F32 objdist;  // cached cost of collapsing edge
		   DecimatorVertex* collapse; // candidate vertex for collapse

		   DecimatorVertex(Point v,S32 _id);
		   ~DecimatorVertex();

         S32 IsBorder();
		   void RemoveIfNonNeighbor(DecimatorVertex *n);
	};

	class Decimator
	{
		private:
			F32 ComputeEdgeCollapseCost(DecimatorVertex *u,DecimatorVertex *v);
			void ComputeEdgeCostAtVertex(DecimatorVertex *v);
			void ComputeAllEdgeCollapseCosts();
			void Collapse(DecimatorVertex *u,DecimatorVertex *v);
			DecimatorVertex *MinimumCostEdge();

         std::vector<Primitive> primitives;
         std::vector<U16> indices;
			
		public:
			Decimator(std::vector<Primitive>& faces, std::vector<U16>& indices, std::vector<Point>& verts);
			~Decimator();

         void ReduceMesh( S32 numFaces );

			void GetMesh(std::vector<Primitive>& p, std::vector<U16>& i) { p=primitives; i=indices; }
	};

}

#endif
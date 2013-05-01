//-----------------------------------------------------------------------------
// Decimator 
// based on "A Simple, Fast, and Effective Polygon Reduction Algorithm"
// by Stan Melax from Game Developer Magazine, November 1998
//-----------------------------------------------------------------------------

#pragma warning ( disable: 4786 4018 )

#include "DTSUtil.h"
#include "DTSDecimator.h"

namespace DTS
{
	static std::vector<DecimatorVertex *>   vertices;
	static std::vector<DecimatorTriangle *> triangles;

	DecimatorTriangle::DecimatorTriangle(DecimatorVertex *v0,DecimatorVertex *v1,DecimatorVertex *v2, S32 _type){
		assert(v0!=v1 && v1!=v2 && v2!=v0);
		vertex[0]=v0;
		vertex[1]=v1;
		vertex[2]=v2;
		ComputeNormal();
		triangles.push_back(this);
		for(S32 i=0;i<3;i++) {
			vertex[i]->face.push_back(this);
			for(S32 j=0;j<3;j++) if(i!=j) {
				addUniqueElement(vertex[i]->neighbor, vertex[j]);
			}
		}
      type = _type;
	}
	DecimatorTriangle::~DecimatorTriangle(){
		S32 i;
		delElement( triangles, this );
		for(i=0;i<3;i++) {
			if(vertex[i]) delElement(vertex[i]->face,this);
		}
		for(i=0;i<3;i++) {
			S32 i2 = (i+1)%3;
			if(!vertex[i] || !vertex[i2]) continue;
			vertex[i ]->RemoveIfNonNeighbor(vertex[i2]);
			vertex[i2]->RemoveIfNonNeighbor(vertex[i ]);
		}
	}
	S32 DecimatorTriangle::HasVertex(DecimatorVertex *v) {
		return (v==vertex[0] ||v==vertex[1] || v==vertex[2]);
	}
	void DecimatorTriangle::ComputeNormal(){
		Point v0=vertex[0]->position;
		Point v1=vertex[1]->position;
		Point v2=vertex[2]->position;
      Point e1 = v1 - v0;
      Point e2 = v2 - v1;
		crossProduct( e1, e2, &normal );
		normal.normalize();
	}
	void DecimatorTriangle::ReplaceVertex(DecimatorVertex *vold,DecimatorVertex *vnew) {
		assert(vold && vnew);
		assert(vold==vertex[0] || vold==vertex[1] || vold==vertex[2]);
		assert(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2]);
		if(vold==vertex[0]){
			vertex[0]=vnew;
		}
		else if(vold==vertex[1]){
			vertex[1]=vnew;
		}
		else {
			assert(vold==vertex[2]);
			vertex[2]=vnew;
		}
		S32 i;
		delElement(vold->face,this);
		assert(!containsElement(vnew->face,this));
		vnew->face.push_back(this);
		for(i=0;i<3;i++) {
			vold->RemoveIfNonNeighbor(vertex[i]);
			vertex[i]->RemoveIfNonNeighbor(vold);
		}
		for(i=0;i<3;i++) {
			assert(containsElement(vertex[i]->face,this)==1);
			for(S32 j=0;j<3;j++) if(i!=j) {
				addUniqueElement( vertex[i]->neighbor, vertex[j]);
			}
		}
		ComputeNormal();
	}

	DecimatorVertex::DecimatorVertex(Point v,S32 _id) {
		position =v;
		id=_id;
		vertices.push_back(this);
	}

	DecimatorVertex::~DecimatorVertex(){
		assert(face.size()==0);
		while(neighbor.size()) {
			delElement(neighbor[0]->neighbor,this);
			delElement(neighbor,neighbor[0]);
		}
		delElement(vertices,this);
	}
	void DecimatorVertex::RemoveIfNonNeighbor(DecimatorVertex *n) {
		// removes n from neighbor list if n isn't a neighbor.
		if(!containsElement(neighbor,n)) return;
		for(S32 i=0;i<face.size();i++) {
			if(face[i]->HasVertex(n)) return;
		}
		delElement(neighbor,n);
	}

   S32 DecimatorVertex::IsBorder()
   {
      S32 i,j;
      for(i=0;i<neighbor.size();i++)
      {
         S32 count=0;
         for(j=0;j<face.size();j++)
         {
            if(face[j]->HasVertex(neighbor[i]))
            {
               count++;
            }
         }
         assert(count>0);
         if(count==1)
         {
            return 1;
         }
      }
      return 0;
   } 

	F32 Decimator::ComputeEdgeCollapseCost(DecimatorVertex *src,DecimatorVertex *dest) {
		// if we collapse edge src/dest by moving src to dest then how 
		// much different will the model change, i.e. how much "error".
		// Texture, vertex normal, and border vertex code was removed
		// to keep this demo as simple as possible.
		// The method of determining cost was designed in order 
		// to exploit small and coplanar regions for
		// effective polygon reduction.
		// Is is possible to add some checks here to see if "folds"
		// would be generated.  i.e. normal of a remaining face gets
		// flipped.  I never seemed to run into this problem and
		// therefore never added code to detect this case.
		S32 i;
		Point p = dest->position - src->position;
		F32 edgelength = p.length();
		F32 curvature = 0;

		// find the "sides" triangles that are on the edge uv
		std::vector<DecimatorTriangle *> sides;
		for( i = 0; i < src->face.size(); i++ )
		{
			if( src->face[i]->HasVertex( dest ) )
			{
				sides.push_back( src->face[i] );
			}
		}

		if(src->IsBorder() )
		{
			if( sides.size() > 1 )
			{
				curvature = 1;
			}
			else
			{
				curvature = 1;
			}
		}
		else
		{
			// use the triangle facing most away from the sides 
			// to determine our curvature term
			for( i = 0; i < src->face.size(); i++ )
			{
				F32 mincurv = 1; // curve for face i and closer side to it
				for( S32 j = 0; j < sides.size(); j++ )
				{
					// use dot product of face normals. '^' defined in Point
					F32 dotprod = dotProduct( src->face[i]->normal, sides[j]->normal );
					mincurv = getmin( mincurv, ( 1.0f - dotprod ) * 0.5f );
				}
				curvature = getmax( curvature, mincurv );
			}
		}
/*
		// check for texture seam ripping
		S32 nomatch=0;
		for(i=0;i<src->face.num;i++) {
			for(S32 j=0;j<sides.num;j++) {
				// perhaps we should actually compare the positions in uv space
				if(src->face[i]->texat(src) == sides[j]->texat(src)) break;
			}
			if(j==sides.num) 
			{
				// we didn't find a triangle with edge uv that shares texture coordinates
				// with face i at vertex src
				nomatch++;
			}
		}
		if(nomatch) {
			curvature=1;
		}
*/
		return edgelength * curvature;
	}

	void Decimator::ComputeEdgeCostAtVertex(DecimatorVertex *v) {
		// compute the edge collapse cost for all edges that start
		// from vertex v.  Since we are only interested in reducing
		// the object by selecting the min cost edge at each step, we
		// only cache the cost of the least cost edge at this vertex
		// (in member variable collapse) as well as the value of the 
		// cost (in member variable objdist).
		if(v->neighbor.size()==0) {
			// v doesn't have neighbors so it costs nothing to collapse
			v->collapse=NULL;
			v->objdist=-0.01f;
			return;
		}
		v->objdist = 1000000;
		v->collapse=NULL;
		// search all neighboring edges for "least cost" edge
		for(S32 i=0;i<v->neighbor.size();i++) {
			F32 dist;
			dist = ComputeEdgeCollapseCost(v,v->neighbor[i]);
			if(dist<v->objdist) {
				v->collapse=v->neighbor[i];  // candidate for edge collapse
				v->objdist=dist;             // cost of the collapse
			}
		}
	}
	void Decimator::ComputeAllEdgeCollapseCosts() {
		// For all the edges, compute the difference it would make
		// to the model if it was collapsed.  The least of these
		// per vertex is cached in each vertex object.
		for(S32 i=0;i<vertices.size();i++) {
			ComputeEdgeCostAtVertex(vertices[i]);
		}
	}

	void Decimator::Collapse(DecimatorVertex *u,DecimatorVertex *v){
		// Collapse the edge uv by moving vertex u onto v
		// Actually remove tris on uv, then update tris that
		// have u to have v, and then remove u.
		if(!v) {
			// u is a vertex all by itself so just delete it
			delete u;
			return;
		}
		S32 i;
		std::vector<DecimatorVertex *>tmp;
		// make tmp a list of all the neighbors of u
		for(i=0;i<u->neighbor.size();i++) {
			tmp.push_back(u->neighbor[i]);
		}
		// delete triangles on edge uv:
		for(i=u->face.size()-1;i>=0;i--) {
			if(u->face[i]->HasVertex(v)) {
				delete(u->face[i]);
			}
		}
		// update remaining triangles to have v instead of u
		for(i=u->face.size()-1;i>=0;i--) {
			u->face[i]->ReplaceVertex(u,v);
		}
		delete u; 
		// recompute the edge collapse costs for neighboring vertices
		for(i=0;i<tmp.size();i++) {
			ComputeEdgeCostAtVertex(tmp[i]);
		}
	}

	DecimatorVertex *Decimator::MinimumCostEdge(){
		// Find the edge that when collapsed will affect model the least.
		// This function actually returns a Vertex, the second vertex
		// of the edge (collapse candidate) is stored in the vertex data.
		// Serious optimization opportunity here: this function currently
		// does a sequential search through an unsorted list :-(
		// Our algorithm could be O(n*lg(n)) instead of O(n*n)
		DecimatorVertex *mn=vertices[0];
		for(S32 i=0;i<vertices.size();i++) {
			if(vertices[i]->objdist < mn->objdist) {
				mn = vertices[i];
			}
		}
		return mn;
	}

    // Public interface

	Decimator::Decimator(std::vector<Primitive>& faces, std::vector<U16>& indices, std::vector<Point>& verts)
	{
      // Add the vertices
		for( S32 i = 0; i < verts.size(); i++ )
      {
			DecimatorVertex *v = new DecimatorVertex( verts[i], i );
		}

      // Add the faces
		for( S32 i = 0; i < faces.size(); i++ )
      {
			DecimatorTriangle *t=new DecimatorTriangle(
							  vertices[indices[faces[i].firstElement]],
							  vertices[indices[faces[i].firstElement+1]],
							  vertices[indices[faces[i].firstElement+2]],
                       faces[i].type );
      }

		ComputeAllEdgeCollapseCosts(); // cache all edge collapse costs
	}

	Decimator::~Decimator()
	{
      for( S32 i = triangles.size() - 1; i >= 0; i-- )
      {
         DecimatorTriangle *tmp = triangles[i];
         //triangles.Remove(tmp);
         delete tmp;
      }

      for( S32 i = vertices.size() - 1; i >= 0; i-- )
      {
         DecimatorVertex *tmp = vertices[i];
         //vertices.Remove(tmp);
         delete tmp;
      }
	}

   void Decimator::ReduceMesh( S32 faceCount )
	{
		// reduce the object
		while(triangles.size() > faceCount)
      {
			// get the next vertex to collapse
			DecimatorVertex *mn = MinimumCostEdge();

			// Collapse this edge
			Collapse(mn,mn->collapse);
		}

      primitives.empty();
      indices.empty();
      // Build our new primitive and index list
      for( S32 i = 0; i < triangles.size(); i++ )
      {
         if( triangles[i]->vertex[0] != NULL && triangles[i]->vertex[1] != NULL && triangles[i]->vertex[2] != NULL )
         {
            Primitive p;
            p.firstElement = indices.size();
            p.numElements = 3;
            p.type = triangles[i]->type;
            primitives.push_back( p );

            indices.push_back( triangles[i]->vertex[0]->id );
            indices.push_back( triangles[i]->vertex[1]->id );
            indices.push_back( triangles[i]->vertex[2]->id );
         }
      }
	}
	
}

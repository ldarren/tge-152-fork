//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef __FOG_CALC_H__
#define __FOG_CALC_H__

#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"


// This class is a fancy inlined implementation of the fog calculations with all invariants moved
//	into the constructor.
class FogCalc
{
	public:
		FogCalc( const PlaneF &dPlane, F32 distOffset, const Point3F &zVec, F32 worldZ, F32 worldPz, const SceneState *state )
		:	fc_distOffset( distOffset ),
			fc_newWorldZ( worldZ - worldPz ),
			fc_fogDistance( state->getFogDistance() ),
			fc_visibleDistance( state->getVisibleDistance() ),
			fc_fogScale( state->getFogScale() ),
			fc_PosFogBands( state->getPosFogBands() ),
			fc_NegFogBands( state->getNegFogBands() ),
			distPlane( dPlane ),
			osZVec( zVec ),
			sState( state )
		{
			// For Textured
			F32 heightOffset;

			gClientSceneGraph->getFogCoordData( tex_invVisibleDistance, heightOffset, tex_invHeightRange );

			tex_visibleDistanceMod = gClientSceneGraph->getVisibleDistanceMod() - distOffset - distPlane.d;
			tex_newWorldZ = worldZ - heightOffset;
		}
		
		inline F32	calcFC( const Point3F &point ) const
		{
         return( sState->getHazeAndFog(mFabs(distPlane.distToPlane(point)) + fc_distOffset, (mDot(point, osZVec) + fc_newWorldZ) ) );			
		}
		
		inline const Point2F	calcTextured( const Point3F &point ) const
		{
         // inline version of SceneGraph::getFogCoordPair
			return( Point2F(
						(tex_visibleDistanceMod - (point.x * distPlane.x + point.y * distPlane.y + point.z * distPlane.z)) * tex_invVisibleDistance,
						(tex_newWorldZ + point.x * osZVec.x + point.y * osZVec.y + point.z * osZVec.z) * tex_invHeightRange
					) );
		}

	private:
		const F32	fc_distOffset;
		const F32	fc_newWorldZ;
		const F32	fc_fogDistance;
		const F32	fc_visibleDistance;
		const F32	fc_fogScale;
		const Vector<SceneState::FogBand> *fc_PosFogBands;
		const Vector<SceneState::FogBand> *fc_NegFogBands;
				
		F32	tex_invVisibleDistance;
		F32   tex_invHeightRange;
		F32	tex_visibleDistanceMod;
		F32	tex_newWorldZ;
		
		const PlaneF distPlane;
		const Point3F osZVec;
		const SceneState *sState;
};

#endif //__OK_FOG_CALC_H__

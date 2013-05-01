//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// Copyright (c) 2003 Black Blood Studios
// Copyright (c) 2001 GarageGames.Com
// Portions Copyright (c) 2001 by Sierra Online, Inc.
//-----------------------------------------------------------------------------
// This release by: Xavier Amado (xavier@blackbloodstudios.com)
// Credits to : Vinci_Smurf, Loonatik, Matt Webster, Frank Bignone, Xavier Amado
//-----------------------------------------------------------------------------

#ifndef _GUIOBJECTVIEW_H_
#define _GUIOBJECTVIEW_H_

#ifndef _GUITSCONTROL_H_
#include "gui/core/guiTSControl.h"
#endif
#ifndef _TSSHAPEINSTANCE_H_
#include "ts/tsShapeInstance.h"
#endif
#ifndef _GAMEBASE_H_
#include "game/gameBase.h"
#endif
#ifndef _CAMERA_H_
#include "game/camera.h"
#endif

class TSThread;

class GuiObjectView : public GuiTSCtrl
{
	private:
		typedef GuiTSCtrl Parent;

	protected:
		enum MouseState
		{
			None,
			Rotating,
			Zooming
		};
		
		MouseState  mMouseState;
		
		bool mZoom;
		bool mSpin;

		Point3F  mCameraPos;
		MatrixF  mCameraMatrix;
		EulerF   mCameraRot;
		EulerF   mCameraOffset;
		Point3F  mOrbitPos;
		F32      mMinOrbitDist;
		F32      mOrbitDist;

		Point2I  mLastMousePoint;

		Point3F     mLightPos;
		VectorF     mLightDirection;
		ColorF      mLightColor;
		ColorF      mLightAmbient;

		struct meshObjects {
			private:

			public:
				meshObjects();
				~meshObjects();

				struct meshs {
					private:
						
					public:
						meshs();
						~meshs();

						TSShapeInstance* mesh;
						char name[32];
						U32 mode; // 0 = normal, 1 = animated
						S32 node;
						S32 parentNode;
						S32 parentIndex;
						U32 detail;
						TSThread *thread;
						S32 lastRenderTime;

						void loadDSQ(const char* dsq);
						void setSequence(const char* seq, F32 time);

				} mMesh[33];

				TSShapeInstance*  mMainObject;
				U32 mDetail;

				void load(S32 index, const char* name, const char* shape, const char* skin, S32 pIndex, S32 pNode, S32 detail);
				void unLoad(S32 index);

				S32 findOpen();
				S32 findMeshByName(const char* name);
				S32 findMeshByNode(const char* node);
				void Clear();
				
		} mMeshObjects;

	public:
		DECLARE_CONOBJECT( GuiObjectView );
		GuiObjectView();
		~GuiObjectView();

		static void consoleInit();
		static void initPersistFields();
		bool onWake();

		void onMouseDown( const GuiEvent &event );
		void onMouseUp( const GuiEvent &event );
		void onMouseDragged( const GuiEvent &event );
		void onRightMouseDown( const GuiEvent &event );
		void onRightMouseUp( const GuiEvent &event );
		void onRightMouseDragged( const GuiEvent &event );
		void setMouseOptions( bool zoom, bool spin );
		
		void setSkin(S32 skType, StringTableEntry texture);
		bool addSkinModifier(U32 slot, const char *skinLocation, const char *skinName, const char *maskName, 
			const ColorI& printColor1, const ColorI& printColor2, 
			const ColorI& printColor3, const ColorI& printColor4 );

		void setCamera();
		void setCamera(F32 rX, F32 rY, F32 rZ, F32 dist);
		void setCameraOffset(F32 oX, F32 oY, F32 oZ);
		bool processCameraQuery( CameraQuery *query );
		void renderWorld( const RectI &updateRect );
		void getObjectTransform( MatrixF *mat, S32 index);
		
		void loadObject(const char* name, const char* shape, const char* skin, const char* parentName, const char* nodeName, S32 detail);
		void unLoadObject(const char* name, const char* node);
		void loadDSQ(const char* name, const char* dsq);
		void setSequence(const char* name, const char* seq, F32 time);
		void Clear();
};

#endif // _GUIOBJECTVIEW_H_
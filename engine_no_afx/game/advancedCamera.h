//-----------------------------------------------------------------------------
// An advanced camera class capable of running in several modes
// The aim is to be able to replace the existing Torque camera 
// system and most camera resources as a single resource
//
// The modes currently implemented are:
// * Tracking            - Fixed position of camera and tracking an object
//                         Good for security cameras and similar
// * God View            - Looking down on the game world similar to Diablo. 
//                         Camera does not rotate with the player
// * Third Person        - Typical behind the player view. Camera rotates with the player looking down the player eye vector
//                         and is not controllable
// * Third Person Target - Behind the player view. Camera rotates to keep both player and target object in view
//                         and is not controllable
// * Orbit				 - Camera moves on a sphere around the player and can be rotated, tilted and zoomed in/out. 
//                         Thanks to Manoel Neto for implementing this and contributing to the resource
// * Static				 - Stationary camera
//
// For the 3rd person modes one can turn on/off that the camera stays level with the terrain
// Also for 3rd person mode enabling vertical freedom makes the camera move with the head of the player
//
// Initial work relies heavily upon the TrackingCamera resource posted by Cory Osborn
// Thanks to Stephen Zepp, Manoel Neto and Zik Saleeba for contributing code to this resource
//-----------------------------------------------------------------------------


//
// CHANGES IN THIS RELEASE
//
// Added better collision check with interiors and terrain
// and vertical freedom mode by Zik Saleeba
//
// Smooth orbit camera - fantastic code by Manoel Neto
//
// Static camera mode - needs a little work yet for better control
// A large cleanup. Code is much more readable now
// Removed old orbit camera by Stephen Zepp
// Reworked Manoel's interpolation code to work in all camera modes + transistions
// 


#ifndef _ADVANCED_CAMERA_H_
#define _ADVANCED_CAMERA_H_

#ifndef _SHAPEBASE_H_
#include "game/shapeBase.h"
#endif

//----------------------------------------------------------------------------
struct AdvancedCameraData: public ShapeBaseData {
	typedef ShapeBaseData Parent;

	Point3F lookAtOffset;       ///< Offset from the object to look at world center
	Point3F thirdPersonOffset;  ///< Offset from the player object when in 3rd person mode
	Point3F godViewOffset;      ///< Offset from the player object when in god view mode
	F32		maxTerrainDiff;		///< Max difference in old height to current for camera to change height
	Point3F orbitOffset;        ///< Current Orbit Mode offset. CAREFUL: these are spherical coords
								///< and should not be manipulated directly without conversion
	Point2F orbitMinMaxZoom;		///< Minimum and maximum amount of zoom allowed in orbit mode
	Point2F orbitMinMaxDeclination; ///< Minimum and maximum amount of declination allowed in orbit mode
	F32 damping;

	AdvancedCameraData();

	DECLARE_CONOBJECT(AdvancedCameraData);
	static void initPersistFields();
	virtual void packData(BitStream* stream);
	virtual void unpackData(BitStream* stream);
};

//----------------------------------------------------------------------------
/// Implements a camera object that can be moved in many different ways.
class AdvancedCamera: public ShapeBase {
	typedef ShapeBase Parent;
	AdvancedCameraData* mDataBlock;            ///< Datablock

	/// Bit masks for different types of events
	enum MaskBits {
		MoveMask     = Parent::NextFreeMask,
		NextFreeMask = Parent::NextFreeMask << 1
	};

	/// @name Interpolation data
	/// @{
	/// Client interpolation data
	struct StateDelta {
		Point3F pos;
		Point3F rot;
		VectorF posVec;
		VectorF rotVec;

		F32 mZoomDistance; 
		F32 mDeclination;
		F32 mAzimuth;
		F32 pZoomDistance; 
		F32 pDeclination;
		F32 pAzimuth;
	};
	static Point3F mCameraPos;             ///< Position of camera in tracking mode, ///DARREN: make it static bcos multiple AdvancedCameras been created
	Point3F mRot;                          ///< Current pitch/yaw angles
	StateDelta delta;                      ///< Used for interpolation on the client.  @see StateDelta
	/// @}

	F32 mZoomDistance;                     ///< Current orbit offset in SPHERICAL Coords
	F32 mDeclination;
	F32 mAzimuth;
	F32 mDamping;

	SimObjectPtr<GameBase> mPlayerObject;  ///< Points to the shapebase object we use
	///< in tracking and 3rd person mode

	SimObjectPtr<GameBase> mTargetObject;  ///< Points to the shapebase object we use
	///< in 3rd person target mode

	int mMode; ///< The mode the camera is in
	bool mFollowTerrain; ///< Keep camera level with terrain in 3rd person
	bool mVerticalFreedom; ///< Allow head pitch to control view in 3rd person

	Point3F mCurrentGodViewOffset;
	Point3F mCurrentThirdPersonOffset;
	Point3F mCurrentLookAtOffset;
	Point3F mCurrentOrbitOffset;

	Point2F mCurrentOrbitMinMaxZoom;
	Point2F mCurrentOrbitMinMaxDeclination;

	/// determines the Point we are looking at
	bool getLookAtPos(Point3F* lookAtPos);

	// Check for collision with terrain and interiors and return point of collision
	Point3F runCameraCollisionCheck(const Point3F& startpos, const Point3F& endpos);

	// Adjust camera height to follow terrain
	Point3F adjustCameraToTerrain(const Point3F& playerPos, const Point3F& cameraPos);

	/// @name Screen Flashing
	/// @{

	/// Returns the level of screenflash that should be used
	/// 
	/// When in Track mode, this will return the tracked object's damage flash
	F32 getDamageFlash() const;

	/// White out is the flash-grenade blindness effect
	/// This returns the level of flash to create
	///
	/// When in Track, this will return the tracked object's white out
	F32 getWhiteOut() const;

	F32 mYaw;
	F32 mPitch;
	F32 mZoom;

	/// @}

public:
	DECLARE_CONOBJECT(AdvancedCamera);

	AdvancedCamera();
	~AdvancedCamera();

	enum 
	{
		TrackMode               = 0,
		GodViewMode             = 1,
		ThirdPersonMode         = 2,
		ThirdPersonTargetMode   = 3,
		OrbitMode				= 4,
		StaticMode				= 5,

		CameraFirstMode = 0,
		CameraLastMode  = 5
	};


	static void initPersistFields();
	static void consoleInit();

	void onEditorEnable();
	void onEditorDisable();

	bool onAdd();
	void onRemove();
	bool onNewDataBlock(GameBaseData* dptr);

	void advanceTime(F32 dt);
	void processTick(const Move* move);
	void interpolateTick(F32 delta);
	void getCameraTransform(F32* pos,MatrixF* mat);
	void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *);

	void writePacketData(GameConnection *conn, BitStream *stream);
	void readPacketData(GameConnection *conn, BitStream *stream);
	U32  packUpdate(NetConnection *conn, U32 mask, BitStream *stream);
	void unpackUpdate(NetConnection *conn, BitStream *stream);
	void onDeleteNotify(SimObject *obj);

	Point3F &getPosition();
	Point3F &getRotation();
	void setPosition(const Point3F& pos);

	/// Position of camera when in tracking mode
	void setCameraPosition(const Point3F& pos);
	Point3F &getCameraPosition();

	/// Methods for setting the objects to track/view
	void setPlayerObject(GameBase *obj);
	GameBase* getPlayerObject();
	void setTargetObject(GameBase *obj);
	GameBase* getTargetObject();

	/// Methods for changing orbit mode location
	void processPlayerInput(int direction);

	/// Method to switch the camera mode
	void setCameraMode(int mode);

	/// Method to set terrain follow mode
	void setFollowTerrainMode(bool enable);

	/// Method to set vertical control of third person mode
	void setVerticalFreedomMode(bool enable);

	// Methods for adjusting offsets
	void setLookAtOffset(Point3F offset);
	void setThirdPersonOffset(Point3F offset);
	void setGodViewOffset(Point3F offset);
	void setOrbitOffset(Point3F offset);
	void setOrbitMinMaxZoom(Point2F zoom);
	void setOrbitMinMaxDeclination(Point2F declination);

	void updateMovementValues(F32 dt);

	Point3F &getLookAtOffset();
	Point3F &getThirdPersonOffset();
	Point3F &getGodViewOffset();
	Point3F &getOrbitOffset();
	Point2F &getOrbitMinMaxZoom();
	Point2F &getOrbitMinMaxDeclination();
};


#endif

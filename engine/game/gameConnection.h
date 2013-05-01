//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//          db-cache -- implementation of datablock caching system.
//        obj-select -- implementation of object selection used for spell targeting.
//          zoned-in -- connection is flagged as "zoned-in" when client is fully
//              connected and user can interact with it.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _GAMECONNECTION_H_
#define _GAMECONNECTION_H_

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif
#ifndef _SHAPEBASE_H_
#include "game/shapeBase.h"
#endif
#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif
#ifndef _MOVEMANAGER_H_
#include "game/moveManager.h"
#endif
#ifndef _BITVECTOR_H_
#include "core/bitVector.h"
#endif

enum GameConnectionConstants
{
   MaxClients = 126,
   DataBlockQueueCount = 16
};

class AudioProfile;
class MatrixF;
class MatrixF;
class Point3F;
class MoveManager;
struct Move;
struct AuthInfo;

#define GameString "Torque Game Engine Demo"

// AFX CODE BLOCK (db-cache) <<
//
// To disable datablock caching, remove or comment out the AFX_CAP_DATABLOCK_CACHE define below.
// Also, at a minimum, the following script preferences should be set to false:
//   $pref::Client::EnableDatablockCache = false; (in arcane.fx/client/defaults.cs)
//   $Pref::Server::EnableDatablockCache = false; (in arcane.fx/server/defaults.cs)
// Alternatively, all script code marked with "DATABLOCK CACHE CODE" can be removed or
// commented out.
//
#define AFX_CAP_DATABLOCK_CACHE
// AFX CODE BLOCK (db-cache) >>

class GameConnection : public NetConnection
{
private:
   typedef NetConnection Parent;

   enum PrivateConstants {
      MoveCountBits = 5,
      /// MaxMoveCount should not exceed the MoveManager's
      /// own maximum (MaxMoveQueueSize)
      MaxMoveCount = 30,
   };
   typedef Vector<Move> MoveList;

   SimObjectPtr<ShapeBase> mControlObject;
   SimObjectPtr<ShapeBase> mCameraObject;
   U32 mDataBlockSequence;
   char mDisconnectReason[256];

   U32  mMissionCRC;             // crc of the current mission file from the server

private:
   U32 mLastControlRequestTime;
   S32 mDataBlockModifiedKey;
   S32 mMaxDataBlockModifiedKey;

   /// @name Client side first/third person
   /// @{

   ///
   bool  mFirstPerson;     ///< Are we currently first person or not.
   bool  mUpdateFirstPerson; ///< Set to notify client or server of first person change.
   bool  mUpdateCameraFov; ///< Set to notify server of camera FOV change.
   F32   mCameraFov;       ///< Current camera fov (in degrees).
   F32   mCameraPos;       ///< Current camera pos (0-1).
   F32   mCameraSpeed;     ///< Camera in/out speed.
   /// @}

   /// @name Move Packets
   /// Write/read move data to the packet.
   /// @{

   ///
   void moveWritePacket(BitStream *bstream);
   void moveReadPacket(BitStream *bstream);
   /// @}
public:

   /// @name Protocol Versions
   ///
   /// Protocol versions are used to indicated changes in network traffic.
   /// These could be changes in how any object transmits or processes
   /// network information. You can specify backwards compatibility by
   /// specifying a MinRequireProtocolVersion.  If the client
   /// protocol is >= this min value, the connection is accepted.
   ///
   /// Torque (V12) SDK 1.0 uses protocol  =  1
   ///
   /// Torque SDK 1.1 uses protocol = 2
   /// Torque SDK 1.4 uses protocol = 12
   /// @{
   static const U32 CurrentProtocolVersion;
   static const U32 MinRequiredProtocolVersion;
   /// @}

   /// Configuration
   enum Constants {
      BlockTypeMove = NetConnectionBlockTypeCount,
      GameConnectionBlockTypeCount,
      MaxConnectArgs = 16,
      DataBlocksDone = NumConnectionMessages,
      DataBlocksDownloadDone,
   };

   /// Set connection arguments; these are passed to the server when we connect.
   void setConnectArgs(U32 argc, const char **argv);
   const char * getConnectArgs(S32 idx) const;
   S32 getConnectArgsCount() const { return mConnectArgc;}

   /// Set the server password to use when we join.
   void setJoinPassword(const char *password);

   /// @name Event Handling
   /// @{

   virtual void onTimedOut();
   virtual void onConnectTimedOut();
   virtual void onDisconnect(const char *reason);
   virtual void onConnectionRejected(const char *reason);
   virtual void onConnectionEstablished(bool isInitiator);
   virtual void handleStartupError(const char *errorString);
   /// @}

   /// @name Packet I/O
   /// @{

   virtual void writeConnectRequest(BitStream *stream);
   virtual bool readConnectRequest(BitStream *stream, const char **errorString);
   virtual void writeConnectAccept(BitStream *stream);
   virtual bool readConnectAccept(BitStream *stream, const char **errorString);
   /// @}

   bool canRemoteCreate();

private:
   /// @name Connection State
   /// This data is set with setConnectArgs() and setJoinPassword(), and
   /// sent across the wire when we connect.
   /// @{

   U32      mConnectArgc;
   char *mConnectArgv[MaxConnectArgs];
   char *mJoinPassword;
   /// @}

protected:
   struct GamePacketNotify : public NetConnection::PacketNotify
   {
      S32 cameraFov;
      GamePacketNotify();
   };
   PacketNotify *allocNotify();

   U32 mLastMoveAck;
   U32 mLastClientMove;
   U32 mFirstMoveIndex;
   U32 mMoveCredit;
   U32 mLastControlObjectChecksum;

   Vector<SimDataBlock *> mDataBlockLoadList;

   MoveList    mMoveList;
   bool        mAIControlled;
   AuthInfo *  mAuthInfo;

   static S32  mLagThresholdMS;
   S32         mLastPacketTime;
   bool        mLagging;

   /// @name Flashing
   ////
   /// Note, these variables are not networked, they are for the local connection only.
   /// @{
   F32 mDamageFlash;
   F32 mWhiteOut;

   F32   mBlackOut;
   S32   mBlackOutTimeMS;
   S32   mBlackOutStartTimeMS;
   bool  mFadeToBlack;

   /// @}

   /// @name Packet I/O
   /// @{

   void readPacket      (BitStream *bstream);
   void writePacket     (BitStream *bstream, PacketNotify *note);
   void packetReceived  (PacketNotify *note);
   void packetDropped   (PacketNotify *note);
   void connectionError (const char *errorString);

   void writeDemoStartBlock   (ResizeBitStream *stream);
   bool readDemoStartBlock    (BitStream *stream);
   void handleRecordedBlock   (U32 type, U32 size, void *data);
   /// @}

public:

   DECLARE_CONOBJECT(GameConnection);
   void handleConnectionMessage(U32 message, U32 sequence, U32 ghostCount);
   void preloadDataBlock(SimDataBlock *block);
   void fileDownloadSegmentComplete();
   void preloadNextDataBlock(bool hadNew);
   static void consoleInit();

   void setDisconnectReason(const char *reason);
   GameConnection();
   ~GameConnection();

   U32 getDataBlockSequence() { return mDataBlockSequence; }
   void setDataBlockSequence(U32 seq) { mDataBlockSequence = seq; }

   bool onAdd();
   void onRemove();

   static GameConnection *getConnectionToServer() { return dynamic_cast<GameConnection*>((NetConnection *) mServerConnection); }
   static GameConnection *getLocalClientConnection() { return dynamic_cast<GameConnection*>((NetConnection *) mLocalClientConnection); }

   /// @name Control object
   /// @{

   ///
   void setControlObject(ShapeBase *co);
   ShapeBase* getControlObject()  { return  mControlObject; }
   void setCameraObject(ShapeBase *co);
   ShapeBase* getCameraObject();
   bool getControlCameraTransform(F32 dt,MatrixF* mat);
   bool getControlCameraVelocity(Point3F *vel);

   bool getControlCameraFov(F32 * fov);
   bool setControlCameraFov(F32 fov);
   bool isValidControlCameraFov(F32 fov);
   
   void setFirstPerson(bool firstPerson);
   
   /// @}

   void detectLag();

   /// @name Datablock management
   /// @{

   S32  getDataBlockModifiedKey     ()  { return mDataBlockModifiedKey; }
   void setDataBlockModifiedKey     (S32 key)  { mDataBlockModifiedKey = key; }
   S32  getMaxDataBlockModifiedKey  ()  { return mMaxDataBlockModifiedKey; }
   void setMaxDataBlockModifiedKey  (S32 key)  { mMaxDataBlockModifiedKey = key; }
   /// @}

   /// @name Fade control
   /// @{

   F32 getDamageFlash() { return mDamageFlash; }
   F32 getWhiteOut() { return mWhiteOut; }

   void setBlackOut(bool fadeToBlack, S32 timeMS);
   F32  getBlackOut();
   /// @}

   /// @name Move Management
   /// @{

   void           pushMove(const Move &mv);
   bool           getNextMove(Move &curMove);
   bool           isBacklogged();
   virtual void   getMoveList(Move**,U32* numMoves);
   virtual void   clearMoves(U32 count);
   void           collectMove(U32 simTime);
   virtual bool   areMovesPending();
   void           incMoveCredit(U32 count);
   /// @}

   /// @name Authentication
   ///
   /// This is remnant code from Tribes 2.
   /// @{

   void            setAuthInfo(const AuthInfo *info);
   const AuthInfo *getAuthInfo();
   /// @}

   /// @name Sound
   /// @{

   void play2D(const AudioProfile *profile);
   void play3D(const AudioProfile *profile, const MatrixF *transform);
   /// @}

   /// @name Misc.
   /// @{

   bool isFirstPerson()  { return mCameraPos == 0; }
   bool isAIControlled() { return mAIControlled; }

   void doneScopingScene();
   void demoPlaybackComplete();

   void setMissionCRC(U32 crc)           { mMissionCRC = crc; }
   U32  getMissionCRC()           { return(mMissionCRC); }
   /// @}

  // AFX CODE BLOCK (obj-select) <<
  // GameConnection is modified to keep track of object selections which are used in
  // spell targeting. This code stores the current object selection as well as the
  // current rollover object beneath the cursor. The rollover object is treated as a
  // pending object selection and actual object selection is usually made by promoting
  // the rollover object to the current object selection.
  private:   
    SimObjectPtr<SimObject> mRolloverObj;  
    SimObjectPtr<SimObject> mPreSelectedObj;  
    SimObjectPtr<SimObject> mSelectedObj;  
	SimObjectPtr<SimObject> mDestObj; //DARREN MOD: mouse click movement
    bool          mChangedSelectedObj;
    U32           mPreSelectTimestamp;
	Point3F		  mDestPoint; //DARREN MOD: mouse click movement
	Point3F		  mOldDestPoint;
	VectorF		  mDestNormal;
	S8			  mDestChanged;//DARREN MOD: 0: no update 1: updated -1: ready to stop

  protected:
    virtual void  onDeleteNotify(SimObject*);
  public:   
    void          setRolloverObj(SimObject*);   
	void		  setDestPointNormal(SimObject* dest, const Point3F & point, const VectorF & normal);
	SimObject*	  getRolloverObj() { return  mRolloverObj; }   
    void          setSelectedObj(SimObject*, bool propagate_to_client=false);   
    void          setDestObj(SimObject*, bool);   
    SimObject*	  getSelectedObj() { return  mSelectedObj; }  
    void          setPreSelectedObjFromRollover();
    void          clearPreSelectedObj();
    void          setSelectedObjFromPreSelected();
  // AFX CODE BLOCK (obj-select) >>
  // AFX CODE BLOCK (zoned-in) <<
  // Flag is added to indicate when a client is fully connected or "zoned-in". 
  // This information determines when AFX will startup active effects on a newly
  // added client. 
  private:
    bool          zoned_in;
  public:
    bool          isZonedIn() const { return zoned_in; }
    void          setZonedIn() { zoned_in = true; }
  // AFX CODE BLOCK (zoned-in) >>
#ifdef AFX_CAP_DATABLOCK_CACHE // AFX CODE BLOCK (db-cache) <<
  private:
    static StringTableEntry  server_cache_filename;
    static StringTableEntry  client_cache_filename;
    static bool   server_cache_on;
    static bool   client_cache_on;
    BitStream*    client_db_stream;
    char*         curr_stringBuf;
    U32           server_cache_CRC;
  public:
    void          repackClientDatablock(BitStream*, S32 start_pos);
    void          saveDatablockCache(bool on_server);
    void          loadDatablockCache();
    bool          loadDatablockCache_Begin();
    bool          loadDatablockCache_Continue();
    void          tempDisableStringBuffering(BitStream* bs) const;
    void          restoreStringBuffering(BitStream* bs) const;
    void          setServerCacheCRC(U32 crc) { server_cache_CRC = crc; }

    static void   resetDatablockCache();
    static bool   serverCacheEnabled() { return server_cache_on; }
    static bool   clientCacheEnabled() { return client_cache_on; }
    static const char* serverCacheFilename() { return server_cache_filename; }
    static const char* clientCacheFilename() { return client_cache_filename; }
#endif // AFX CODE BLOCK (db-cache) >>
};

#endif

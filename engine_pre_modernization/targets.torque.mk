EXE_NAME=torqueDemo
EXE_DEDICATED_NAME=$(EXE_NAME)d
BIN_DIRECTORY=../example
CHECK_LINK_FILE=../lib/xiph/linux/checklinks.sh

SOURCE.AUDIO=\
	audio/audio.cc \
	audio/audioBuffer.cc \
	audio/audioDataBlock.cc \
	audio/audioFunctions.cc \
	audio/audioStreamSourceFactory.cc \
	audio/oggMixedStreamSource.cc \
	audio/vorbisStream.cc \
	audio/vorbisStreamSource.cc \
	audio/wavStreamSource.cc \

SOURCE.COLLISION=\
	collision/abstractPolyList.cc \
	collision/boxConvex.cc \
	collision/clippedPolyList.cc \
	collision/concretePolyList.cc \
	collision/convex.cc \
	collision/convexBrush.cc \
	collision/depthSortList.cc \
	collision/earlyOutPolyList.cc \
	collision/extrudedPolyList.cc \
	collision/gjk.cc \
	collision/optimizedPolyList.cc \
	collision/planeExtractor.cc \
	collision/polyhedron.cc \
	collision/polytope.cc \

SOURCE.CONSOLE=\
	console/astAlloc.cc \
	console/astNodes.cc \
	console/BASscan.cc \
	console/BASgram.cc \
	console/codeBlock.cc \
	console/compiledEval.cc  \
	console/compiler.cc \
	console/console.cc \
	console/consoleDoc.cc \
	console/consoleFunctions.cc \
	console/consoleInternal.cc \
	console/consoleLogger.cc \
	console/consoleObject.cc \
	console/consoleParser.cc \
	console/consoleTypes.cc \
	console/cmdgram.cc \
	console/CMDscan.cc \
	console/dynamicTypes.cc \
	console/scriptObject.cc \
	console/simBase.cc \
	console/simDictionary.cc \
	console/simManager.cc \
	console/stringStack.cc \
	console/telnetConsole.cc \
	console/telnetDebugger.cc \
	console/typeValidators.cc \

SOURCE.CONSTRUCTOR=\
    constructor/constructorSimpleMesh.cc
    
SOURCE.CORE=\
	core/bitRender.cc \
	core/bitStream.cc \
	core/bitTables.cc \
	core/crc.cc \
	core/chunkFile.cc \
	core/dataChunker.cc \
	core/dnet.cc \
	core/fileObject.cc \
	core/fileStream.cc \
	core/filterStream.cc \
	core/findMatch.cc \
	core/frameAllocator.cc \
	core/idGenerator.cc \
	core/iTickable.cc \
	core/memStream.cc \
	core/nStream.cc \
	core/nTypes.cc \
	core/resDictionary.cc \
	core/resizeStream.cc \
	core/resManager.cc \
	core/stringTable.cc \
	core/stringBuffer.cc \
	core/tagDictionary.cc \
	core/tokenizer.cc \
	core/tVector.cc \
	core/zipAggregate.cc \
	core/zipHeaders.cc \
	core/zipSubStream.cc \
        core/unicode.cc \
	core/theoraPlayer.cc \

SOURCE.DGL=\
	dgl/bitmapBm8.cc \
	dgl/bitmapBmp.cc \
	dgl/bitmapGif.cc \
	dgl/bitmapJpeg.cc \
	dgl/bitmapPng.cc \
	dgl/dgl.cc \
	dgl/dglMatrix.cc \
	dgl/gBitmap.cc \
	dgl/gFont.cc \
	dgl/gPalette.cc \
	dgl/gTexManager.cc \
	dgl/gVectorField.cc \
	dgl/lensFlare.cc \
	dgl/materialList.cc \
	dgl/materialPropertyMap.cc \
	dgl/splineUtil.cc \
	dgl/stripCache.cc \
	dgl/gDynamicTexture.cc \

SOURCE.EDITOR=\
	editor/creator.cc \
	editor/editor.cc \
	editor/editTSCtrl.cc \
	editor/guiTerrPreviewCtrl.cc \
	editor/missionAreaEditor.cc \
	editor/terraformer.cc \
	editor/terraformerNoise.cc \
	editor/terraformerTexture.cc \
	editor/terrainActions.cc \
	editor/terrainEditor.cc \
	editor/worldEditor.cc \

SOURCE.GUI=\
	gui/containers/guiCtrlArrayCtrl.cc \
	gui/containers/guiFrameCtrl.cc \
	gui/containers/guiPaneCtrl.cc \
	gui/containers/guiScrollCtrl.cc \
	gui/containers/guiStackCtrl.cc \
	gui/containers/guiWindowCtrl.cc \
	gui/containers/guiTabBookCtrl.cc \
	gui/controls/guiBackgroundCtrl.cc \
	gui/controls/guiBitmapBorderCtrl.cc \
	gui/controls/guiBitmapButtonCtrl.cc \
	gui/controls/guiBitmapCtrl.cc \
	gui/controls/guiBorderButton.cc \
	gui/controls/guiButtonBaseCtrl.cc \
	gui/controls/guiButtonCtrl.cc \
	gui/controls/guiCheckBoxCtrl.cc \
	gui/controls/guiColorPicker.cc \
	gui/controls/guiConsole.cc \
	gui/controls/guiConsoleEditCtrl.cc \
	gui/controls/guiConsoleTextCtrl.cc \
	gui/controls/guiMLTextCtrl.cc \
	gui/controls/guiMLTextEditCtrl.cc \
	gui/controls/guiPopUpCtrl.cc \
	gui/controls/guiRadioCtrl.cc \
	gui/controls/guiSliderCtrl.cc \
	gui/controls/guiTextCtrl.cc \
	gui/controls/guiTextEditCtrl.cc \
	gui/controls/guiTextEditSliderCtrl.cc \
	gui/controls/guiTextListCtrl.cc \
	gui/controls/guiTreeViewCtrl.cc \
	gui/controls/guiTabPageCtrl.cc \
	gui/controls/guiListBoxCtrl.cc \
        gui/controls/guiDirectoryFileListCtrl.cc \
        gui/controls/guiDirectoryTreeCtrl.cc \
	gui/core/guiArrayCtrl.cc \
	gui/core/guiCanvas.cc \
	gui/core/guiControl.cc \
	gui/core/guiDefaultControlRender.cc \
	gui/core/guiTSControl.cc \
	gui/core/guiTypes.cc \
	gui/editor/guiControlListPopup.cc \
	gui/editor/guiDebugger.cc \
	gui/editor/guiEditCtrl.cc \
	gui/editor/guiFilterCtrl.cc \
	gui/editor/guiGraphCtrl.cc \
	gui/editor/guiInspector.cc \
	gui/editor/guiInspectorTypes.cc \
	gui/editor/guiMenuBar.cc \
	gui/game/guiAviBitmapCtrl.cc \
	gui/game/guiChunkedBitmapCtrl.cc \
	gui/game/guiFadeinBitmapCtrl.cc \
	gui/game/guiMessageVectorCtrl.cc \
	gui/game/guiProgressCtrl.cc \
	gui/utility/guiBubbleTextCtrl.cc \
	gui/utility/guiInputCtrl.cc \
	gui/utility/guiMouseEventCtrl.cc \
	gui/utility/messageVector.cc \
	gui/utility/guiTransitionCtrl.cc \
   gui/shiny/guiTheoraCtrl.cc \
	gui/shiny/guiEffectCanvas.cc \
	gui/shiny/guiTickCtrl.cc \

SOURCE.I18N=\
   i18n/lang.cc \
   i18n/i18n.cc

SOURCE.INTERIOR=\
	interior/floorPlanRes.cc \
	interior/forceField.cc \
	interior/interior.cc \
	interior/interiorCollision.cc \
	interior/interiorDebug.cc \
	interior/interiorInstance.cc \
	interior/interiorIO.cc \
	interior/interiorLightAnim.cc \
	interior/interiorLMManager.cc \
	interior/interiorMapRes.cc \
	interior/interiorRender.cc \
	interior/interiorRes.cc \
	interior/interiorResObjects.cc \
	interior/interiorSubObject.cc \
	interior/itfdump.asm \
	interior/interiorMap.cc \
	interior/lightUpdateGrouper.cc \
	interior/mirrorSubObject.cc \
	interior/pathedInterior.cc

SOURCE.MATH=\
	math/mBox.cc \
	math/mConsoleFunctions.cc \
	math/mMathFn.cc \
	math/mMath_C.cc \
	math/mMatrix.cc \
	math/mPlaneTransformer.cc \
	math/mQuadPatch.cc \
	math/mQuat.cc \
	math/mRandom.cc \
	math/mSolver.cc \
	math/mSplinePatch.cc \
	math/mathTypes.cc \
	math/mathUtils.cc \
	math/mMathAMD.cc \
	math/mMathAMD_ASM.asm \
	math/mMathSSE.cc \
	math/mMathSSE_ASM.asm

SOURCE.PLATFORM=\
	platform/platform.cc \
	platform/gameInterface.cc \
	platform/platformAssert.cc \
	platform/platformCPU.cc \
	platform/platformCPUInfo.asm \
	platform/platformFileIO.cc \
	platform/platformMemory.cc \
	platform/platformRedBook.cc \
	platform/platformString.cc \
	platform/platformVideo.cc \
	platform/profiler.cc 

SOURCE.PLATFORMWIN32=\
	platformWin32/winAsmBlit.cc \
	platformWin32/winCPUInfo.cc \
	platformWin32/winConsole.cc \
	platformWin32/winD3DVideo.cc \
	platformWin32/winDInputDevice.cc \
	platformWin32/winDirectInput.cc \
	platformWin32/winFileio.cc \
	platformWin32/winFont.cc \
	platformWin32/winGL.cc \
	platformWin32/winGLSpecial.cc \
	platformWin32/winInput.cc \
	platformWin32/winMath.cc \
	platformWin32/winMath_ASM.cc \
	platformWin32/winMemory.cc \
	platformWin32/winMutex.cc \
	platformWin32/winNet.cc \
	platformWin32/winOGLVideo.cc \
	platformWin32/winOpenAL.cc \
	platformWin32/winProcessControl.cc \
	platformWin32/winRedbook.cc \
	platformWin32/winSemaphore.cc \
	platformWin32/winStrings.cc \
	platformWin32/winThread.cc \
	platformWin32/winTime.cc \
	platformWin32/winV2Video.cc \
	platformWin32/winWindow.cc \

SOURCE.SIM=\
	sim/actionMap.cc \
	sim/connectionStringTable.cc \
	sim/decalManager.cc \
	sim/netConnection.cc \
	sim/netDownload.cc \
	sim/netEvent.cc \
	sim/netGhost.cc \
	sim/netInterface.cc \
	sim/netObject.cc \
	sim/netStringTable.cc \
	sim/pathManager.cc \
	sim/sceneObject.cc \
	sim/simPath.cc \

SOURCE.GAME=\
	game/aiClient.cc \
	game/aiConnection.cc \
	game/aiPlayer.cc \
	game/aiWheeledVehicle.cc \
	game/ambientAudioManager.cc \
	game/audioEmitter.cc \
	game/badWordFilter.cc \
	game/banList.cc \
	game/camera.cc \
	game/cameraSpline.cc \
	game/collisionTest.cc \
	game/debris.cc \
	game/debugView.cc \
	game/fireballAtmosphere.cc \
	game/game.cc \
	game/gameBase.cc \
	game/gameConnection.cc \
	game/gameConnectionEvents.cc \
	game/gameConnectionMoves.cc \
	game/gameFunctions.cc \
	game/gameProcess.cc \
	game/gameTSCtrl.cc \
	game/guiNoMouseCtrl.cc \
	game/guiPlayerView.cc \
	game/item.cc \
	game/main.cc \
	game/missionArea.cc \
	game/missionMarker.cc \
	game/pathCamera.cc \
	game/physicalZone.cc \
	game/player.cc \
	game/projectile.cc \
	game/rigid.cc \
	game/rigidShape.cc \
	game/scopeAlwaysShape.cc \
	game/shadow.cc \
	game/shapeBase.cc \
	game/shapeCollision.cc \
	game/shapeImage.cc \
	game/showTSShape.cc \
	game/sphere.cc \
	game/staticShape.cc \
	game/trigger.cc \
	game/tsStatic.cc \
	game/version.cc \

SOURCE.GAME.NET=\
	game/net/httpObject.cc \
	game/net/net.cc \
	game/net/netTest.cc \
	game/net/serverQuery.cc \
	game/net/tcpObject.cc \

SOURCE.GAME.FPS=\
	game/fps/guiClockHud.cc \
	game/fps/guiCrossHairHud.cc \
	game/fps/guiHealthBarHud.cc \
	game/fps/guiShapeNameHud.cc \

SOURCE.GAME.FX=\
	game/fx/cameraFXMgr.cc \
	game/fx/explosion.cc \
	game/fx/fxFoliageReplicator.cc \
	game/fx/fxLight.cc \
	game/fx/fxRenderObject.cc \
	game/fx/fxShapeReplicator.cc \
	game/fx/fxSunLight.cc \
	game/fx/lightning.cc \
	game/fx/particleEmitter.cc \
	game/fx/particleEngine.cc \
	game/fx/precipitation.cc \
	game/fx/splash.cc \
	game/fx/underLava.cc \
	game/fx/weatherLightning.cpp

SOURCE.GAME.VEHICLES=\
	game/vehicles/flyingVehicle.cc \
	game/vehicles/guiSpeedometer.cc \
	game/vehicles/hoverVehicle.cc \
	game/vehicles/vehicle.cc \
	game/vehicles/vehicleBlocker.cc \
	game/vehicles/wheeledVehicle.cc \


SOURCE.UTIL=\
   util/frustrumCuller.cpp \
   util/quadTreeTracer.cpp \
   util/rectClipper.cpp \
   util/triBoxCheck.cpp \
   util/triRayCheck.cpp

SOURCE.PLATFORMX86UNIX=\
	platform/platformNetAsync.cc \
	platformX86UNIX/x86UNIXAsmBlit.cc \
	platformX86UNIX/x86UNIXCPUInfo.cc \
	platformX86UNIX/x86UNIXConsole.cc \
	platformX86UNIX/x86UNIXFileio.cc \
	platformX86UNIX/x86UNIXFont.cc \
	platformX86UNIX/x86UNIXGL.cc \
	platformX86UNIX/x86UNIXInput.cc \
	platformX86UNIX/x86UNIXInputManager.cc \
	platformX86UNIX/x86UNIXIO.cc \
	platformX86UNIX/x86UNIXMath.cc \
	platformX86UNIX/x86UNIXMath_ASM.cc \
	platformX86UNIX/x86UNIXMemory.cc \
	platformX86UNIX/x86UNIXMessageBox.cc \
	platformX86UNIX/x86UNIXMutex.cc \
	platformX86UNIX/x86UNIXNet.cc \
	platformX86UNIX/x86UNIXOGLVideo.cc \
	platformX86UNIX/x86UNIXOpenAL.cc \
	platformX86UNIX/x86UNIXProcessControl.cc \
	platformX86UNIX/x86UNIXRedbook.cc \
	platformX86UNIX/x86UNIXSemaphore.cc \
	platformX86UNIX/x86UNIXStrings.cc \
	platformX86UNIX/x86UNIXThread.cc \
	platformX86UNIX/x86UNIXTime.cc \
	platformX86UNIX/x86UNIXWindow.cc \
	platformX86UNIX/x86UNIXUtils.cc 

SOURCE.PLATFORMX86UNIXDEDICATED=\
	platform/platformNetAsync.cc \
	platformX86UNIX/x86UNIXCPUInfo.cc \
	platformX86UNIX/x86UNIXConsole.cc \
	platformX86UNIX/x86UNIXDedicatedStub.cc \
	platformX86UNIX/x86UNIXFileio.cc \
	platformX86UNIX/x86UNIXIO.cc \
	platformX86UNIX/x86UNIXMath.cc \
	platformX86UNIX/x86UNIXMath_ASM.cc \
	platformX86UNIX/x86UNIXMemory.cc \
	platformX86UNIX/x86UNIXMutex.cc \
	platformX86UNIX/x86UNIXNet.cc \
	platformX86UNIX/x86UNIXProcessControl.cc \
	platformX86UNIX/x86UNIXSemaphore.cc \
	platformX86UNIX/x86UNIXStrings.cc \
	platformX86UNIX/x86UNIXThread.cc \
	platformX86UNIX/x86UNIXTime.cc \
	platformX86UNIX/x86UNIXWindow.cc \
	platformX86UNIX/x86UNIXUtils.cc 

SOURCE.PLATFORMLINUX=$(SOURCE.PLATFORMX86UNIX)
SOURCE.PLATFORMLINUXDEDICATED=$(SOURCE.PLATFORMX86UNIXDEDICATED)

SOURCE.PLATFORMOpenBSD=$(SOURCE.PLATFORMX86UNIX)
SOURCE.PLATFORMOpenBSDDEDICATED=$(SOURCE.PLATFORMX86UNIXDEDICATED)

SOURCE.PLATFORMFreeBSD=$(SOURCE.PLATFORMX86UNIX)
SOURCE.PLATFORMFreeBSDDEDICATED=$(SOURCE.PLATFORMX86UNIXDEDICATED)

SOURCE.SCENEGRAPH=\
	sceneGraph/detailManager.cc \
	sceneGraph/lightManager.cc \
	sceneGraph/sceneGraph.cc \
	sceneGraph/sceneLighting.cc \
	sceneGraph/sceneRoot.cc \
	sceneGraph/sceneState.cc \
	sceneGraph/sceneTraversal.cc \
	sceneGraph/sgUtil.cc \
	sceneGraph/shadowVolumeBSP.cc \
	sceneGraph/windingClipper.cc 

SOURCE.TERRAIN=\
	terrain/fluidQuadTree.cc \
	terrain/fluidRender.cc \
	terrain/fluidSupport.cc \
	terrain/sky.cc \
	terrain/sun.cc \
	terrain/blender.cc \
	terrain/blender_asm.asm \
	terrain/bvQuadTree.cc \
	terrain/terrCollision.cc \
	terrain/terrData.cc \
	terrain/terrLighting.cc \
	terrain/terrRender.cc \
	terrain/waterBlock.cc 

SOURCE.TS=\
	ts/tsAnimate.cc \
	ts/tsCollision.cc \
	ts/tsDecal.cc \
	ts/tsDump.cc \
	ts/tsIntegerSet.cc \
	ts/tsLastDetail.cc \
	ts/tsMaterialList.cc \
	ts/tsMesh.cc \
	ts/tsPartInstance.cc \
	ts/tsShape.cc \
	ts/tsShapeAlloc.cc \
	ts/tsShapeConstruct.cc \
	ts/tsShapeInstance.cc \
	ts/tsShapeOldRead.cc \
	ts/tsSortedMesh.cc \
	ts/tsThread.cc \
	ts/tsTransform.cc  
	
SOURCE.LIGHTINGSYSTEM=\
	lightingSystem/sgDecalProjector.cc \
	lightingSystem/sgDetailMapping.cc \
	lightingSystem/sgLightingModel.cc \
	lightingSystem/sgLightManager.cc \
	lightingSystem/sgLightMap.cc \
	lightingSystem/sgLightObject.cc \
	lightingSystem/sgMissionLightingFilter.cc \
	lightingSystem/sgObjectShadows.cc \
	lightingSystem/sgSceneLighting.cc \
	lightingSystem/sgSceneLightingInterior.cc \
	lightingSystem/sgSceneLightingTerrain.cc \
	lightingSystem/sgScenePersist.cc \
	lightingSystem/volLight.cc

# jmq: added the stuff after SOURCE.TS for tools build hack
SOURCE.ENGINE =\
	$(SOURCE.COLLISION) \
	$(SOURCE.CONSOLE) \
	$(SOURCE.CONSTRUCTOR) \
	$(SOURCE.CORE) \
	$(SOURCE.DGL) \
	$(SOURCE.I18N) \
	$(SOURCE.INTERIOR) \
	$(SOURCE.MATH) \
	$(SOURCE.PLATFORM) \
	$(SOURCE.SCENEGRAPH) \
	$(SOURCE.SIM) \
	$(SOURCE.TERRAIN) \
	$(SOURCE.TS) \
	$(SOURCE.AUDIO) \
	$(SOURCE.GUI) \
	$(SOURCE.GAME) \
	$(SOURCE.GAME.FPS) \
	$(SOURCE.GAME.NET) \
	$(SOURCE.GAME.FX) \
	$(SOURCE.GAME.VEHICLES) \
   $(SOURCE.UTIL) \
   $(SOURCE.LIGHTINGSYSTEM)

ifeq "$(OS)" "WIN32"
SOURCE.ENGINE += $(SOURCE.PLATFORM$(OS))
else
SOURCE.ENGINE += $(SOURCE.PLATFORM$(OS)DEDICATED)
endif


SOURCE.TESTAPP =\
	$(SOURCE.AUDIO) \
	$(SOURCE.COLLISION) \
	$(SOURCE.CONSOLE) \
	$(SOURCE.CONSTRUCTOR) \
	$(SOURCE.CORE) \
	$(SOURCE.DGL) \
	$(SOURCE.EDITOR) \
	$(SOURCE.GUI) \
	$(SOURCE.GAME) \
	$(SOURCE.GAME.FPS) \
	$(SOURCE.GAME.NET) \
	$(SOURCE.GAME.FX) \
	$(SOURCE.GAME.VEHICLES) \
   $(SOURCE.UTIL) \
	$(SOURCE.I18N) \
	$(SOURCE.INTERIOR) \
	$(SOURCE.MATH) \
	$(SOURCE.PLATFORM) \
	$(SOURCE.SCENEGRAPH) \
	$(SOURCE.SIM) \
	$(SOURCE.TERRAIN) \
	$(SOURCE.TS) \
	$(SOURCE.LIGHTINGSYSTEM)

SOURCE.TESTAPP_CLIENT =\
	$(SOURCE.TESTAPP) \
	$(SOURCE.PLATFORM$(OS)) \

SOURCE.TESTAPP_DEDICATED =\
	$(SOURCE.TESTAPP) \
	$(SOURCE.PLATFORM$(OS)DEDICATED) \

SOURCE.TESTAPP_CLIENT.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $O, $(basename $(SOURCE.TESTAPP_CLIENT))) )
SOURCE.TESTAPP_DEDICATED.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $O, $(basename $(SOURCE.TESTAPP_DEDICATED))) )
SOURCE.ENGINE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $O, $(basename $(SOURCE.ENGINE))) )
SOURCE.ALL += $(SOURCE.TESTAPP_CLIENT)
targetsclean += torqueClean

#---------------------------------------
# Set up include variables here.
INCLUDES_BASE = -I../lib/zlib -I../lib/lungif -I../lib/lpng -I../lib/ljpeg -I../lib/directx8 -I../lib/xiph/include
INCLUDES_LINUX = $(INCLUDES_BASE) -I../lib/openal/LINUX -I../lib/xiph/include/theora
INCLUDES_OpenBSD = $(INCLUDES_BASE) -I../lib/openal/OpenBSD
INCLUDES_FreeBSD = $(INCLUDES_BASE) -I../lib/openal/FreeBSD
INCLUDES_WIN32 = $(INCLUDES_BASE) -I../lib/openal/win32

#----------------------------------------
# normal binary
$(EXE_NAME): $(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE)

DIR.LIST = $(addprefix $(DIR.OBJ)/, $(sort $(dir $(SOURCE.TESTAPP_CLIENT))))

$(DIR.LIST): targets.torque.mk

$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): CFLAGS += $(INCLUDES_$(OS))

$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): LIB.PATH +=../lib/$(DIR.OBJ) \

$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): LINK.LIBS.GENERAL += \
	$(PRE.LIBRARY.LIB)ljpeg$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lpng$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lungif$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)zlib$(EXT.LIB) 

$(DIR.OBJ)/$(EXE_NAME)$(EXT.EXE): $(DIR.OBJ) $(DIR.LIST) $(SOURCE.TESTAPP_CLIENT.OBJ) 
	${CHECK_LINK_FILE}
	$(DO.LINK.CONSOLE.EXE)
	$(CP) $(DIR.OBJ)/$(EXE_NAME)$(BUILD_SUFFIX).* $(BIN_DIRECTORY)


#----------------------------------------
# engine library
engine: $(DIR.OBJ)/engine$(EXT.LIB)

DIR.LIST = $(addprefix $(DIR.OBJ)/, $(sort $(dir $(SOURCE.ENGINE))))

$(DIR.LIST): targets.torque.mk

# unix build needs to add DEDICATED to the CFLAGS
EXTRAFLAGS=
ifneq "$(OS)" "WIN32"
EXTRAFLAGS=-DDEDICATED -DTORQUE_ENGINE
endif

$(DIR.OBJ)/engine$(EXT.LIB): CFLAGS += $(EXTRAFLAGS) -DTORQUE_LIB $(INCLUDES_$(OS))

$(DIR.OBJ)/engine$(EXT.LIB): $(DIR.OBJ) $(DIR.LIST) $(SOURCE.ENGINE.OBJ)
	${CHECK_LINK_FILE}
	$(DO.LINK.LIB)

#----------------------------------------
# dedicated server build (unix only)
dedicated: $(DIR.OBJ)/$(EXE_DEDICATED_NAME)$(EXT.EXE)

DIR.LIST = $(addprefix $(DIR.OBJ)/, $(sort $(dir $(SOURCE.TESTAPP_DEDICATED))))

$(DIR.LIST): targets.torque.mk

$(DIR.OBJ)/$(EXE_DEDICATED_NAME)$(EXT.EXE): CFLAGS += -DDEDICATED $(INCLUDES_$(OS))

$(DIR.OBJ)/$(EXE_DEDICATED_NAME)$(EXT.EXE): LIB.PATH +=../lib/$(DIR.OBJ) \

$(DIR.OBJ)/$(EXE_DEDICATED_NAME)$(EXT.EXE): LINK.LIBS.GENERAL = \
	$(LINK.LIBS.SERVER) \
	$(PRE.LIBRARY.LIB)ljpeg$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lpng$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lungif$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)zlib$(EXT.LIB)

$(DIR.OBJ)/$(EXE_DEDICATED_NAME)$(EXT.EXE): $(DIR.OBJ) $(DIR.LIST) $(SOURCE.TESTAPP_DEDICATED.OBJ)
	${CHECK_LINK_FILE}
	$(DO.LINK.CONSOLE.EXE)
	$(CP) $(DIR.OBJ)/$(EXE_DEDICATED_NAME)$(BUILD_SUFFIX).* $(BIN_DIRECTORY)

#----------------------------------------
torqueClean:
ifneq ($(wildcard $(EXE_NAME)_DEBUG.*),)
	-$(RM)  $(EXE_NAME)$(BUILD_SUFFIX)*
endif
ifneq ($(wildcard $(EXE_NAME)_RELEASE.*),)
	-$(RM)  $(EXE_NAME)_RELEASE*
endif

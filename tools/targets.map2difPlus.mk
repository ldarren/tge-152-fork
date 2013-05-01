MAP2DIFPLUS.SOURCE=\
	map2difPlus/createLightmaps.cc \
	map2difPlus/editInteriorRes.cc \
	map2difPlus/entityTypes.cc \
	map2difPlus/main.cc \
	map2difPlus/bspNode.cc \
	map2difPlus/navGraph.cc \
	map2difPlus/editFloorPlanRes.cc \
	map2difPlus/morianUtil.cc \
	map2difPlus/csgBrush.cc \
	map2difPlus/exportGeometry.cc \
	map2difPlus/editGeometry.cc \
	map2difPlus/convert.cc \
	map2difPlus/lmapPacker.cc \

MAP2DIFPLUS.SOURCE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $O, $(basename $(MAP2DIFPLUS.SOURCE))) )
SOURCE.ALL += $(MAP2DIFPLUS.SOURCE)


map2difPlus: map2difPlus$(EXT.EXE)

map2difPlus$(EXT.EXE): CFLAGS += -I../engine -I../lib/zlib -I../lib/lungif -I../lib/lpng -I../lib/ljpeg -I../lib/openal/LINUX -I../lib/xiph/include

map2difPlus$(EXT.EXE): LIB.PATH += \
	../engine/$(DIR.OBJ) \
	../lib/$(DIR.OBJ) \

map2difPlus$(EXT.EXE): LINK.LIBS.GENERAL = $(PRE.ENGINE.LIB)engine$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)ljpeg$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lpng$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lungif$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)zlib$(EXT.LIB) \
	$(LINK.LIBS.TOOLS)


map2difPlus$(EXT.EXE): dirlist engine$(EXT.LIB) $(MAP2DIFPLUS.SOURCE.OBJ)
	$(DO.LINK.CONSOLE.EXE)

FONTTOOL.SOURCE=\
	fonttool/Getopt.cc \
	fonttool/fonttool.cc


FONTTOOL.SOURCE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $O, $(basename $(FONTTOOL.SOURCE))) )
SOURCE.ALL += $(FONTTOOL.SOURCE)


fonttool: fonttool$(EXT.EXE)

fonttool$(EXT.EXE): CFLAGS += -I../engine -I../lib/zlib -I../lib/lungif -I../lib/lpng -I../lib/ljpeg -I../lib/xiph/include

fonttool$(EXT.EXE): LIB.PATH += \
	../engine/$(DIR.OBJ) \
	../lib/$(DIR.OBJ) \

fonttool$(EXT.EXE): LINK.LIBS.GENERAL = $(PRE.ENGINE.LIB)engine$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)ljpeg$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lpng$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lungif$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)zlib$(EXT.LIB) \
	$(LINK.LIBS.TOOLS)


fonttool$(EXT.EXE): dirlist engine$(EXT.LIB) $(FONTTOOL.SOURCE.OBJ)
	$(DO.LINK.CONSOLE.EXE)

LANGC.SOURCE=\
	langc/Getopt.cc \
	langc/langc.cc \
	langc/langcomp.cc 


LANGC.SOURCE.OBJ:=$(addprefix $(DIR.OBJ)/, $(addsuffix $O, $(basename $(LANGC.SOURCE))) )
SOURCE.ALL += $(LANGC.SOURCE)


langc: langc$(EXT.EXE)

langc$(EXT.EXE): CFLAGS += -I../engine -I../lib/zlib -I../lib/lungif -I../lib/lpng -I../lib/ljpeg -I../lib/xiph/include

langc$(EXT.EXE): LIB.PATH += \
	../engine/$(DIR.OBJ) \
	../lib/$(DIR.OBJ) \

langc$(EXT.EXE): LINK.LIBS.GENERAL = $(PRE.ENGINE.LIB)engine$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)ljpeg$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lpng$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)lungif$(EXT.LIB) \
	$(PRE.LIBRARY.LIB)zlib$(EXT.LIB) \
	$(LINK.LIBS.TOOLS)


langc$(EXT.EXE): dirlist engine$(EXT.LIB) $(LANGC.SOURCE.OBJ)
	$(DO.LINK.CONSOLE.EXE)

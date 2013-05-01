OPENGL.SOURCE=\
	opengl2d3d/opengl2d3d.cc

OPENGL.SOURCE.DEF=\
	opengl2d3d/opengl2d3d.def


GLU.SOURCE=\
	opengl2d3d/glu2d3d.cc

GLU.SOURCE.DEF=\
	opengl2d3d/glu2d3d.def


OPENGL.SOURCE.OBJ=$(addprefix $(DIR.OBJ)/, $(OPENGL.SOURCE:.cc=$O))
GLU.SOURCE.OBJ=$(addprefix $(DIR.OBJ)/, $(GLU.SOURCE:.cc=$O))
SOURCE.ALL += $(OPENGL.SOURCE) $(GLU.SOURCE)


glu2d3d: opengl2d3d $(DIR.OBJ)/glu2d3d$(EXT.DLL) 
opengl2d3d: $(DIR.OBJ)/opengl2d3d$(EXT.DLL) 


#----------------------------------------
$(DIR.OBJ)/opengl2d3d$(EXT.DLL): CFLAGS += -I- -Idirectx8

$(DIR.OBJ)/opengl2d3d$(EXT.DLL): LIB.PATH += \
	directx8 \

$(DIR.OBJ)/opengl2d3d$(EXT.DLL): LINK.LIBS.GENERAL += \
	d3dx.lib \
	ddraw.lib \

#dxguid.lib dxerr8.lib ole32.lib shell32.lib
#d3dx8.lib d3d8.lib winmm.lib dxguid.lib d3dxof.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib 
#d3dx8dt.lib d3d8.lib d3dxof.lib winmm.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib 

$(DIR.OBJ)/opengl2d3d$(EXT.DLL): dirlist $(OPENGL.SOURCE.OBJ) $(OPENGL.SOURCE.DEF)
	$(DO.LINK.DLL)
	$(CP) $(DIR.OBJ)/opengl2d3d$(EXT.DLL) ../example


#----------------------------------------
$(DIR.OBJ)/glu2d3d$(EXT.DLL): CFLAGS += -I- -Idirectx8 -D_X86_

$(DIR.OBJ)/glu2d3d$(EXT.DLL): LIB.PATH += \
	$(DIR.OBJ) \
	directx8 \

$(DIR.OBJ)/glu2d3d$(EXT.DLL): LINK.LIBS.GENERAL += \
	opengl2d3d$(EXT.LIB)  \
	d3dx.lib \
	ddraw.lib \

$(DIR.OBJ)/glu2d3d$(EXT.DLL): dirlist opengl2d3d $(DIR.OBJ)/opengl2d3d$(EXT.LIB) $(GLU.SOURCE.OBJ) $(GLU.SOURCE.DEF)
	$(DO.LINK.DLL)
	$(CP) $(DIR.OBJ)/glu2d3d$(EXT.DLL) ../example





include ../mk/conf.GCC.mk

COMPILER.c      =gcc
# -Wno-invalid-offsetof is only valid for g++ compilation, had to remove it from the CFLAGS variable seeing that we
#  pass CFLAGS to both gcc and g++
COMPILER.cc     =g++ -Wno-invalid-offsetof

CFLAGS += -mtune=i686

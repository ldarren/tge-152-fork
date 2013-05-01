include ../mk/conf.GCC.mk

COMPILER.c      =gcc
# -Wno-invalid-offsetof is only valid for g++ compilation, had to remove it from the CFLAGS variable seeing that we
#  pass CFLAGS to both gcc and g++
COMPILER.cc     =g++ -Wno-invalid-offsetof

CFLAGS += -mtune=i686

# turn off -O2 and -O3 optimization: causes executable crash with some
# versions of 3.4
CFLAGS:=$(subst -O2,-O1,$(CFLAGS))
CFLAGS:=$(subst -O3,-O1,$(CFLAGS))


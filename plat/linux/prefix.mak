# architecture makefile configure

# prefix & suffix
BIN_PREFIX 			= 
BIN_SUFFIX 			= .b

OBJ_PREFIX 			= 
OBJ_SUFFIX 			= .o

LIB_PREFIX 			= lib
LIB_SUFFIX 			= .a

DLL_PREFIX 			= lib
DLL_SUFFIX 			= .so

ASM_SUFFIX 			= .S

# prefix
ifneq ($(BIN),)
PRE_ 				:= $(BIN)/$(PRE)
else
PRE_ 				:=
endif

# tool
CC 					= $(PRE_)gcc
AR 					= $(PRE_)ar
STRIP 				= $(PRE_)strip
RANLIB 				= $(PRE_)ranlib
LD 					= $(PRE_)g++
AS					= yasm
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make
PWD 				= pwd

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -DNDEBUG -freg-struct-return -fno-bounds-check -fvisibility=hidden
CXFLAGS_DEBUG 		= -g -D__tb_debug__
CXFLAGS 			= -c -Wall -D__tb_arch_$(ARCH)__
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

# arch
ifeq ($(ARCH),x86)
CXFLAGS 			+= -march=i686 -mssse3 
endif

ifeq ($(ARCH),x64)
CXFLAGS 			+= -m64 -mssse3
endif

# opti
ifeq ($(SMALL),y)
CXFLAGS_RELEASE 	+= -Os
else
CXFLAGS_RELEASE 	+= -O3
endif

# small
CXFLAGS-$(SMALL) 	+= -D__tb_small__

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				= \
					-std=c99 \
					-fomit-frame-pointer \
					-D_GNU_SOURCE=1 -D_REENTRANT \
					-fno-math-errno -fno-signed-zeros -fno-tree-vectorize \
					-Wno-parentheses -Wstrict-prototypes \
					-Wno-switch -Wno-format-zero-length -Wdisabled-optimization \
					-Wpointer-arith -Wno-pointer-sign -Wwrite-strings \
					-Wtype-limits -Wundef -Wmissing-prototypes -Wno-pointer-to-int-cast \
					-Werror=unused-variable -Werror=implicit-function-declaration 
#					-Werror

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	= -fno-rtti
CCFLAGS_DEBUG 		= 
CCFLAGS 			= \
					-D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
					-D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600

# ldflags
LDFLAGS_RELEASE 	=
LDFLAGS_DEBUG 		= 
LDFLAGS 			= 
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-o 			= -o

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= -f elf $(ARCH_ASFLAGS)
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

ifeq ($(ARCH),x64)
ASFLAGS 			+= -m amd64
endif

# arflags
ARFLAGS 			= -cr

# share ldflags
SHFLAGS 			= -shared -Wl,-soname

# include sub-config
include 			$(PLAT_DIR)/config.mak



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

# tool
PRE 				= 
CC 					= $(PRE)gcc
AR 					= $(PRE)ar
STRIP 				= $(PRE)strip
RANLIB 				= $(PRE)ranlib
LD 					= $(PRE)g++
AS					= yasm
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make
PWD 				= pwd

# arch flags
ifeq ($(ARCH),x86)
ARCH_CXFLAGS 		= -march=i686
endif

ifeq ($(ARCH),x64)
ARCH_CXFLAGS 		= -m64
ARCH_ASFLAGS 		= -m amd64
endif

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -O3 -DNDEBUG -freg-struct-return -fno-bounds-check
CXFLAGS_DEBUG 		= -g
CXFLAGS 			= -c -Wall -mssse3 $(ARCH_CXFLAGS) -D__tb_arch_$(ARCH)__
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				= \
					-std=c99 \
					-fomit-frame-pointer \
					-D_GNU_SOURCE=1 -D_REENTRANT \
					-Wno-parentheses \
					-Wno-switch -Wno-format-zero-length -Wdisabled-optimization \
					-Wpointer-arith -Wredundant-decls -Wno-pointer-sign -Wwrite-strings \
					-Wtype-limits -Wundef -Wno-pointer-to-int-cast \
					-Wstrict-prototypes -fno-math-errno -fno-signed-zeros -fno-tree-vectorize \
					-Werror=implicit-function-declaration 

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

# arflags
ARFLAGS 			= -cr

# share ldflags
SHFLAGS 			= -shared -Wl,-soname

# include sub-config
include 			$(PLAT_DIR)/config.mak



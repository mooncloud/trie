TARGETD := libtrie.so 
TARGETS := libtrie.a 
TARGET_DIR := ./lib

CC := g++
AR := ar
SUFFIX := cpp

INCLUDE := ./include
LIBPATH := ./lib 
LIB := 

INCFLAGS := $(addprefix -I, $(INCLUDE))
LDFLAGS := $(addprefix -L, $(LIBPATH)) $(addprefix -l, $(LIB))

#调试的时候加上调试开关g，提供lib库的时候，记得关闭
#CFLAGS := -g -Wall $(addprefix -D, $(MACROS)) -fPIC #-D_MYDEBUG   
CFLAGS := -O2 -Wall $(addprefix -D, $(MACROS)) -fPIC #-D_MYDEBUG
ARFLAGS := -rc 
MACROS :=  

SRC_DIRS := ./src 
SRC := $(foreach DIR, $(SRC_DIRS), $(wildcard $(DIR)/*.$(SUFFIX)))
OBJ := $(patsubst %.$(SUFFIX), %.o, $(SRC))

DB_VERSION := 


.PHONY:all clean

all:$(TARGETD) $(TARGETS) test_main
     
%.o:%.$(SUFFIX)
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

#编译成静态库	
static:$(TARGETS)
${TARGETS}: $(OBJ)
	${AR} ${ARFLAGS} $@ $(OBJ)
	mv $(TARGETS) $(TARGET_DIR)

#编译成动态库
#qztag_mining_test.cpp
share:$(TARGETD)
$(TARGETD): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) -shared
	mv $(TARGETD) $(TARGET_DIR)

test_main:
	$(CC) $(CFLAGS) -o $@ ./test/main.cpp   $(INCFLAGS) $(LDFLAGS) -ltrie 
	mv ./test_main ./bin/
clean:
	rm $(TARGET_DIR)/$(TARGETD) $(OBJ) 

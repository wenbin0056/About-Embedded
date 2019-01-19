
CPPSRCS += $(wildcard *.cpp ) 
CSRCS += $(wildcard *.cpp ) 
OBJS := $(patsubst %.c,%.o,$(wildcard *.c)) 
HEADERS := -I"../include"
HEADERS += -I"../../../main/include/"

TARGET := libhttpd

C_COMPILER = gcc
CPP_COMPILER = g++
AR = ar

COMPILER_FLAG = -DHAVE_FCNTL_H -DHAVE_ATOLL -DHAVE_DIRENT_H -DHAVE_SELECT -DCFG_Space -DHAVE_INT64T



all:  $(TARGET)

%.o: %.c
	@echo "CC $@"
	@$(C_COMPILER) -c  $(HEADERS) $(COMPILER_FLAG) -o $@ $<
	@echo "CC $@ end"	
	
%.o: %.cpp
	@echo "CC $@"
	@$(CPP_COMPILER) $(HEADERS) $(COMPILER_FLAG) -o "$@" "$<"	

$(TARGET):$(OBJS)
	$(C_COMPILER)  $(OBJS) -lpthread  -o $(TARGET)

clean:
	rm $(TARGET) *.o

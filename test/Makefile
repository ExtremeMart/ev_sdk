
APP:=test

CPP=g++

#SRCS:= $(wildcard *.cpp *.c)
SRCS:= \
	../3rd/cJSON/cJSON.c \
	../test/test.cpp 
	
OBJS:= $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

CPPFLAGS:= \
	-I../include \
	-I../3rd/cJSON \
	-I../3rd/license/v10/include
	
LDFLAGS:= \
	-L../lib -lji \
	-lglog
	
LDFLAGS += `pkg-config --libs opencv`

CXX_RULE := -Wall -O3 -fPIC

all: $(APP)

%.o: %.c
	$(CPP) ${CXX_RULE} $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	$(CPP) ${CXX_RULE} $(CPPFLAGS) -c $< -o $@
	
$(APP): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(CPPFLAGS) $(LDFLAGS) -Wl,-rpath=/usr/local/ev_sdk/lib
	cp $(APP) ../bin

clean:
#	echo $(SRCS) 
#	echo $(OBJS)
	rm -rf $(OBJS) $(APP)
	

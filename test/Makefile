APP:=test-ji-api

CPP=g++

#SRCS:= $(wildcard *.cpp *.c)
SRCS:= \
	../3rd/cJSON/src/cJSON.c \
	../test/test.cpp

OBJS:= $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

CPPFLAGS:= \
	-I/usr/local/include/opencv4 \
	-I../include \
	-I../3rd/cJSON/include \
	-I../3rd/license/v20_0/include

LDFLAGS:= \
	-L../lib -lji \
	-lglog

#LDFLAGS += `pkg-config --libs opencv`
LDFLAGS += -lopencv_videoio -lopencv_highgui -lopencv_imgcodecs -lopencv_core -lopencv_imgproc

CXX_RULE := -std=c++11 -Wall -O3 -fPIC

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


APP:=libji.so

CPP=g++

#SRCS:= $(wildcard *.cpp *.c)
SRCS:= \
	../3rd/cJSON/cJSON.c \
	../3rd/boost_interface/BoostInterface.cpp \
	../src/ji.cpp 
	
OBJS:= $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

CPPFLAGS:= \
	-I../include \
	-I../src \
	-I../3rd/cJSON \
	-I../3rd/license/v7.1/include \
	-I../3rd/boost_interface \
	-I../3rd/encrypted_module-master/include
	
LDFLAGS:= \
	-L../3rd/license/v7.1/lib -lji_license \
	-L../3rd/encrypted_module-master/lib -lencrypt_wrapper \
	-lm -lcrypto
	

LDFLAGS += `pkg-config --libs opencv`	

CXX_RULE := -std=c++11 -Wall -O3 -fPIC

all: $(APP)

%.o: %.c
	$(CPP) ${CXX_RULE} $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	$(CPP) ${CXX_RULE} $(CPPFLAGS) -c $< -o $@
	
$(APP): $(OBJS)
	$(CPP) -shared -o $@ $(OBJS) $(CPPFLAGS) $(LDFLAGS)
	cp $(APP) ../lib

clean:
#	echo $(SRCS) 
#	echo $(OBJS)
	rm -rf $(OBJS) $(APP)
	

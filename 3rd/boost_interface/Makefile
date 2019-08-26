OBJS =	

LIBS += -lboost_system -lboost_filesystem \
	    -lboost_thread -lboost_coroutine -lboost_context -lboost_regex
	   
#LIBS += $(shell pkg-config --libs opencv)
LIBS += -I/usr/local/include/opencv4 -lopencv_videoio -lopencv_highgui -lopencv_imgcodecs -lopencv_core -lopencv_imgproc

TARGET = testWkt

$(TARGET):	$(OBJS)
	g++ -std=c++11 -o  $(TARGET) main.cpp BoostInterface.cpp $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

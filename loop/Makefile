source := main.cpp
target := loop

CC = g++
LD = ld

LIB += -L/usr/local/lib
INCLUDE += -I/usr/local/include

CPPFLAGS += $(INCLUDE) -std=c++14
LDFLAGS += $(LIB) -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio

all: $(objects)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(target) $(source)
clean: 
	@rm $(target)

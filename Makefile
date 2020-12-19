
CXXFLAGS +=-g -I ./include -std=c++11

# LDFLAGS+= -L ./lib -lmlucv

C_SRCS=$(wildcard *.cpp)

C_OBJS=$(C_SRCS:%.cpp=%.o)

OBJS=$(C_OBJS)

TARGET=cachesimulator.out

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)

MYPROGRAM=HRobot
LOCALINCLUDE=../common
SOURCES=Axis.cpp ConsoleGUI.cpp Logger.cpp Robot.cpp RobotController.cpp RobotManager.cpp RobotSocketI2C.cpp main.cpp
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))
CXXFLAGS= -O2 -Wall -g -std=c++11 -D_GLIBCXX_USE_NANOSLEEP
MYLIBRARIES= -lncurses -pthread
CXX=g++-4.7
#CXX=g++
#------------------------------------------------------------------------------



all: $(MYPROGRAM)

%.o : %.c
	$(CXX) -I $(LOCALINCLUDE) -c $(CXXFLAGS) -o $@ $<

RobotPacket.o : ../common/RobotPacket.cpp
	$(CXX) -I $(LOCALINCLUDE) -c $(CXXFLAGS) -o RobotPacket.o ../common/RobotPacket.cpp 

$(MYPROGRAM): RobotPacket.o $(OBJECTS)
	$(CXX) -I $(LOCALINCLUDE) $(CXXFLAGS) RobotPacket.o $(OBJECTS) -o$(MYPROGRAM) $(MYLIBRARIES)

clean:
	rm -f $(OBJECTS)
	rm -f $(MYPROGRAM)

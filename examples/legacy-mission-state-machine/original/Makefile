CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17 -Iinclude

SRC=src/main.cpp src/mission_state_machine.cpp src/telemetry_health.cpp src/failsafe_manager.cpp src/mission_logger.cpp
OUT=mission_state_machine

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
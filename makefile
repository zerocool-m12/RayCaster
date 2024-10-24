SRC = main.cpp
CXX = clang++
CXX_L = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = ZeroCool_RayCaster

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXX_L) -o $(TARGET)

.PHONY: clear
clear:
	rm -f $(TARGET)

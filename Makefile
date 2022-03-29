CXX = g++
WARNINGS_ON = -Wall -Wextra -Wshadow -Wconversion -Wdouble-promotion
WARNINGS_OFF = -Wno-unused-function
CXXFLAGS = -O0 -g $(WARNINGS_ON) $(WARNINGS_OFF)

linux:
	@$(CXX) -o pi.out pi_linux_main.c $(CXXFLAGS)

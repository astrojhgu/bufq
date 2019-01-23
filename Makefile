#CXX=clang++ -std=c++17

main: main.cpp bufq.hpp
	$(CXX) $< -o $@ -g -pthread

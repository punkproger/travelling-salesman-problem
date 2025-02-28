#TODO: start use CMake instead of Makefile 

SRC = src/
COMMON_DIR  = $(SRC)Common/
FILEREADER_DIR  = $(SRC)FileReader/
OPTIMIZATION_DIR  = $(SRC)Optimization/
SOLVERS_DIR = $(SRC)Solvers/
IMAGE_GENERATOR_DIR = $(SRC)ImageGenerator/
SOLVERS_FLAGS = -I./$(SOLVERS_DIR)interface

BRUTE_FORCE_SOLVER_DIR   = $(SOLVERS_DIR)CBruteForceSolver/
BACK_TRACkING_SOLVER_DIR = $(SOLVERS_DIR)CBackTrackingSolver/
CHRISTOFIEDES_SOLVER_DIR = $(SOLVERS_DIR)CChristofidesSolver/

CXX = g++
FLAGS = -std=c++11 -g -Wall -O3 -I./$(SRC)

OBJECTS = *.o

all: main

main: main.o FileReader.o Optimization.o Functions.o ImageGenerator.o CBruteForceSolver.o CBackTrackingSolver.o CChristofidesSolver.o
	$(CXX) main.o FileReader.o Optimization.o Functions.o ImageGenerator.o CBruteForceSolver.o CBackTrackingSolver.o CChristofidesSolver.o -o tsp

main.o: $(SRC)main.cpp
	$(CXX) -c $(SRC)main.cpp $(FLAGS) -I./$(SOLVERS_DIR) $(SOLVERS_FLAGS)

FileReader.o: $(FILEREADER_DIR)FileReader.cpp
	$(CXX) -c $(FILEREADER_DIR)FileReader.cpp $(FLAGS)

Optimization.o: $(OPTIMIZATION_DIR)Optimization.cpp
	$(CXX) -c $(OPTIMIZATION_DIR)Optimization.cpp $(FLAGS)

Functions.o: $(COMMON_DIR)Functions.cpp
	$(CXX) -c $(COMMON_DIR)Functions.cpp $(FLAGS)

ImageGenerator.o: $(IMAGE_GENERATOR_DIR)ImageGenerator.cpp
	$(CXX) -c $(IMAGE_GENERATOR_DIR)ImageGenerator.cpp $(FLAGS)

CBruteForceSolver.o: $(BRUTE_FORCE_SOLVER_DIR)CBruteForceSolver.cpp
	$(CXX) -c $(BRUTE_FORCE_SOLVER_DIR)CBruteForceSolver.cpp $(FLAGS) $(SOLVERS_FLAGS)

CBackTrackingSolver.o: $(BACK_TRACkING_SOLVER_DIR)CBackTrackingSolver.cpp
	$(CXX) -c $(BACK_TRACkING_SOLVER_DIR)CBackTrackingSolver.cpp $(FLAGS) $(SOLVERS_FLAGS)

CChristofidesSolver.o: $(CHRISTOFIEDES_SOLVER_DIR)CChristofidesSolver.cpp
	$(CXX) -c $(CHRISTOFIEDES_SOLVER_DIR)CChristofidesSolver.cpp $(FLAGS) $(SOLVERS_FLAGS)

clean:
	rm -rf *.o tsp
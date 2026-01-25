# Makefile for test verification
#
# Usage:
#   make              - Build verify_vectors
#   make run          - Build and run, output to stdout
#   make json         - Build and output to cpp_output.json
#   make clean        - Remove built files

CXX = g++
CXXFLAGS = -std=c++17 -I../src -Wall -Wextra

.PHONY: all run json clean

all: verify_vectors

verify_vectors: verify_vectors.cpp ../src/Generator.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

run: verify_vectors
	./verify_vectors

json: verify_vectors
	./verify_vectors > cpp_output.json
	@echo "Output written to cpp_output.json"

clean:
	rm -f verify_vectors cpp_output.json


CC=g++ -std=c++11 -g3 -Wall -O0
HEADERS=-I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -I/home/bjoern/rose/
LIBS=-lrose -lboost_thread -lboost_filesystem -lboost_wave -lboost_iostreams -lboost_system -lboost_regex
OBJECTS=exporter.o ./BasicBlockSummary.o bjoernNode.o bjoernUseDefAnalyzer.o CSVWriter.o MyPartialSymbolicSemantics.o MyProcessor.o TracePolicy.o

exporter : $(OBJECTS)
	$(CC) -o "exporter" $(HEADERS) $(OBJECTS) $(LIBS)

exporter.o: exporter.cpp
	$(CC) $(HEADERS) -c exporter.cpp 

BasicBlockSummary.o : BasicBlockSummary.cpp
	$(CC) $(HEADERS) -c BasicBlockSummary.cpp 

bjoernNode.o: ../common/bjoernNode.cpp
	$(CC) $(HEADERS) -c ../common/bjoernNode.cpp

bjoernUseDefAnalyzer.o: bjoernUseDefAnalyzer.cpp
	$(CC) $(HEADERS) -c bjoernUseDefAnalyzer.cpp

CSVWriter.o: CSVWriter.cpp
	$(CC) $(HEADERS) -c CSVWriter.cpp

MyPartialSymbolicSemantics.o: MyPartialSymbolicSemantics.cpp
	$(CC) $(HEADERS) -c MyPartialSymbolicSemantics.cpp

MyProcessor.o: MyProcessor.cpp
	$(CC) $(HEADERS) -c MyProcessor.cpp

TracePolicy.o: TracePolicy.cpp
	$(CC) $(HEADERS) -c TracePolicy.cpp

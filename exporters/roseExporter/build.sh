#!/bin/bash

g++ -std=c++11 -I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"exporter.d" -MT"exporter.d" -o "exporter.o" "exporter.cpp"

g++ -std=c++11 -I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"CSVWriter.d" -MT"CSVWriter.d" -o "CSVWriter.o" "CSVWriter.cpp"

g++ -std=c++11 -I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"bjoernNode.d" -MT"bjoernNode.d" -o "bjoernNode.o" "../common/bjoernNode.cpp"

g++ -std=c++11 -I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"BasicBlockSummary.d" -MT"BasicBlockSummary.d" -o "BasicBlockSummary.o" "BasicBlockSummary.cpp"

g++ -std=c++11 -I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"TracePolicy.d" -MT"TracePolicy.d" -o "TracePolicy.o" "TracePolicy.cpp"

g++ -std=c++11 -L/home/bjoern/boost_47_153/lib -L/home/bjoern/rose/lib -o "exporter"  ./exporter.o ./CSVWriter.o ./bjoernNode.o ./BasicBlockSummary.o ./TracePolicy.o -lrose -lboost_thread -lboost_filesystem -lboost_wave -lboost_iostreams -lboost_system -lboost_regex

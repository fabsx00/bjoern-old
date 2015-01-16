#!/bin/bash

# g++ -I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"interBb0.d" -MT"interBb0.d" -o "interBb0.o" "interBb0.cpp"

# g++ -L/home/bjoern/boost_47_153/lib -L/home/bjoern/rose/lib -o "interBb0"  ./interBb0.o -lrose -lboost_thread -lboost_filesystem -lboost_wave -lboost_iostreams -lboost_system -lboost_regex


g++ -I/home/bjoern/rose/include -I/home/bjoern/rose/include/rose -I/home/bjoern/boost_47_153/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"interBb1.d" -MT"interBb1.d" -o "interBb1.o" "interBb1.cpp"

g++ -L/home/bjoern/boost_47_153/lib -L/home/bjoern/rose/lib -o "interBb1"  ./interBb1.o -lrose -lboost_thread -lboost_filesystem -lboost_wave -lboost_iostreams -lboost_system -lboost_regex

#!/bin/bash

BOOST_DIR="/usr/local/"

g++ -std=c++11 -Wno-unused-local-typedefs -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"to_csv.d" -MT"to_csv.d" -o "to_csv.o" "to_csv.cpp"

g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"CSVWriter.d" -MT"CSVWriter.d" -o "CSVWriter.o" "CSVWriter.cpp"

g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"BAP_SSA.d" -MT"BAP_SSA.d" BAP_SSA.cpp

g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"general.d" -MT"general.d" general.cpp

g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"read_graph.d" -MT"read_graph.d" read_graph.cpp

g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"dot_export.d" -MT"dot_export.d" dot_export.cpp

g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"bjoernNodes.d" -MT"bjoernNodes.d" bjoernNodes.cpp

g++ -std=c++11 -O0 -g3 -Wno-unused-local-typedefs -o "to_csv"  ./to_csv.o ./CSVWriter.o ./bjoernNodes.o ./BAP_SSA.o ./read_graph.o ./dot_export.o ./general.o

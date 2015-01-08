#!/bin/bash

BATCH_IMPORTER_JAR=/home/bjoern/neo4j/batch-import-jar-with-dependencies.jar

source ./set_rose_env.sh
./astExporter "$@"
java -jar $BATCH_IMPORTER_JAR /home/bjoern/neo4j/graph.db nodes.csv edge.csv

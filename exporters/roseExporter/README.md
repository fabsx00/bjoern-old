ROSE Exporter
=============

This program parses a binary using the ROSE compiler framework
(https://github.com/rose-compiler/edg4x-rose) and creates CSV files
containing nodes and edges suitable to be imported into a Neo4j graph
database using the batch-importer by  Michael Hunger
(https://github.com/jexp/batch-import).

In summary, we create the following nodes:

* *Function nodes.* For each function of the binary, we store one node
  for its entry point.

* *Basic block nodes.* These are nodes of the control flow graph that contain
  at least one but possibly many instructions.

* *Instruction nodes.* Each of these nodes represents a single
  instruction.

The following edges connect these nodes:

* *Function-to-entry edges.* We connect each functions with its entry
  basic block.

* *Control flow edges.* Basic blocks are connected via control flow
  edges. We use labels 'true' and 'false' to indicate conditional
  branches.

* *Edges from basic blocks to instructions.* Each basic block is
  connected via edges to the instructions it contains. We use
  child-numbers in instruction nodes to obtain ordering.


#!/bin/sh

# Pass on command-line-arguments
/home/user/bap-0.8/util/toil -bin $@ -o il.txt

# Get CFG - as a dot graph...
/home/user/bap-0.8/util/iltrans -il il.txt -to-cfg -pp-ast-cfg cfg.dot

# Get SSA-DDG - as a dot graph... with basic block granularity
/home/user/bap-0.8/util/iltrans -il il.txt -to-ssa -pp-ssa-ddg ssa_ddg.dot


./to_csv cfg.dot ssa_ddg.dot


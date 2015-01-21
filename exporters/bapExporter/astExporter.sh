#!/bin/sh

$BAPDIR=/home/user/bap-0.8/utils

# Pass on command-line-arguments
$BAPDIR/toil -bin $@ -o il.txt

# Get CFG - as a dot graph...
$BAPDIR/iltrans -il il.txt -to-cfg -pp-ast-cfg cfg.dot

# Get SSA-DDG - as a dot graph... with basic block granularity
$BAPDIR/iltrans -il il.txt -to-ssa -pp-ssa-ddg ssa_ddg.dot

./to_csv cfg.dot ssa_ddg.dot


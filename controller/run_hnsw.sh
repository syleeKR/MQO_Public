#!/bin/bash

DATAPATH="/datasets/xxx/MQO/deep/data1M.fbin"
QUERYPATH="/datasets/xxx/MQO/deep/query100K.fbin"
ANSWERPATH="/datasets/xxx/MQO/deep/answer/data1M_query100K.bin"

GRAPH_PATH="/datasets/xxx/MQO/deep/hnsw.bin"

SAVEPATH="$HOME/MQO/experiment_results/hnsw"

APPROX_METHOD="mixed_modified"
M=7
EF=7
B=400 # ~ |Q| / 250
MIN_BEAM=4
MAX_BEAM=400
INCR=1.414
USE_L2=1

source ~/miniconda3/etc/profile.d/conda.sh
conda activate mqo
cd ../hnsw_exp
####################################### If need compile ##############################
rm run
echo "begin compiling"

g++ -std=c++17 -O3 -march=native -ffast-math -fopenmp  -ftree-vectorize -funroll-loops -I"$CONDA_PREFIX/include" -o run run_hnsw.cpp
echo "compiled"
####################################### ####################################### 
#Run
./run "$DATAPATH" "$QUERYPATH" "$ANSWERPATH" "$GRAPH_PATH" "$SAVEPATH" "$APPROX_METHOD" "$M" "$EF" "$B" "$MIN_BEAM" "$MAX_BEAM" "$INCR" "$USE_L2"

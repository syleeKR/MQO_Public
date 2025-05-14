#!/bin/bash

# PARAM

DATAPATH="/datasets/xxx/MQO/deep/data1M.fbin"
QUERYPATH="/datasets/xxx/MQO/deep/query100K.fbin"
ANSWERPATH="/datasets/xxx/MQO/deep/answer/data1M_query100K.bin"

GRAPH_PATH="/datasets/xxx/MQO/deep/nsg1M.graph"

SAVEPATH="$HOME/MQO/experiment_results/nsg"

APPROX_METHOD="mixed_modified"

M=7
EF=7
B=400 # ~ |Q| / 250
MIN_BEAM=4
MAX_BEAM=100
INCR=1.414
USE_L2=1



source ~/miniconda3/etc/profile.d/conda.sh
conda activate mqo
cd ../nsg_exp
####################################### If need compile ##############################
rm run
echo "begin compiling"
g++ -std=c++17 -O3 -march=native -ffast-math -fopenmp -ftree-vectorize -funroll-loops \
    -Iefanna_graph/include -Insg/include -I"$CONDA_PREFIX/include" \
    run_nsg.cpp efanna_graph/src/*.cpp nsg/src/*.cpp -o run

echo "compiled"
####################################### ####################################### 
# Run
./run "$DATAPATH" "$QUERYPATH" "$ANSWERPATH" "$GRAPH_PATH" "$SAVEPATH" "$APPROX_METHOD" "$M" "$EF" "$B" "$MIN_BEAM" "$MAX_BEAM" "$INCR" "$USE_L2"


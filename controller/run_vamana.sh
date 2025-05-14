#!/bin/bash

DATAPATH="/datasets/xxx/MQO/deep/data1M.fbin"
QUERYPATH="/datasets/xxx/MQO/deep/query100K.fbin"
ANSWERPATH="/datasets/xxx/MQO/deep/answer/data1M_query100K.bin"

GRAPH_PATH="/datasets/xxx/MQO/deep/v1M.bin"
SAVEPATH="$HOME/MQO/experiment_results/vamana"

APPROX_METHOD="mixed_modified"
M=7
EF=7
B=400 # ~ |Q| / 250
MIN_BEAM=4
MAX_BEAM=100
INCR=1.414
DATA_TYPE="float"   # u8int float
USE_L2=1 #"l2"   # mips l2

source ~/miniconda3/etc/profile.d/conda.sh
conda activate mqo
cd ../vamana_exp

####################################### If need compile ############################## 
rm -f run
echo "begin compiling"
g++ -std=c++17 \
    -Ofast \
    -fopenmp \
    -funroll-loops \
    -fopenmp-simd -mavx2 -mfma -msse2 \
    -mavx512f -mavx512dq -mavx512vl -DUSE_AVX2 \
    -I DiskANN/include \
    -I "$CONDA_PREFIX/include" \
    run_vamana.cpp \
    DiskANN/src/*.cpp \
    -L "$CONDA_PREFIX/lib" \
    -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread \
    -liomp5 -lpthread -lm -ldl -laio \
    -o run
echo "compiled"
####################################### ####################################### 

#Run
export LD_LIBRARY_PATH="$CONDA_PREFIX/lib:$LD_LIBRARY_PATH"
./run "$DATAPATH" "$QUERYPATH" "$ANSWERPATH" "$GRAPH_PATH" "$SAVEPATH" "$APPROX_METHOD" "$M" "$EF" "$B" "$MIN_BEAM" "$MAX_BEAM" "$INCR" "$DATA_TYPE" "$USE_L2"

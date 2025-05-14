# OANN


```

conda create -n mqo \
  python=3.9 \
  boost \
  cmake \
  gxx_linux-64 \
  intel-openmp \
  mkl \
  mkl-include \
  libaio \
  gperftools \
  -c conda-forge

conda install matplotlib
conda install pandas
conda install -c conda-forge eigen


```

## Tutorial
Code to run each algorithm
- hnsw_exp 
- nsg_exp
- vaman_exp

Building approx MST / exact MST trajectory
- mqo

Data handling
- data_handler

scripts 
- controller

## Quick Start
```
cd controller
fill in directories
bash run_nsg.sh
```
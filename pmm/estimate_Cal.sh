#!/bin/bash
./bin/estimate --dim 1 -k 2 < onlySpeedKm.csv > ./estimate_result/estimate_k2.out 2> estimate.err
./bin/estimate --dim 1 -k 3 < onlySpeedKm.csv > ./estimate_result/estimate_k3.out 2> estimate.err
./bin/estimate --dim 1 -k 4 < onlySpeedKm.csv > ./estimate_result/estimate_k4.out 2> estimate.err
./bin/estimate --dim 1 -k 5 < onlySpeedKm.csv > ./estimate_result/estimate_k5.out 2> estimate.err
./bin/estimate --dim 1 -k 8 < onlySpeedKm.csv > ./estimate_result/estimate_k8.out 2> estimate.err

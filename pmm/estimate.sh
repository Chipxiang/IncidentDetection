#!/bin/bash
#cat /home/ljx/label_data/fold_1/*.csv | bin/estimate --dim 3 > fold_1/estimate.out 2> fold_1/estimate.err
cat /home/ljx/label_data/fold_2/*.csv | bin/estimate --dim 3 > fold_2/estimate.out 2> fold_2/estimate.err &
cat /home/ljx/label_data/fold_3/*.csv | bin/estimate --dim 3 > fold_3/estimate.out 2> fold_3/estimate.err &
cat /home/ljx/label_data/fold_4/*.csv | bin/estimate --dim 3 > fold_4/estimate.out 2> fold_4/estimate.err &
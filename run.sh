g++ -std=c++11 -stdlib=libc++ -pthread -O3 main_multi.cpp -o anomgroupdetect;
# 1st parameter: anomaly type
# 2nd parameter: anomaly ratio
# 3rd parameter: anomaly density
# 4th parameter: camouflage ratio

DATASET="amazon"
S=5
T=7
ANOM_TYPES=(0 1 2 3)
ANOM_RATIOS=(0.05)
ANOM_DENSITIES=(0.05)
CAMO_RATIOS=(0.1)

for AT in $ANOM_TYPES
do
    for AR in $ANOM_RATIOS
    do
        for AD in $ANOM_DENSITIES
        do
            for CR in $CAMO_RATIOS
            do
                ./anomgroupdetect "$DATASET" $AT $AR $AD $CR $S $T
            done
        done
    done
done

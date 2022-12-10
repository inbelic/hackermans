#!/bin/bash

compute_solution()
{
    ./hackermans test_cases/tc$1.csv > solns/$1.csv
}

rm solns/*
for i in 1 2 3 4 5 6 7 8 9 10
# for i in 2
do
    echo "Case $i"
    compute_solution $i
done

rm submission.zip
mkdir temp
cp solns/* temp/
cp hackermans.cpp temp/hackermans.cpp
cd temp
zip submission *
cd ..
mv temp/submission.zip .
rm -rf temp

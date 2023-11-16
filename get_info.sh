#! /bin/bash

set -e

TTY_name="/dev/ttyACM0"

test -e "$TTY_name" || (echo "Error: $TTY_name not found"; exit 1)

echo "Start script"

rm -rf gcov_output

cd scripts

echo "Receiving infos"
python3 receive.py
echo "Converting received infos"
./lwfs.sh inputFile.txt

cd ..
mkdir gcov_output

echo "Generate outputs"
gcovr --html-details -o gcov_output/output.html

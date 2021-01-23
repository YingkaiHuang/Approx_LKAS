#!/bin/bash

profilertype="$1"
echo $profilertype

if [ "$profilertype" != "chrono" ]; then
    echo "Error: incorrect profiler type!" >&2
    echo "... Run this script with \"/bin/bash run_denoise_single.sh chrono\""
    exit 1
fi


if [ "$profilertype" = "chrono" ]; then
    echo "profiler type = chrono"

	# Compile the runner
	echo "#########################"
	echo "#   		Compile		  #"
	echo "#########################"
	make run_compress_multiple_chrono

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	./compress_multiple.o /home/sayandipde/Approx_IBC/final_app/cpp/Profiling/multiple-images/
fi



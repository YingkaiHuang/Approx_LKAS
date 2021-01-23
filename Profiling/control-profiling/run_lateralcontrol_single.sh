#!/bin/bash

profilertype="$1"
echo $profilertype

if [ "$profilertype" != "chrono" ] && [ "$profilertype" != "valgrind" ]; then
    echo "Error: incorrect profiler type!" >&2
    echo "... Run this script with \"/bin/bash run_denoise_single.sh chrono/valgrind\""
    exit 1
fi

if [ "$profilertype" = "chrono" ]; then
    echo "profiler type = chrono"

	# Compile the runner
	echo "#########################"
	echo "#   		Compile		  #"
	echo "#########################"
	make run_lateralcontrol_single_chrono

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	./lateralcontrol_single.o
fi

if [ "$profilertype" = "valgrind" ]; then
	echo "profiler type = valgrind"

	# Compile the runner
	echo "#########################"
	echo "#   		Compile		  #"
	echo "#########################"
	make run_lateralcontrol_single_valgrind

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind --collect-atstart=no --dump-instr=yes --fair-sched=try ./lateralcontrol_single.o
	#valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind --dump-instr=yes ./lanedetect_single.o
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.1 | tee valgrind/readable.profile.callgrind.inclusive
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.1 | tee valgrind/readable.profile.callgrind
	
fi

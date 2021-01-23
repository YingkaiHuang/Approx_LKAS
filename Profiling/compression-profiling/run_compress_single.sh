#!/bin/bash

profilertype="$1"
echo $profilertype

if [ "$profilertype" != "chrono" ] && [ "$profilertype" != "valgrind" ] && [ "$profilertype" != "pytime" ]; then
    echo "Error: incorrect profiler type!" >&2
    echo "... Run this script with \"/bin/bash run_denoise_single.sh chrono/valgrind/pytime\""
    exit 1
fi

if [ "$profilertype" = "pytime" ]; then
    echo "profiler type = python time monotonic"

	# Compile the runner
	echo "#########################"
	echo "#   		Compile		  #"
	echo "#########################"
	make run_compress_single_pytime

fi

if [ "$profilertype" = "chrono" ]; then
    echo "profiler type = chrono"

	# Compile the runner
	echo "#########################"
	echo "#   		Compile		  #"
	echo "#########################"
	make run_compress_single_chrono

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	./compress_single.o
fi

if [ "$profilertype" = "valgrind" ]; then
	echo "profiler type = valgrind"

	# Compile the runner
	echo "#########################"
	echo "#   		Compile		  #"
	echo "#########################"
	make run_compress_single_valgrind_compress

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.compress --collect-atstart=no --dump-instr=yes --fair-sched=try ./compress_single.o
	#valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind --dump-instr=yes ./lanedetect_single.o
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.compress.1 | tee valgrind/readable.profile.callgrind.compress
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.compress.1 | tee valgrind/readable.profile.callgrind.compress.inclusive
	
	# Compile the runner
	echo "#########################"
	echo "#   		Compile		  #"
	echo "#########################"
	make clean
	make run_compress_single_valgrind_decompress

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.decompress --collect-atstart=no --dump-instr=yes --fair-sched=try ./compress_single.o
	#valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind --dump-instr=yes ./lanedetect_single.o
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.decompress.1 | tee valgrind/readable.profile.callgrind.decompress
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.decompress.1 | tee valgrind/readable.profile.callgrind.decompress.inclusive
fi



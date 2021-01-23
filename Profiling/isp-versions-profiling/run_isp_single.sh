#!/bin/bash

profilertype="$1"
echo $profilertype

if [ "$profilertype" != "chrono" ] && [ "$profilertype" != "valgrind" ]; then
    echo "Error: incorrect profiler type!" >&2
    echo "... Run this script with \"/bin/bash run_denoise_single.sh chrono/valgrind\""
    exit 1
fi

if [ "$profilertype" = "chrono" ]; then
	# Compile the generator
	echo "#########################"
	echo "# Compile the generator #"
	echo "#########################"
	make gen_rev 
	make gen_fwd

	# Run the generator
	echo "#########################"
	echo "#   run the generator   #"
	echo "#########################"
	echo "rev"
	./auto_scheduler_generate_rev.o    -o . -g auto_schedule_gen_rev -f auto_schedule_true_rev -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v0"
	./auto_scheduler_generate_fwd_v0.o -o . -g auto_schedule_gen_fwd_v0 -f auto_schedule_true_fwd_v0 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v1"
	./auto_scheduler_generate_fwd_v1.o -o . -g auto_schedule_gen_fwd_v1 -f auto_schedule_true_fwd_v1 -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	echo "v2"
	./auto_scheduler_generate_fwd_v2.o -o . -g auto_schedule_gen_fwd_v2 -f auto_schedule_true_fwd_v2 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v3"
	./auto_scheduler_generate_fwd_v3.o -o . -g auto_schedule_gen_fwd_v3 -f auto_schedule_true_fwd_v3 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v4"
	./auto_scheduler_generate_fwd_v4.o -o . -g auto_schedule_gen_fwd_v4 -f auto_schedule_true_fwd_v4 -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	echo "v5"
	./auto_scheduler_generate_fwd_v5.o -o . -g auto_schedule_gen_fwd_v5 -f auto_schedule_true_fwd_v5 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v6"
	./auto_scheduler_generate_fwd_v6.o -o . -g auto_schedule_gen_fwd_v6 -f auto_schedule_true_fwd_v6 -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	echo "#########################"
	echo "#   generator finished  #"
	echo "#########################"

	# Compile the runner
	echo "#########################"
	echo "#   Compile the runner  #"
	echo "#########################"
	make run_isp_single_chrono

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	./isp_single.o 0
	./isp_single.o 1
	./isp_single.o 2
	./isp_single.o 3
	./isp_single.o 4
	./isp_single.o 5
	./isp_single.o 6
fi 

if [ "$profilertype" = "valgrind" ]; then
	# Compile the generator
	echo "#########################"
	echo "# Compile the generator #"
	echo "#########################"
	make gen_rev 
	make gen_fwd

	# Run the generator
	echo "#########################"
	echo "#   run the generator   #"
	echo "#########################"
	echo "rev"
	./auto_scheduler_generate_rev.o    -o . -g auto_schedule_gen_rev -f auto_schedule_true_rev -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v0"
	./auto_scheduler_generate_fwd_v0.o -o . -g auto_schedule_gen_fwd_v0 -f auto_schedule_true_fwd_v0 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v1"
	./auto_scheduler_generate_fwd_v1.o -o . -g auto_schedule_gen_fwd_v1 -f auto_schedule_true_fwd_v1 -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	echo "v2"
	./auto_scheduler_generate_fwd_v2.o -o . -g auto_schedule_gen_fwd_v2 -f auto_schedule_true_fwd_v2 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v3"
	./auto_scheduler_generate_fwd_v3.o -o . -g auto_schedule_gen_fwd_v3 -f auto_schedule_true_fwd_v3 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v4"
	./auto_scheduler_generate_fwd_v4.o -o . -g auto_schedule_gen_fwd_v4 -f auto_schedule_true_fwd_v4 -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	echo "v5"
	./auto_scheduler_generate_fwd_v5.o -o . -g auto_schedule_gen_fwd_v5 -f auto_schedule_true_fwd_v5 -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
	echo "v6"
	./auto_scheduler_generate_fwd_v6.o -o . -g auto_schedule_gen_fwd_v6 -f auto_schedule_true_fwd_v6 -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	echo "#########################"
	echo "#   generator finished  #"
	echo "#########################"

	# Compile the runner
	echo "#########################"
	echo "#   Compile the runner  #"
	echo "#########################"
	make run_isp_single_valgrind

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.v0 --collect-atstart=no --dump-instr=yes  --fair-sched=try ./isp_single.o 0
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.v0.1 | tee valgrind/readable.profile.callgrind.v0
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.v0.1 | tee valgrind/readable.profile.callgrind.inclusive.v0
	
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.v1 --collect-atstart=no --dump-instr=yes  --fair-sched=try ./isp_single.o 1
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.v1.1 | tee valgrind/readable.profile.callgrind.v1
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.v1.1 | tee valgrind/readable.profile.callgrind.inclusive.v1
	
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.v2 --collect-atstart=no --dump-instr=yes  --fair-sched=try ./isp_single.o 2
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.v2.1 | tee valgrind/readable.profile.callgrind.v2
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.v2.1 | tee valgrind/readable.profile.callgrind.inclusive.v2
	
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.v3 --collect-atstart=no --dump-instr=yes  --fair-sched=try ./isp_single.o 3
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.v3.1 | tee valgrind/readable.profile.callgrind.v3
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.v3.1 | tee valgrind/readable.profile.callgrind.inclusive.v3
	
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.v4 --collect-atstart=no --dump-instr=yes  --fair-sched=try ./isp_single.o 4
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.v4.1 | tee valgrind/readable.profile.callgrind.v4
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.v4.1 | tee valgrind/readable.profile.callgrind.inclusive.v4
	
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.v5 --collect-atstart=no --dump-instr=yes  --fair-sched=try ./isp_single.o 5
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.v5.1 | tee valgrind/readable.profile.callgrind.v5
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.v5.1 | tee valgrind/readable.profile.callgrind.inclusive.v5
	
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind.v6 --collect-atstart=no --dump-instr=yes  --fair-sched=try ./isp_single.o 6
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.v6.1 | tee valgrind/readable.profile.callgrind.v6
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.v6.1 | tee valgrind/readable.profile.callgrind.inclusive.v6
fi

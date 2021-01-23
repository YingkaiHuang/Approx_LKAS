all:

run_lanedetect_multiple_chrono:
	g++ lanedetect_multiple.cpp lane_detection.cpp -O3 -Ofast -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/Approx_IBC/final_app/eigen \
	-I ./other-sources \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L /usr/local/lib -ljpeg \
	-L ~/anaconda3/lib \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o lanedetect_multiple.o -std=c++11	

run_lanedetect_single_chrono:
	g++ lanedetect_single.cpp lane_detection.cpp -O3 -Ofast -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/Approx_IBC/final_app/eigen \
	-I ./other-sources \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L /usr/local/lib -ljpeg \
	-L ~/anaconda3/lib \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o lanedetect_single.o -std=c++11	
	
run_lanedetect_single_valgrind:
	g++ lanedetect_single.cpp lane_detection.cpp -O3 -Ofast -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/Approx_IBC/final_app/eigen \
	-I ./other-sources \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L /usr/local/lib -ljpeg \
	-L ~/anaconda3/lib \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHVALGRIND \
	-o lanedetect_single.o -std=c++11
	
run_lanedetect_single:
	g++ lanedetect_single.cpp lane_detection.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/Approx_IBC/final_app/eigen \
	-I ./other-sources \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L /usr/local/lib -ljpeg \
	-L ~/anaconda3/lib \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DNOPROFILE \
	-o lanedetect_single.o -std=c++11	
		
clean:
	rm -f *.o
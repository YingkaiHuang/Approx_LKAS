all:

run_lateralcontrol_multiple_chrono:
	g++ main_lateralcontrol_multiple.cpp lane_detection.cpp lateralcontrol_multiple.cpp -O3 -Ofast -g \
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
	-o lateralcontrol_multiple.o -std=c++11

run_lateralcontrol_single_chrono:
	g++ main_lateralcontrol_single.cpp lane_detection.cpp lateralcontrol_single.cpp -O3 -Ofast -g \
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
	-o lateralcontrol_single.o -std=c++11

run_lateralcontrol_single_valgrind:
	g++ main_lateralcontrol_single.cpp lane_detection.cpp lateralcontrol_single.cpp -O3 -Ofast -g \
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
	-o lateralcontrol_single.o -std=c++11	
		
clean:
	rm -f *.o
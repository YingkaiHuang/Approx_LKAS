all:

run_compress_single_pytime:
	python compress_single.py --time_compress_decompress
	
run_compress_multiple_chrono:
	g++ compress_multiple.cpp -O3 -Ofast -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L ~/anaconda3/lib -ljpeg \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o compress_multiple.o -std=c++11

run_compress_single_chrono:
	g++ compress_single.cpp -O3 -Ofast -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L ~/anaconda3/lib -ljpeg \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o compress_single.o -std=c++11
	
run_compress_single_valgrind_compress:
	g++ compress_single.cpp -O3 -Ofast -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L /usr/local/lib -ljpeg \
	-L ~/anaconda3/lib \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHVALGRINDCOMPRESS \
	-o compress_single.o -std=c++11
	
run_compress_single_valgrind_decompress:
	g++ compress_single.cpp -O3 -Ofast -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin \
	-L /usr/local/lib -ljpeg \
	-L ~/anaconda3/lib \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHVALGRINDDECOMPRESS \
	-o compress_single.o -std=c++11

clean:
	rm -f *.o 
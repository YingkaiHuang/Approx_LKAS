all:


gen_fwd:
	g++  auto_scheduler_generate_fwd.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L ~/apps/Halide/bin -lHalide \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags`  \
	-fno-rtti -lpthread -ldl -lm \
	-o auto_scheduler_generate_fwd.o -std=c++11

run_demosaic_multiple_chrono:
	g++ demosaic_multiple.cpp \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I /home/yingkai/opencv_build/opencv/include \
	-L ~/apps/Halide/bin -lHalide \
	-L  /home/yingkai/opencv_build/opencv/build/lib \
	auto_schedule_dem_fwd.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags`  \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o demosaic_multiple.o -std=c++11

run_demosaic_single_chrono:
	g++ demosaic_single.cpp \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I /home/yingkai/opencv_build/opencv/include \
	-I /home/yingkai/opencv_build/opencv/build/include \
	-L ~/apps/Halide/bin -lHalide \
	-L  /home/yingkai/opencv_build/opencv/build/lib \
	auto_schedule_dem_fwd.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags`  \
	`pkg-config opencv --cflags --libs`-lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o demosaic_single.o -std=c++11	
	
run_demosaic_single_valgrind:
	g++ demosaic_single.cpp \
	-I /usr/include/valgrind \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I /home/yingkai/opencv_build/opencv/include \
	-L ~/apps/Halide/bin -lHalide \
	-L  /home/yingkai/opencv_build/opencv/build/lib \
	auto_schedule_dem_fwd.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags`  \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHVALGRIND \
	-o demosaic_single.o -std=c++11	

clean:
	rm -f *.o auto_schedule_dem_fwd.*

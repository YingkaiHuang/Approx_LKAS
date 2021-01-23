QT -= core
QT -= gui

TARGET = cpp_webots_main
TEMPLATE = app

DEFINES -= UNICODE
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += no_keywords

#DEFINES += NON_MATLAB_PARSING
#DEFINES += MAX_EXT_API_CONNECTIONS=255
DEFINES += DO_NOT_USE_SHARED_MEMORY
#DEFINES += HALIDE_NO_JPEG
*-msvc* {
    QMAKE_CXXFLAGS += -W3
}
*-g++* {
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-empty-body
    QMAKE_CXXFLAGS += -Wno-write-strings_API=true
    QMAKE_CXXFLAGS += -g
    QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-narrowing
    QMAKE_CXXFLAGS += -std=c++11

    QMAKE_CFLAGS += -Wall
    QMAKE_CFLAGS += -Wno-strict-aliasing
    QMAKE_CFLAGS += -Wno-unused-parameter
    QMAKE_CFLAGS += -Wno-unused-but-set-variable
    QMAKE_CFLAGS += -Wno-unused-local-typedefs  
    #QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=1

	 
}


win32 {
    LIBS += -lwinmm
    LIBS += -lWs2_32
}

macx {
}


unix:!macx {
    LIBS += -lrt
    LIBS += -ldl
    LIBS += -lm
    LIBS += `pkg-config opencv --libs`
    LIBS += -L/usr/local/lib/ -ltiff  
    LIBS += -L/home/yingkai/anaconda3/lib/ -ljpeg
    LIBS += -L/home/yingkai/apps/Halide/distrib/bin/ -lHalide
    LIBS += -L/snap/webots/14/usr/share/webots/lib/controller/ 
    LIBS += `libpng-config --cflags --ldflags`
    #LIBS += -lpthread
    LIBS += -L/home/yingkai/libtorch/lib -lc10 -ltorch
    LIBS +=  -lCppCar -lCppController -lCppDriver
    
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_rev.a -ldl
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v0.a -ldl
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v1.a -ldl
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v2.a -ldl
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v3.a -ldl
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v4.a -ldl
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v5.a -ldl
    LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v6.a -ldl
	LIBS += /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/Profiling/demosaic-profiling/auto_schedule_dem_fwd.a -ldl
}



INCLUDEPATH += "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/eigen"
INCLUDEPATH += "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/other-sources"
INCLUDEPATH += "/home/yingkai/apps/Halide/distrib/include"
INCLUDEPATH += "/home/yingkai/anaconda3/bin"
INCLUDEPATH += "/home/yingkai/anaconda3/include"
INCLUDEPATH += "/home/yingkai/apps/Halide/distrib/tools"
INCLUDEPATH += "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide"
INCLUDEPATH += "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/LaneDetection_and_Control/other-sources"
INCLUDEPATH += "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/Profiling/demosaic-profiling"
INCLUDEPATH += "/snap/webots/current/usr/share/webots/include/controller/cpp"
INCLUDEPATH += "/home/yingkai/libtorch/include"
INCLUDEPATH += "/home/yingkai/libtorch/include/torch/csrc/api/include"

SOURCES += \
    cpp_webots_framework.cpp \
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/other-sources/get_yL_fromref.cpp \
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/other-sources/meanshift.cpp \
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/LaneDetection_and_Control/lane_detection.cpp \
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/LaneDetection_and_Control/lateralcontrol_multiple_30.cpp \  
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/LaneDetection_and_Control/lateralcontrol_multiple_50.cpp \ 
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/LaneDetection_and_Control/lateralcontrol_multiple_30_bev.cpp \  
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/LaneDetection_and_Control/lateralcontrol_multiple_50_bev.cpp \ 
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/other-sources/image_signal_processing.cpp \
	/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/other-sources/utils.cpp \
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/LoadCamModel.cpp \
    /home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/MatrixOps.cpp \



unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

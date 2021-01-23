QT -= core
QT -= gui

TARGET = cpp_vrep_main
TEMPLATE = app

DEFINES -= UNICODE
CONFIG   += console
CONFIG   -= app_bundle

DEFINES += NON_MATLAB_PARSING
DEFINES += MAX_EXT_API_CONNECTIONS=255
DEFINES += DO_NOT_USE_SHARED_MEMORY

*-msvc* {
    QMAKE_CXXFLAGS += -W3
}
*-g++* {
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-empty-body
    QMAKE_CXXFLAGS += -Wno-write-strings
    QMAKE_CXXFLAGS += -g
    QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-narrowing

    QMAKE_CFLAGS += -Wall
    QMAKE_CFLAGS += -Wno-strict-aliasing
    QMAKE_CFLAGS += -Wno-unused-parameter
    QMAKE_CFLAGS += -Wno-unused-but-set-variable
    QMAKE_CFLAGS += -Wno-unused-local-typedefs   
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
    LIBS += `pkg-config --libs opencv`
    LIBS += -L/usr/local/lib -ljpeg
    LIBS += -L/home/kbimpisi/Approx_IBC_offline/Halide/cmake_build/lib -lHalide
    LIBS += `libpng-config --cflags --ldflags`
    LIBS += -lpthread
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_rev.a -ldl
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v0.a -ldl
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v1.a -ldl
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v2.a -ldl
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v3.a -ldl
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v4.a -ldl
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v5.a -ldl
    LIBS += /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v6.a -ldl
}


INCLUDEPATH += "/home/kbimpisi/Approx_IBC/final_app/vrep/programming/include"
INCLUDEPATH += "/home/kbimpisi/Approx_IBC/final_app/vrep/programming/remoteApi"
INCLUDEPATH += "/home/kbimpisi/Approx_IBC/final_app/eigen"
INCLUDEPATH += "/home/kbimpisi/Approx_IBC/final_app/cpp/cpp_vrep_api/other-sources"
INCLUDEPATH += "/home/kbimpisi/Approx_IBC_offline/Halide/cmake_build/include"
INCLUDEPATH += "/home/kbimpisi/Approx_IBC_offline/Halide/cmake_build/bin"
INCLUDEPATH += "/home/kbimpisi/Approx_IBC_offline/Halide/tools"
INCLUDEPATH += "/home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide"

SOURCES += \
    cpp_vrep_framework.cpp \
    /home/kbimpisi/Approx_IBC/final_app/cpp/cpp_vrep_api/other-sources/my_vrep_api.cpp \
    /home/kbimpisi/Approx_IBC/final_app/cpp/cpp_vrep_api/other-sources/lane_detection.cpp \
    /home/kbimpisi/Approx_IBC/final_app/cpp/cpp_vrep_api/other-sources/lateral_controller.cpp \
    /home/kbimpisi/Approx_IBC/final_app/cpp/cpp_vrep_api/other-sources/IBC_controller.cpp \    
    /home/kbimpisi/Approx_IBC/final_app/cpp/cpp_vrep_api/other-sources/image_signal_processing.cpp \
    /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/LoadCamModel.cpp \
    /home/kbimpisi/Approx_IBC/final_app/cpp/ReversiblePipeline/src/Halide/MatrixOps.cpp \
    /home/kbimpisi/Approx_IBC/final_app/cpp/cpp_vrep_api/other-sources/utils.cpp \
    /home/kbimpisi/Approx_IBC/final_app/vrep/programming/remoteApi/extApi.c \
    /home/kbimpisi/Approx_IBC/final_app/vrep/programming/remoteApi/extApiPlatform.c \
    /home/kbimpisi/Approx_IBC/final_app/vrep/programming/common/shared_memory.c

HEADERS +=\
    /home/kbimpisi/Approx_IBC/final_app/vrep/programming/remoteApi/extApi.h \
    /home/kbimpisi/Approx_IBC/final_app/vrep/programming/remoteApi/extApiPlatform.h \
    /home/kbimpisi/Approx_IBC/final_app/vrep/programming/common/shared_memory.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
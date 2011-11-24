##-------------------------------------------------
#
# Project created by QtCreator 2011-08-12T06:56:38
#
#-------------------------------------------------

QT       += core gui

TARGET = QtProject
TEMPLATE = app

DEFINES += USE_STD0X

CONFIG += qtestlib

SOURCES += main.cpp \
        MainWindow.cpp \
    ../Complex.cpp \
    ../FFT.cpp \
    ../OpenCL_FFT/fft_execute.cpp \
    ../OpenCL_FFT/fft_kernelstring.cpp \
    ../OpenCL_FFT/fft_setup.cpp \
    ../tests/Test.cpp \
    ../tests/Timer.cpp
#\
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/stdafx.cpp) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/statisticalTimer.cpp) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/clAmdFft.openCL.cpp) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/clAmdFft.client.cpp)

HEADERS  += MainWindow.h \
    ../Convolution.h \
    ../Complex.h \
    ../FFT.h \
    ../OpenCL_FFT/procs.h \
    ../OpenCL_FFT/clFFT.h \
    ../OpenCL_FFT/fft_base_kernels.h \
    ../OpenCL_FFT/fft_internal.h \
    ../FFT_FFTW3.h \
    ../FFTs.h \
    ../cl.hpp \
    ../FFT_MKL.h \
    ../FFT_OpenCL_A.h \
    ../tests/Test.h \
    ../FFT_CC.h \
    ../FourierTransform.h \
    ../uOpenCL.h \
    ../FFT_OpenCL_Contiguous.h \
    ../PartitionCamping_OpenCLTest.h \
    ../ContiguousKernelGenerator.h \
    ../FFT_OpenCL_Contiguous_InnerKernelTester.h \
    ../tests/Timer.h
#\
#    $$quote(../clAmdFft-1 1.4.182-Linux/include/clAmdFft.version.h) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/include/clAmdFft.h) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/targetver.h) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/stdafx.h) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/statisticalTimer.h) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/clAmdFft.openCL.h) \
#    $$quote(../clAmdFft-1 1.4.182-Linux/samples/clAmdFft.client.h)

LIBS += -lfftw3f -framework OpenCL
#-lopencl

OTHER_FILES += \
    ../OpenCL_FFT/ReadMe.txt \
    ../OpenCLPlan.txt
#    ../clAmdFft-1 1.4.182-Linux/samples/CMakeLists.txt


















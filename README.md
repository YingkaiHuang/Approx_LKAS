# Setting up IMACS Approx_LKAS using Webots in Linux

More details about the simulator can be found in http://www.es.ele.tue.nl/ecs/imacs. You could also read the papers below.
If you are using this simulator in any form in your work, please cite:
```
S. De, S. Mohamed, K. Bimpisidis, D. Goswami, T. Basten and H. Corporaal, "Approximation Trade Offs in an Image-Based Control System," in Design, Automation & Test in Europe Conference (DATE), 2020, pp. 1680-1685, doi: 10.23919/DATE48585.2020.9116552.

S. De, S. Mohamed, D. Goswami and H. Corporaal, "Approximation-Aware Design of an Image-Based Control System," in IEEE Access, vol. 8, pp. 174568-174586, 2020, doi: 10.1109/ACCESS.2020.3023047.

S. Mohamed, S. De, K. Bimpisidis, V, Nathan, D. Goswami, H. Corporaal, and T. Basten, “IMACS: A Framework for Performance Evaluation of Image Approximation in a Closed-loop System,” in Proceedings of the 8th Mediterranean Conference on Embedded Computing (MECO), 2019.
```

Tested with the following versions:
* OS: Ubuntu 18.04
* g++ (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0
* llvm: clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
* Halide: checkout branch 810a14b1cef3eb99c4051a2a7ca0c50a9872c37c

1. Install dependent libraries
2. Install Halide
3. Install OpenCV
4. Install Eigen
5. Install Pytorch and libtorch
6. Install Webots
7. Run IMACS

Initially, clone this repository:
```
git clone https://github.com/YingkaiHuang/Approx_LKAS.git
cd Approx_LKAS
pwd
```
For brevity, `$(root)=pwd`, i.e. the path to `Approx_LKAS` is called as `$(root)` or `$(LKASROOT)`. 

# 1-6 Dependencies

## 1. Dependent libraries
The following libraries might be needed for successful execution.
```
sudo apt-get install libtinfo-dev
sudo apt-get install libjpeg-dev
sudo apt-get install libtiff5-dev
sudo apt-get install libpng-dev
sudo apt-get install jasper
sudo apt-get install libgtk-3-dev
sudo apt-get install libopenblas-dev
sudo apt-get install -y libavcodec-dev
sudo apt-get install -y libavformat-dev
sudo apt-get install libavutil-dev
sudo apt-get install libswscale-dev
sudo apt-get install valgrind
# For openCV installation
sudo apt-get install cmake
# LibRAW for reversible pipeline
sudo apt-get install -y libraw-dev
## qt4
sudo apt-get install qt4-qmake
sudo apt-get install libqt4-dev
```

## 2. Installing Halide

If already installed, only export the paths of `Halide/bin` and `Halide/distrib/bin` to `LD_LIBRARY_PATH`. 

Note that for our framework, we use a specific checkout version `git checkout 810a14b1cef3eb99c4051a2a7ca0c50a9872c37c`. If you are using the latest version, you may have to adapt the syntax in the source files.

For more details, https://github.com/halide/Halide/blob/master/README.md.

If not already installed, follow the steps below.

Building Halide
===============

### 2.1 Dependencies for Halide

1. Install CLANG + LLVM
2. Make in HALIDE_PATH
3. Install Halide

#### 2.1.1 CLANG + LLVM
Have clang+llvm-6.0.0 installed. If already installed, you only need to export the paths CLANG and LLVM_CONFIG.
1. Download the Pre-Built Binary corresponding to your OS from http://releases.llvm.org/download.html.
Recommended version: LLVM 6.0.0
2. Extract the downloaded file into the folder `llvm` in `$(root)/externalApps`. If this folder does not exist `mkdir externalApps`.
```
cd $(root)/externalApps
mkdir llvm
tar -xvf clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz -C llvm --strip-components=1
#export CLANG=<path to clang>/bin/clang
export CLANG=$(root)/externalApps/llvm/bin/clang
#export LLVM_CONFIG=<path to llvm>/bin/llvm-config
export LLVM_CONFIG=$(root)/externalApps/llvm/bin/llvm-config
```
Alternately, these paths can be added to the `/etc/environment` file.
```
sudo gedit /etc/environment
```
Add these lines to the end of this file, save and close the file. As an example,
```
# root = /home/sajid/Desktop/IMACS_cppVrep
CLANG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/clang
LLVM_CONFIG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/llvm-config
```
source the paths for this to take effect.
```
source /etc/environment
```

If you do not have `sudo` rights, please follow the steps in `FAQ 4`.
#### 2.1.2 Building Halide with make

With `LLVM_CONFIG` set (or `llvm-config` in your path), you should be
able to just run `make` in the root directory of the Halide source tree.
We checkout a specific version as the syntax of codes in Approx_IBC is based on this version.
```
cd $(root)/externalApps
git clone https://github.com/halide/Halide.git
cd Halide
git checkout 810a14b1cef3eb99c4051a2a7ca0c50a9872c37c
gedit Makefile
```


Save and then Run the `Makefile`. This should not result in any errors.
```
make
```
A successful installation means all the tests are passed.

#### 2.1.3 Installing Halide
Halide does not have a `make install`. To install we use the following command.
```
make distrib
```
This will copy files to the `distrib` folder. 
Export the Halide bin to the library path.
```
export LD_LIBRARY_PATH=$(ROOT)/externalApps/Halide/distrib/bin:$(ROOT)/externalApps/Halide/bin:$LD_LIBRARY_PATH
```

## 3. Installing OpenCV

If opencv is already installed, make sure `opencv.pc` file is in the `/usr/lib/pkgconfig` or change the `PKG_CONFIG_PATH` to point to this file.

If not installed, follow the OpenCV installation steps from https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html.

Required Packages:
* GCC 4.4.x or later
* CMake 2.8.7 or higher
* Git
* GTK+2.x or higher, including headers (libgtk2.0-dev)
* pkg-config
* Python 2.6 or later and Numpy 1.5 or later with developer packages (python-dev, python-numpy)
* ffmpeg or libav development packages: libavcodec-dev, libavformat-dev, libswscale-dev
* [optional] libtbb2 libtbb-dev
* [optional] libdc1394 2.x
* [optional] libjpeg-dev, libpng-dev, libtiff-dev, libjasper-dev, libdc1394-22-dev
* [optional] CUDA Toolkit 6.5 or higher

### 3.1 Install ccache
```
sudo apt install -y ccache
sudo /usr/sbin/update-ccache-symlinks
echo 'export PATH="/usr/lib/ccache:$PATH"' | tee -a ~/.bashrc
source ~/.bashrc && echo $PATH
```
### 3.2 Install java
If not already installed, you can follow the steps from here: https://www.digitalocean.com/community/tutorials/how-to-install-java-with-apt-on-ubuntu-18-04.

In case there is an error: unable to locate `*.jar`
```
sudo apt install default-jre
sudo apt install default-jdk
sudo update-alternatives --config javac
sudo update-alternatives --config java
```
Choose the correct `jdk` version. `jre` does not have `tools.jar`
Update the JAVA_HOME path
```
sudo gedit /etc/environment
```
At the end of this file, add the following line, making sure to replace the path with your own copied path:
```
JAVA_HOME="/usr/lib/jvm/java-11-openjdk-amd64/bin/"
```
Modifying this file will set the JAVA_HOME path for all users on your system.
Save the file and exit the editor.

Now reload this file to apply the changes to your current session:
```
source /etc/environment
```
### 3.3 Getting OpenCV Source Code
```
cd $(root)/externalApps
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
```
### 3.4 Building OpenCV from Source Using CMake
```
cd opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. -DOPENCV_GENERATE_PKGCONFIG=ON
make
sudo make install
```

## 4. Install eigen
```
cd $(root)/externalApps
git clone https://gitlab.com/libeigen/eigen.git
```
## 5. Install Pytorch & Libtorch
## 5.1 Install Pytorch
Follow the instruction on the website https://pytorch.org/get-started/locally/ 
Please install the version 1.4.0 which is used in this project.
## 5.2 Install Libtorch
You can find the "libtorch-cxx11-abi-shared-with-deps-1.4.0+cpu.zip" file in "libtorch" folder, just unzip it.

## 6. Install Webots
Go to Webots official website and follow its instruction to install Webots on Linux
Webiste: https://cyberbotics.com/#cyberbotics

# 7. Running IMACS

## 7.1 Change hardcoded paths
Please change paths to your own system in the following files
```
$(LKASROOT)/LaneDetection_and_Control/lane_detection.cpp
$(LKASROOT)/cpp_webots_api/other-sources/image_signal_processing.cpp
$(LKASROOT)/cpp_webots_api/cpp_vrep_framework.pro
```
1. In "cpp_vrep_framework.pro", please change: 
line 57 which is the path of ljpeg;
line 58 which is the path of Halide;
line 59 which is the path of Webots;
line 62 which is the path of libtorch;
line 65-73 which is the path of ReversiblePipeline;
line 78-89 which are the paths of headers, you can change them to your folders;
line 91-103 which are the paths of codes.

To obtain the path, you could run `pwd` in a terminal opened from the corresponding folder.
2. Change line 89 of `lane_detection.cpp` to `string out_string = "$(LKASROOT)/cpp_webots_api/out_imgs";` where $(LKASROOT) is your actual path.
3. Change line 143 of `image_signal_processing.cpp` to `"$(LKASROOT)/ReversiblePipeline/camera_models/NikonD7000/";` where $(LKASROOT) is your actual path.
4. Change line 77 of `image_signal_processing.cpp` to `string out_string = "$(LKASROOT)/cpp_webots_api/out_imgs";` where $(LKASROOT) is your actual path.
5. Change line 76 of `cpp_webots_api/other-sources/config_webots.hpp` to `string gold_ref_csv = "$(LKASROOT)/worlds/final.csv";` where $(LKASROOT) is your actual path.
6. Change line 174, 291, 307 of "cpp_webots_api/cpp_webots_framework.cpp" which are the paths of pretrained models. You can find these models in "$(LKASROOT)/cpp_webots_api".
## 6.2 Make IMACS
Generate the `Makefile` using `qmake`.
```
cd $(LKASROOT)/cpp_webots_api
qmake cpp_vrep_framework.pro
make
```

When you want to run `make` in `$(LKASSROOT)` always run `qmake cpp_vrep_framework.pro` first. 
`make clean` cleans the file generated by this make.
`make distclean` cleans all the generated files by LKAS.

## 6.3 Start a Webots scene
Open a Webots-scene. Webots needs to be started before running IMACS framework.
In Webots, you can load one world file in "worlds" folder.
## 6.4 Run IMACS (in a new terminal)
```
cd $(LKASROOT)/cpp_webots_api
# Usage: ./imacs {pipeline_version} 
# Minimum one argument needed. (default values) ./cpp_webots_main 0
```

You do not have to always close Webots to run imacs again. Just reload world in Webots and wait for sometime before executing `./cpp_webots_main 0`.

In case you encounter some other errors, you can debug using the following command:
```
valgrind ./cpp_webots_main 0
```

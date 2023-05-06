# libvits-ncnn 🎙️

This is an ncnn implementation of the VITS library that supports cross-platform GPU-accelerated speech synthesis.  
The project is forked from [weirdseed/Vits-Android-ncnn](https://github.com/weirdseed/Vits-Android-ncnn), Thanks to the original author for their contribution.  


### 📖 Usage

1. 🔍 Prepare dependencies  

    Get the ncnn static library suitable for your runtime environment from its wiki or [ncnn/releases](https://github.com/Tencent/ncnn/releases).  
    Place it in the root directory ncnn folder like this:

    ```
    ncnn
    ├─bin
    ├─include
    └─lib
    ```

2. 🛠️ Compile the project  

    **a. libvits-ncnn**  
    Execute in the repo root directory:
    ```
    mkdir build && cd build  
    cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang  
    make
    ```
    After compilation, you can find `libvits-ncnn.so` in the build directory.  

    **b. vits-cli**  
    Enter the `demo` directory and execute：
    ```
    mkdir build && cd build  
    cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang  
    make
    ```
    After compilation, you can find `vits-cli` in the build directory.  

3. 🚀 Run the demo

    In the directory where `vits-cli` is located, prepare the dependencies required for running:  
    
    a. Download the [openjtalk dictionary file](https://sourceforge.net/projects/open-jtalk/files/Dictionary/open_jtalk_dic-1.11/open_jtalk_dic_utf_8-1.11.tar.gz/download) and unzip it;  
    b. Download the [VITS ncnn model](https://github.com/weirdseed/Vits-Android-ncnn/releases/download/v1.2.1/models.zip), unzip the Atri part to the `atri/` directory (for testing the monophonic model), and extract `365_epochs` to the `365_epochs/` directory (for testing the multi-tone model).  
    c. Download the [VITS ncnn params](https://github.com/weirdseed/Vits-Android-ncnn/tree/master/app/src/main/assets) (`single`/`multi` directories).

    At this time, the directory has:
    ```
    build
    ├─vits-cli
    ├─365_epochs
    ├─atri
    ├─multi
    ├─open_jtalk_dic_utf_8-1.11
    └─single
    ```

    Now execute `./vits-cli`.

### 🔧 Main changes
 - JNI export interface changed to C++  
 - C++ re-implemented preprocessing

### 📝 TODO 
 - [x] Support for Japanese monophonic model
 - [x] Support for Japanese multi-tone model
 - [ ] Support for Chinese monophonic model
 - [ ] Support for Chinese multi-tone model

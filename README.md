# Elastos.SDK.Wallet.C

## Build on Ubuntu 64bit
### Check the required tools
Make sure your computer have installed the required packages below:
* [git](https://www.git-scm.com/downloads)
* [cmake](https://cmake.org/download)
* [wget](https://www.gnu.org/software/wget)

### Build for linux

```shell
$ . ./scripts/linux.sh
$ cmake -D CMAKE_BUILD_TYPE=Debug/Release ../../..
$ make
```

### Build for android

```shell
$ export ANDROID_NDK=~/your_android_ndk_dir
$ . ./scripts/android.sh armeavi-v7a(for armv7)/arm64(for armv8)/x86
$ cmake -D CMAKE_BUILD_TYPE=Debug/Release ../../..
$ make
```


## Build on Mac
### Build for ios simulator

```shell
$ cd build
$ . ./scripts/ios.sh x86_64
$ cmake -D CMAKE_BUILD_TYPE=Debug/Release ../../..
$ make -j4
```

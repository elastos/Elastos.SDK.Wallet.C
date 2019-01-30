# Elastos.SDK.Wallet.C

[Elastos.SDK.Wallet.C documentation](https://walletsdkc.readthedocs.io)

## Build on Ubuntu 64bit
### Check the required tools
Make sure your computer have installed the required packages below:
* [git](https://www.git-scm.com/downloads)
* [cmake](https://cmake.org/download)
* [wget](https://www.gnu.org/software/wget)

### Build for linux

```shell
$ ./scripts/build.sh
```

### Build for android

```shell
$ export ANDROID_NDK_HOME={your android ndk path}
$ ./scripts/build.sh -f Android
```


## Build on Mac
### Build for ios simulator

```shell
$ ./scripts/build.sh -f Android
```

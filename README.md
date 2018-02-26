OpenCL Amoveo miner
==========

This is the best miner for Amoveo currently available.

## OpenCL dependency

You will need openCL software that is compatible with your graphics card.
I think this might come default with Mac, since I didn't have to install anything on my laptop for this.

Linux:
AMD driver / SDK link https://developer.amd.com/amd-accelerated-parallel-processing-app-sdk/
Nvidia driver / SDK link https://developer.nvidia.com/cuda-downloads

Windows:
OpenCL SDK can be downloaded by link https://developer.amd.com/amd-accelerated-parallel-processing-app-sdk/. Also you can try another OpenCL SDK vendors. Path to intalled SDK should be written to a new environment variable OPENCL_SDK.
Boost and OpenSSL libraries are included by Nuget Manager and should be downloaded automatically.


## other dependencies

for ubuntu
```
   sudo apt-get install erlang libncurses5-dev libssl-dev unixodbc-dev g++ git
```
This list probably includes more dependencies than you actually need.

for mac:

install erlang version 18 or higher.
You can use [Homebrew](https://brew.sh):
```
brew install erlang
```
There might be other dependencies too.

for windows:
I don't have a windows machine to find out.



## installation
you need git to install this way.
```
git clone https://github.com/zack-bitcoin/amoveo-opencl-miner.git
```
or you can use your browser to download it [by clicking here](https://github.com/zack-bitcoin/amoveo-opencl-miner/archive/master.zip)


## configure

You can connect it to a full node. Change the url at the top of miner.erl to point to your full node. It is named `Peer`. This gives all your money to whoever runs that full node.

You can also connect to a mining pool. If you connect to a mining pool, you get paid by the person running the pool.
This way you don't have to run Amoveo.
set `Peer` to be the url for the mining pool.

By default `Peer` is set up to connect to a public mining pool.

Put your pubkey into the `Pubkey` definition at the top of miner.erl so that you can get paid. 



## mining

On ubuntu and Mac OSX, it can be compiled and turned on like this: 
```
sh compile.sh 
```
Then you start mining like this:
```
miner:start().
```
To turn it off, first use `Control + C`, `a`, `enter` to exit the erlang interface.
Then to kill the miner processes, do:
```
sh clean.sh
```




## other notes

[Here is a link to where the same mining is implemented in erlang. The 'pow' function is the one to look at](https://github.com/BumblebeeBat/pink_crypto)

[Here it is implemented in C language](https://github.com/zack-bitcoin/amoveo-c-miner)

[Here is a link to the main Amoveo repo.](https://github.com/zack-bitcoin/amoveo). If you want to solo mine without a mining pool, you will need this. Solo miners make more profit.

Looking at cgMiner, which is a popular bitcoin miner, it uses between 2*5 and 2*29 threads. So this is the range we should test for.

from Scott, about installing and configuring opencl in linux:
ok so I fixed an issue where /user/include/ didn't have CL/cl.h link.. by installing apt-get install opencl-headers so now CL/cl.h has a valid path to /user/include/ but it didn't fix the issue but was an issue


from Greg, about installing and configuring in Windows:
I compiled with the Nvidia CUDA Toolkit OpenCL with Visual Studio 2017 community. For Erlang, I just installed with the Erlang binary installer and compiled with erlc then moved the .beam in my release folder and started it

Greg ran into this trouble: but that is messy because we need multiple tools, Erlang shell doesn't start with the right permissions and the default root is C:/Program Files/erl9.2/bin so I moved the miner.erl here to compile it
I think there is some erlang args / env vars to change the default root, I didn't know any Erlang before
#For development purposes, it is convenient to recompile your code every time you run it. This way if anything changed, the changes will be included.
# Since the things being compiled are so small, they can be compiled instantly, and there is no cost to recompiling every time we run the software.

gcc amoveo_miner.c -Wall -o amoveo_miner -framework OpenCL

# next recompile the erlang.
erlc miner.erl

# finally start an erlang interpreter so you can call the program.
 erl

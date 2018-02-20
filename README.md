OpenCL Amoveo miner
==========



This is a work in progress.
So far, it does this:

1) Feeds 66 bytes to the GPU
2) GPU modifies one of the bytes
3) receives the new 66 bytes from the GPU
4) prints out the new values of each byte.

## build and run

```
sh compile.sh
./amoveo_miner
```

## The next steps

* take the hash of the 66 bytes, and store it in the first 32 bytes. Verify that the hash is the same as what erlang would generate.

* increase the memory size to 98 bytes, so we have room to return a solution.

* add the difficulty check to the kernel.

* take the hash using many alternative nonces, if any nonce works, store that nonce in the solution space.

* connect the c code to the erlang from amoveo-c-miner so we can give the work to the server.
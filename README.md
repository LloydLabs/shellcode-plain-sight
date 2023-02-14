# Hiding Shellcode In Plain Sight
This technique is very simple, a RW memory region 2048 the size of the shellcode is allocated. This region is then filled with randomized data data (`RtlGenRandom`), the shellcode is then placed **randomly** somewhere within this massive region each time. This makes it hard for an AV/EDR solution, or an analyst, to simply see where the shellcode is in-memory. To summarize:
1. Allocate a large `PAGE_READWRITE` region, 2048 the size of the target shellcode, and align to `0x1000`
2. Fill this allocated region with random data
3. Write the shellcode to a random location within this region, save position
4. Change the page permissions to `PAGE_EXECUTE`
5. Execute the shellcode (page + position)
6. Zero the memory where the entire large region is to ensure the data does not persist after being freed, using the `RtlZeroMemory` macro
7. Free the region of memory

As can be seen below, the entropy does not change much at all. It is not obvious, from e.g. a memory dump, where the shellcode is actually situated.

![Memory dump entropy](https://i.imgur.com/96ixWdw.png)

You can execute the shellcode however you wish. This code is simply to demonstrate the simple technique. This technique was originally detailed in Avast's Raspberry Robin writeup [here](https://decoded.avast.io/janvojtesek/raspberry-robins-roshtyak-a-little-lesson-in-trickery/).

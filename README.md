# Hiding Shellcode In Plain Sight
This technique is very simple, a RW memory region 2048 the size of the shellcode is allocated (in another process, or not). It is detailed in Avast's Raspberry Robin writeup [here](https://decoded.avast.io/janvojtesek/raspberry-robins-roshtyak-a-little-lesson-in-trickery/) This region is then filled with randomized data data (`RtlGenRandom`), the shellcode is then placed **randomly** somewhere within this massive region each time. To summarize:
1. Allocate a large `PAGE_READWRITE` region, 2048 the size of the target shellcode, and align to `0x1000`
2. Fill this allocated region with random data
3. Write the shellcode to a random location within this region, save position
4. Change the page permissions to `PAGE_EXECUTE`
5. Execute the shellcode (page + position)
6. Zero the memory where the entire large region is to ensure the data does not persist after being freed, using the `RtlZeroMemory` macro
7. Free the region of memory

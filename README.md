# User-Space-OOM

Compile and Run the project with following steps:
1. Build the kernel module in MemoryKernelModule directory using the **make** command. The output file **memory_info.ko** will be generated.
2. Build the user-space OOM daemon with the **make** command in UserSpaceOOM directory. The output file **userOOM** will be generated in that directory.
3. Build the test program in test directory with **make** command. The output file **userOOMtest** will be generated in that directory.
4. Insert the **memory_info** kernel module using command **sudo insmod memory_info.ko**. *dmesg* will show the status of the module. On sucessful insertion of the module, you will see the percentages of available memory and swap memory printed in the kernel log(dmesg).
5. Now run the **userOOM** daemon [cmd = ./userOOM]. This will keep on running in that terminal window until you manually terminate by ^Z.
6. Open another terminal window, and run the **userOOMtest** to test the daemon [cmd = ./userOOMtest]. The test program will keep on allocating memory until system runs out of memory. The user-space daemon will kill the test process to reclaim memory. The test program prints its *PID* which can used to see if *userOOM* kills the right task.

Refer the attached report **User_Space_OOM_Killer.pdf** for the design and evaluation of the system.

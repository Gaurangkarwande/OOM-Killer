# User-Space-OOM
[with priority bassed killing]

Compile and Run the project with following steps:

0. git checkout priority-killing
1. Build the kernel module in MemoryKernelModule directory using the **make** command. The output file **memory_info.ko** will be generated.
2. Build the user-space OOM daemon with the **make** command in UserSpaceOOM directory. The output file **userOOM** will be generated in that directory.
3. Build the test program in **test_proc1** directory with **make** command. The output file **userOOMtest** will be generated in that directory. The priority of this process is 100.
4. Build the test program in **test_proc2** directory with **make** command. The output file **userOOMtest** will be generated in that directory. The priority of this process is 10. We now have two test processes with assigned priorities that we will run to check which one gets killed earlier. Note that - Process 2 is more memory intensive than Process 1.
5. Insert the **memory_info** kernel module using command **sudo insmod memory_info.ko**. *dmesg* will show the status of the module. On sucessful insertion of the module, you will see the percentages of available memory and swap memory printed in the kernel log(dmesg).
6. Create new folder **user_processes** in your **/tmp** directory.
```
cd /tmp
mkdir user_processes
```
We will be dynamically storing the process priorities in this directory.

7. Now run the **userOOM** daemon [cmd = ./userOOM]. This will keep on running in that terminal window until you manually terminate by ^Z.
8. Open two more terminal window or tmux panes, go to test_proc1 in first window and test_proc2 in second window and run the **userOOMtest** in both simultaneosly.
```
./userOOMtest
```


 The test programs will keep on allocating memory until system runs out of memory. The user-space daemon will kill the test processes to reclaim memory. The test program prints its *PID* which can used to see if *userOOM* kills the right task.

 **As test_proc1 has lower priority (100) than test_proc2 (10), test_proc1 is killed first.**

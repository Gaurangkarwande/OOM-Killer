# User-Space-OOM
[with priority bassed killing]

Compile and Run the project with following steps:

0. git checkout master
1. Build the kernel module in MemoryKernelModule directory using the **make** command. The output file **memory_info.ko** will be generated.
2. Build the PSI Interface file with the **make** command in PSI_Monitor file. The output file **PSI_Monitor** will be generated in that directory.
3. Build the user-space OOM daemon with the **make** command in UserSpaceOOM directory. The output file **userOOM** will be generated in that directory.
4. Build the test program in **test_proc1** directory with **make** command. The output file **userOOMtest** will be generated in that directory. The utility of this process is 8.
5. Build the test program in **test_proc2** directory with **make** command. The output file **userOOMtest** will be generated in that directory. The utility of this process is 5. We now have two test processes with assigned priorities that we will run to check which one gets killed earlier. Note that - Process 2 is more memory intensive than Process 1.
6. In order to run the ML Processes, following steps need to be executed in the command line.
```
 pip3 install pytorch
 sudo dnf install opencv 
 pip install opencv-python
 ```
7. Insert the **memory_info** kernel module using command **sudo insmod memory_info.ko**. *dmesg* will show the status of the module. On sucessful insertion of the module, you will see the percentages of available memory and swap memory printed in the kernel log(dmesg).
8. Create new folder **user_processes** in your **/tmp** directory. We will be dynamically storing the process priorities in this directory.

```
cd /tmp
mkdir user_processes
```

9. Now run the **userOOM** daemon [cmd = ./userOOM]. This will keep on running in that terminal window until you manually terminate by ^Z.
10. Now run the PSI Interface in another window **PSI_Monitor** [cmd = ./PSI_Mem]. The set threshold for fullavg10, poll interval and recovery interval will be displayed along with continuous monitoring of full avg10 values. The can be manually terminated by ^Z.
11. In a new test window, now run the bash script testproc.sh to run two ML Test processes and the two dummy memory allocaton processes.
```
bash testproc.sh
```

The two heavy ML processes will be exacting on the memory along with the two dummy test programs that will keep allocating memory until system runs out of memory.
The user-space daemon will kill the processes with lower cost (One ML process and two test programs will be killed in this case)to reclaim memory. The test program and ML tests prints its *PID* which can used to see if *userOOM* kills the right task. 
The statistics of the running process such as OOM_Score, Value and VmRSS will be displayed. The command line will display if a process is killed.
The test processes that were killed will be displayed along with the memory amount of memory freed and the sum of user defined values.

 **The ML process with a value closest to the set threshold in the knapsack algorithm will be retained. The other ML processes and dummy test program will be terminated**

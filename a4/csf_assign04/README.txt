CONTRIBUTIONS

ahashi1 and eappiah4 worked together on the implementation and report

REPORT

TODO: add your report according to the instructions in the
"Experiments and analysis" section of the assignment description.


Test run with threshold 2097152

real    0m0.431s
user    0m0.414s
sys     0m0.014s
Test run with threshold 1048576

real    0m0.270s
user    0m0.474s
sys     0m0.022s
Test run with threshold 524288

real    0m0.173s
user    0m0.469s
sys     0m0.035s
Test run with threshold 262144

real    0m0.145s
user    0m0.593s
sys     0m0.055s
Test run with threshold 131072

real    0m0.143s
user    0m0.553s
sys     0m0.060s
Test run with threshold 65536

real    0m0.154s
user    0m0.574s
sys     0m0.091s
Test run with threshold 32768

real    0m0.172s
user    0m0.609s
sys     0m0.119s
Test run with threshold 16384

real    0m0.162s
user    0m0.608s
sys     0m0.169s

As we lower the threshold, as expected, we see a decrease in the total time. However, after about 4 runs, we see the total time start to go up. This is likely due to the fact that we only have 4 CPU cores available. So after about 4 runs, we can't truly run our processes concurrently and instead the OS has to employ context switching. This has a cost and could be the reason for our increase in time. 

Of course, as we lower the threshold, we are running more parallel processes and this could be the reason we see an increase in the sys times. In essence, the OS kernel is given more work to do when the threshold is lower. 

And for the user times, we see an overall increase in time as we increase the threshold. This could be because decreasing the threshold increases the amount of parallel operations, which increases the number of child processes which will ultimately contribute to an increase in user time. 



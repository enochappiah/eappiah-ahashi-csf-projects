ahashi1 and eappiah4 worked on all the functions together and refactored code to make it more readable

Comparing the values from different combinations of write-through, write-back, write-allocate, no-write-allocate, FIFO, and LRU. This can show us how they all perform and can help us narrow down the best configuration!

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 9280883

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 311017
Load misses: 7180
Store hits: 163705
Store misses: 33781
Total cycles: 23102502

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 24706383

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 8867683

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 no-write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 311613
Load misses: 6584
Store hits: 164819
Store misses: 32667
Total cycles: 22865216

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 24394683

**************************************************************************************

Then we tried to do the same for smaller files like read02.trace to give us more perspective on how the different configurations perform with different file sizes. 

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-back lru < read02.trace
Total loads: 10
Total stores: 0
Load hits: 9
Load misses: 1
Store hits: 0
Store misses: 0
Total cycles: 410

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 no-write-allocate write-through lru < read02.trace
Total loads: 10
Total stores: 0
Load hits: 9
Load misses: 1
Store hits: 0
Store misses: 0
Total cycles: 410

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-through lru < read02.trace
Total loads: 10
Total stores: 0
Load hits: 9
Load misses: 1
Store hits: 0
Store misses: 0
Total cycles: 410

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-back fifo < read02.trace
Total loads: 10
Total stores: 0
Load hits: 9
Load misses: 1
Store hits: 0
Store misses: 0
Total cycles: 410

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 no-write-allocate write-through fifo < read02.trace
Total loads: 10
Total stores: 0
Load hits: 9
Load misses: 1
Store hits: 0
Store misses: 0
Total cycles: 410

[ahashi1@ugrad3 a3]$ ./csim 256 4 16 write-allocate write-through fifo < read02.trace
Total loads: 10
Total stores: 0
Load hits: 9
Load misses: 1
Store hits: 0
Store misses: 0
Total cycles: 410

**************************************************************************************

Despite all the values in the smaller files appearing to be the same, we can start to pinpoint a good configuration. We wanted to see which configuration had the largest number of hits and the lowest number of misses. This is because a larger number of hits would mean we don't have to reach in main memory that often and we want a lower number of misses because again, we don't want to reach in memory that often. With this, we found that write-through write-allocate LRU and write-back write-allocate LRU work the best. However, if we had to pick one we would go with write-back write-allocate LRU becaue it uses less cycles. And since the difference in cycles are by a wide margin we can safely say it will be more efficient. 

**************************************************************************************

Now we should find which number of sets, blocks, and bytes is the best. This is a bit tricky so we can try to analyze the biggest and smallest values for each. So we can have the number of sets be either 1 or 256. The number of blocks be either 1 or 256. And the number of bytes be either 4 or 64. 

[ahashi1@ugrad3 a3]$ ./csim 1 1 4 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 16869
Load misses: 203799
Store hits: 6150
Store misses: 76375
Total cycles: 35956893

[ahashi1@ugrad3 a3]$ ./csim 1 1 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 62584
Load misses: 158084
Store hits: 37316
Store misses: 45209
Total cycles: 397903193

[ahashi1@ugrad3 a3]$ ./csim 1 256 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220127
Load misses: 541
Store hits: 79422
Store misses: 3103
Total cycles: 10715993

[ahashi1@ugrad3 a3]$ ./csim 256 256 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 220275
Load misses: 393
Store hits: 79465
Store misses: 3060
Total cycles: 5827993

[ahashi1@ugrad3 a3]$ ./csim 256 1 4 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 181851
Load misses: 38817
Store hits: 54947
Store misses: 27578
Total cycles: 9321093

[ahashi1@ugrad3 a3]$ ./csim 256 256 4 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 219148
Load misses: 1520
Store hits: 61677
Store misses: 20848
Total cycles: 2539993

[ahashi1@ugrad3 a3]$ ./csim 256 1 64 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 218041
Load misses: 2627
Store hits: 78826
Store misses: 3699
Total cycles: 15944793

[ahashi1@ugrad3 a3]$ ./csim 1 256 4 write-allocate write-back lru < swim.trace
Total loads: 220668
Total stores: 82525
Load hits: 208088
Load misses: 12580
Store hits: 59947
Store misses: 22578
Total cycles: 5986693

**************************************************************************************

Based on the data we got back with different combinatiosn of small and large values we found a potential solution. It's evident that using the largest number of sets, blocks, and bytes results in the most efficient cache. Perhaps the most glaring improvement is the number of cycles. 

Thus we found that the best configuration would be: 256 256 64 write-allocate write-back LRU

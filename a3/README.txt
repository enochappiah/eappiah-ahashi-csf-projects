ahashi1 and eappiah4 worked on all the functions together and refactored code to make it more readable

sm: 1 1 4
lg: 256 256 64


sm sm sm write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 13415
Load misses: 304782
Store hits: 13556
Store misses: 183930
Total cycles: 67776483

lg lg lg write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5902883

sm sm lg write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 133602
Load misses: 184595
Store hits: 120038
Store misses: 77448
Total cycles: 543698083

lg sm sm write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 263687
Load misses: 54510
Store hits: 153611
Store misses: 43875
Total cycles: 14665983

sm lg sm write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 307414
Load misses: 10783
Store hits: 167494
Store misses: 29992
Total cycles: 7515283

sm lg lg write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316383
Load misses: 1814
Store hits: 194867
Store misses: 2619
Total cycles: 11403683

lg lg sm write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 313766
Load misses: 4431
Store hits: 170130
Store misses: 27356
Total cycles: 3694383

lg sm lg write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 312297
Load misses: 5900
Store hits: 194064
Store misses: 3422
Total cycles: 20512483

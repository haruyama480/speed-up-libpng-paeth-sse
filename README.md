# speed-up-libpng-paeth-sse

not yet improved

## requirements
- g++
- cmake
- SSE 4.1

## result
(aws t3.large)
```
2024-08-10T16:01:43+00:00
Running ./bench
Run on (2 X 2500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x1)
  L1 Instruction 32 KiB (x1)
  L2 Unified 1024 KiB (x1)
  L3 Unified 36608 KiB (x1)
Load Average: 0.92, 0.46, 0.36
----------------------------------------------------------
Benchmark                Time             CPU   Iterations
----------------------------------------------------------
BM_SIMPLE/512         4748 ns         4215 ns       166122
BM_SIMPLE/4096       40126 ns        40019 ns        17143
BM_SIMPLE/32768     354235 ns       323775 ns         2187
BM_BEFORE/512         1780 ns         1699 ns       408811
BM_BEFORE/4096        4299 ns         3806 ns       185398
BM_BEFORE/32768      23103 ns        20677 ns        33464
BM_AFTER/512          1853 ns         1706 ns       410090
BM_AFTER/4096         4398 ns         3894 ns       179185
BM_AFTER/32768       22509 ns        20845 ns        32746
```

### environments
AMI
- CMake on AWS Linux 2023 with maintenance support by Apps4Rent
- https://aws.amazon.com/marketplace/pp/prodview-iw7eppjowfffs?sr=0-7&ref_=beagle&applicationId=AWSMPContessa

EC2
- t3.large
  - memory: 8 GiB
  - CPU: 2 virtual cores

### procedure detail
install devtools
```
sudo yum install -y gcc-c++
sudo yum groupinstall -y "Development Tools"
```

check requirements
```
[root@ip-172-31-42-111 build]# g++ --version | head -1
g++ (GCC) 11.3.1 20221121 (Red Hat 11.3.1-4)
[root@ip-172-31-42-111 build]# cmake --version | head -1
cmake version 3.22.2
[root@ip-172-31-42-111 build]# cat /proc/cpuinfo | grep sse4_1
flags           : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss ht syscall nx pdpe1gb rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc cpuid tsc_known_freq pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm 3dnowprefetch invpcid_single pti fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx avx512f avx512dq rdseed adx smap clflushopt clwb avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves ida arat pku ospke
```

get the benchmark
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./bench
```

etc
```
[root@ip-172-31-42-111 build]# ./test
Running main() from /root/proj/build/_deps/googletest-src/googletest/src/gtest_main.cc
[==========] Running 6 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 6 tests from TestPaeth
[ RUN      ] TestPaeth.origin_paeth
[       OK ] TestPaeth.origin_paeth (0 ms)
[ RUN      ] TestPaeth.branchlessabs_paeth
[       OK ] TestPaeth.branchlessabs_paeth (127 ms)
[ RUN      ] TestPaeth.absless_paeth
[       OK ] TestPaeth.absless_paeth (115 ms)
[ RUN      ] TestPaeth.sse_abs_paeth_libpng
[       OK ] TestPaeth.sse_abs_paeth_libpng (124 ms)
[ RUN      ] TestPaeth.sse_abs_paeth
[       OK ] TestPaeth.sse_abs_paeth (120 ms)
[ RUN      ] TestPaeth.sse_absless_paeth
[       OK ] TestPaeth.sse_absless_paeth (97 ms)
[----------] 6 tests from TestPaeth (586 ms total)

[----------] Global test environment tear-down
[==========] 6 tests from 1 test suite ran. (586 ms total)
[  PASSED  ] 6 tests.


[root@ip-172-31-42-111 build]# g++ --version
g++ (GCC) 11.3.1 20221121 (Red Hat 11.3.1-4)
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

[root@ip-172-31-42-111 build]# cmake --version
cmake version 3.22.2

CMake suite maintained and supported by Kitware (kitware.com/cmake).
[root@ip-172-31-42-111 build]# cat /proc/cpuinfo
processor       : 0
vendor_id       : GenuineIntel
cpu family      : 6
model           : 85
model name      : Intel(R) Xeon(R) Platinum 8259CL CPU @ 2.50GHz
stepping        : 7
microcode       : 0x5003707
cpu MHz         : 2499.996
cache size      : 36608 KB
physical id     : 0
siblings        : 2
core id         : 0
cpu cores       : 1
apicid          : 0
initial apicid  : 0
fpu             : yes
fpu_exception   : yes
cpuid level     : 13
wp              : yes
flags           : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss ht syscall nx pdpe1gb rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc cpuid tsc_known_freq pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm 3dnowprefetch invpcid_single pti fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx avx512f avx512dq rdseed adx smap clflushopt clwb avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves ida arat pku ospke
bugs            : cpu_meltdown spectre_v1 spectre_v2 spec_store_bypass l1tf mds swapgs itlb_multihit mmio_stale_data retbleed gds
bogomips        : 4999.99
clflush size    : 64
cache_alignment : 64
address sizes   : 46 bits physical, 48 bits virtual
power management:

processor       : 1
vendor_id       : GenuineIntel
cpu family      : 6
model           : 85
model name      : Intel(R) Xeon(R) Platinum 8259CL CPU @ 2.50GHz
stepping        : 7
microcode       : 0x5003707
cpu MHz         : 2499.996
cache size      : 36608 KB
physical id     : 0
siblings        : 2
core id         : 0
cpu cores       : 1
apicid          : 1
initial apicid  : 1
fpu             : yes
fpu_exception   : yes
cpuid level     : 13
wp              : yes
flags           : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss ht syscall nx pdpe1gb rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc cpuid tsc_known_freq pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm 3dnowprefetch invpcid_single pti fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx avx512f avx512dq rdseed adx smap clflushopt clwb avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves ida arat pku ospke
bugs            : cpu_meltdown spectre_v1 spectre_v2 spec_store_bypass l1tf mds swapgs itlb_multihit mmio_stale_data retbleed gds
bogomips        : 4999.99
clflush size    : 64
cache_alignment : 64
address sizes   : 46 bits physical, 48 bits virtual
power management:

[root@ip-172-31-42-111 build]# cat /proc/meminfo
MemTotal:        7999092 kB
MemFree:         6641128 kB
MemAvailable:    7454888 kB
Buffers:            1900 kB
Cached:           991544 kB
SwapCached:            0 kB
Active:           270824 kB
Inactive:         779396 kB
Active(anon):        868 kB
Inactive(anon):    64508 kB
Active(file):     269956 kB
Inactive(file):   714888 kB
Unevictable:           0 kB
Mlocked:               0 kB
SwapTotal:             0 kB
SwapFree:              0 kB
Zswap:                 0 kB
Zswapped:              0 kB
Dirty:                 0 kB
Writeback:             0 kB
AnonPages:         56776 kB
Mapped:            70544 kB
Shmem:              8600 kB
KReclaimable:      98236 kB
Slab:             185384 kB
SReclaimable:      98236 kB
SUnreclaim:        87148 kB
KernelStack:        1904 kB
PageTables:         2272 kB
SecPageTables:         0 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:     3999544 kB
Committed_AS:     275248 kB
VmallocTotal:   34359738367 kB
VmallocUsed:       11472 kB
VmallocChunk:          0 kB
Percpu:              888 kB
HardwareCorrupted:     0 kB
AnonHugePages:      2048 kB
ShmemHugePages:        0 kB
ShmemPmdMapped:        0 kB
FileHugePages:         0 kB
FilePmdMapped:         0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
Hugetlb:               0 kB
DirectMap4k:       92592 kB
DirectMap2M:     5025792 kB
DirectMap1G:     3145728 kB
```

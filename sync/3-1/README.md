time cp -r test_src/ test_dst/

real    0m15.855s
user    0m0.017s
sys     0m13.832s


time ./cp_r test_src/ test_dst/

real    0m3.710s
user    0m0.547s
sys     0m26.811s

time ./cp_r_sync ../test_src/ ../test_dst/

real    0m4.964s
user    0m0.788s
sys     0m31.872s

aldor -Fo aside.as
cc -std=c99 -I$ALDORROOT/include -c cside.c
cc cside.o aside.o -o lcm64 -L$BMROOT/lib -L$ALDORROOT/lib -laldor -lfoam -lm
./lcm64

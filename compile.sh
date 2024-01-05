cflags = ("-std=c17", "-Wall", "-Wextra", "-Wpedantic", "-O3", "-lpthread", "-latomic", "-Wl,--stack,16777216", "-g0",
    "-march=native")    # compiler arguments
    
tunits = ()    # translation units
out = # name of the output file

for arg in "$@" # captures all arguments passed to the script upon invocation
do
    if [[arg == *.c*]]; then tunits += arg;
done
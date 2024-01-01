$cfiles = [System.Collections.ArrayList]::new()
$unrecognized = [System.Collections.ArrayList]::new()


foreach ($arg in $args) {
    if ($arg -clike "*.c") {
        [void]$cfiles.Add($arg.ToString().Replace(".\", ""))
    }
    else {
        [void]$unrecognized.Add($arg)
    }
}

if ($unrecognized.Count -ne 0) {
    Write-Error "Incompatible files passed for compilation: ${unrecognized}"
    Exit 1
}

$exe = $cfiles[0].Replace(".c", ".exe")

$cflags = @( 
    "-O3",
    "-g0",
    "-march=native",
    "-latomic",     # work around some errors that occur with _Atomic floats and doubles, link the libatomic library
    "-lpthread",    # link pthread
    "-std=c17"
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wl,--stack,16777216",
    "-o${exe}"  # do not leae a space beofre -o and ${}
)

Write-Host "gcc.exe ${cfiles} ${cflags}" -ForegroundColor Cyan
gcc.exe $cfiles $cflags    

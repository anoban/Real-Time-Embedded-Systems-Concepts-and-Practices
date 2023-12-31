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

$cflags = @( 
    "-mavx"
    "-O3",
    "-g0",
    "-std=c17"
    "-Wall",
    "-Wextra",
    "-Wpedantic"
)

Write-Host "gcc.exe ${cfiles} ${cflags}" -ForegroundColor Cyan
gcc.exe $cfiles $cflags    

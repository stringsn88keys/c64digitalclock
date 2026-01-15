# Build script for PETSCII digital clock on Commodore 64 using cc65
# PowerShell script for Windows

# Check if cc65 is in the expected location
if (-Not (Test-Path "../cc65")) {
    Write-Error "Error: cc65 directory not found at ../cc65"
    exit 1
}

# Add cc65 to PATH
$env:PATH = "../cc65/bin;$env:PATH"

# Compile the program
cl65 -t c64 -o clock.prg clock.c

# Check if compilation was successful
if ($LASTEXITCODE -eq 0) {
    Write-Output "Build successful: clock.prg created"
} else {
    Write-Error "Build failed"
    exit 1
}

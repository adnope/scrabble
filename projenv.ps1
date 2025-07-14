# projenv.ps1

# Build all or build a target
function build {
  param(
    [string]$target
  )

  $buildDir = "build"
  if (-Not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir
    cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ --no-warn-unused-cli -S . -B $buildDir -G "Ninja"
  }

  if (-Not $target) {
    cmake --build $buildDir --config Debug --target all
  } else {
    cmake --build $buildDir --config Debug --target $target
  }
}

# Build and run the program
function scrabble {
  param(
    [string]$arg1,
    [string]$arg2,
    [string]$arg3
  )

  Build -target "scrabble"
  $execPath = "build\bin\scrabble.exe"
  if (Test-Path $execPath) {
    & $execPath $arg1 $arg2 $arg3
  } else {
    Write-Host "Error: '$execPath' not found"
  }
}

# Build and run the test
function test {
  param(
    [string]$arg1,
    [string]$arg2,
    [string]$arg3
  )

  Build -target "scrabble_test"
  $execPath = "build\bin\scrabble_test.exe"
  if (Test-Path $execPath) {
    & $execPath $arg1 $arg2 $arg3
  } else {
    Write-Host "Error: '$execPath' not found"
  }
}

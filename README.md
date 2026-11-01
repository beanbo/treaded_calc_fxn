# treaded_calc_fxn

A Windows console program from 2021 (Visual Studio project `SP_lab3`) that evaluates the same
numeric series twice — in one thread and split across 5 `std::thread`s — measures both runs in
processor cycles with `__rdtsc`, and writes the result through the
[Simple-logger](https://github.com/beanbo/Simple-logger) DLL.

## How it works (from `SP_lab3/SP_lab3.cpp`)

- The series is a sum over `i = 1..n` of `1 / S(i)`, where
  `S(i) = Σ_{j=1..i} (j + pow(x + j, 1/k)) / (2·i·j − 1)`, with hard-coded `x = 1000`,
  `n = 500`, `k = 3`.
- The single-threaded run prints progress every 10 % with seconds spent and an estimate of
  seconds left.
- The threaded run splits `1..n` into 5 ranges, one thread each, and adds the partial sums.
- It prints `Tacts to standard Fxn: …` and `Tacts to threaded Fxn: …`, then loads `logger.dll`
  and appends the same two numbers to `result.log`.

Known quirk, left as is: `1 / k` is integer division, so `pow(x + j, 1 / k)` is always `1`.

## Stack

- C++17 (`std::thread`, `__rdtsc`), WinAPI (`LoadLibrary`, `MessageBox`)
- Visual Studio solution `SP_lab3.sln`, platform toolset v142 (Visual Studio 2019)
- A prebuilt `logger.dll` is committed in `SP_lab3/`

## Build and run

Open `SP_lab3.sln` in Visual Studio 2019 or newer, build, and run with `logger.dll` next to the
executable or in the working directory. Not verified here: the project needs Windows and MSVC.

## Context

Laboratory work No. 3 of a system programming course (the project name `SP_lab3`), 2021.

# ls3w2iq

## Introduction

LabSat 3 Wideband is a three-channel recorder. This program aims at striping the signals from different channels. The output file can be read and processed by software defined reciever like [SoftGNSS](https://github.com/gnsscusdr/CU-SDR-Collection).

## Build

For macOS users, you need Clang compiler.

```bash
xcode-select --install
```

For Linux users, you need gcc complier. You can install gcc with your package manager. For example, for Ubuntu and Debian users:

```bash
sudo apt install gcc
```

I don't know how to compile it on Windows. Maybe you need Visual Studio.

Then clone this repo and build:

```
git clone https://github.com/sunruisjtu2020/ls3w2iq.git --depth=1 && cd ls3w2iq/src && make
```

Then you get the program ls3w2iq.

## Usage

For example, you got a record (`File_001.LS3W` and `File_001.ini`) from LabSat. You can stripe the record into three channels.

```bash
./ls3w2iq -i File_001.LS3W -o File_001
```

And you'll get `File_001_A.iq`, `File_001_B.iq`, `File_001_C.iq`. You can process these files with SoftGNSS or other SDR.
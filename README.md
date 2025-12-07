# Soundwave, Proccess WAV soundfiles

## How it works
* Processes WAV audio files via standard input/output (stdin/stdout) for easy shell piping.

* Validates WAV headers and ensures file integrity before performing any operations.

* Supports the following audio transformations:

	1. info – Displays detailed metadata about the WAV file (sample rate, format, data size, etc.).

	2. rate (factor) – Adjusts playback sample rate by a floating-point factor.

	3. channel left/right – Extracts a single audio channel from a stereo WAV file.

	4. volume (multiplier) – Scales audio amplitude for 8-bit and 16-bit PCM data.

	5. generate – Produces a synthetic FM-modulated mono WAV signal with parameters for duration, sample rate, modulation frequency, carrier frequency, modulation index, and amplitude.

* Ensures WAV format correctness, including chunk sizes, bit depth, and block alignment.

* Emits transformed WAV data through stdout and preserves any trailing non-audio chunks.

* Fails safely with clear error messages if input is malformed or parameters are invalid.

* Generate expects the arguments after the -- to also contain a number
## Example
```
λ ./soundwave info < ../wav/good.wav
size of file: 133
size of format chunk: 16
WAVE type format: 1
mono/stereo: 1
sample rate: 44100
bytes/sec: 88200
block alignment: 2
bits/sample: 16
size of data chunk: 88
```

## Cloning
```sh
git clone https://github.com/metallaxis/soundwave.git
```
or for also cloning the sounds:
```sh
git clone --recursive https://github.com/metallaxis/soundwave.git
```

## Compiling
After cloning the repository run the following command:
```sh
make build (windows/linux)
```

## Running: 
Go to the build directory that was created from
```sh
make build (windows/linux)
```
and run:
```sh
./soundwave
```
Then it will show the helper window with all the available subcommands

## Sources / Extra Info
[WAV File info](https://en.wikipedia.org/wiki/WAV)<br>
[HW1 Webpage](https://progintro.github.io/assets/pdf/hw1.pdf)<br>

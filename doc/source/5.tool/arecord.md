# arecord
arecord is command-line sound recorder and player for ALSA soundcard driver


## Description
arecord is a command-line soundfile recorder for the ALSA soundcard driver. It supports several file formats and multiple soundcards with multiple devices. If recording with interleaved mode samples the file is automatically split before the 2GB filesize.

aplay is much the same, only it plays instead of recording. For supported soundfile formats, the sampling rate, bit depth, and so forth can be automatically determined from the soundfile header.

If filename is not specified, the standard output or input is used. The aplay utility accepts multiple filenames


## Usage

```sh
arecord [flags] [filename]
```

If the filename is not specified then it uses Standard input. Options:

```
-h, –help : It is used to show the help messages and exit.
–version : Print current version.
-l, –list-devices : List all soundcards and digital audio devices.
-L, –list-pcms : List all PCMs(Pulse Code Modulation) defined.
-D, –device=NAME : Select PCM by name.
-q –quiet : Quiet mode. Suppress messages (not sound :).
-t, –file-type TYPE File type (voc, wav, raw or au). If this parameter is omitted the WAVE format is used.
-c, –channels=# : The number of channels. The default is one channel. Valid values are 1 through 32.
-f –format=FORMAT : If no format is given U8 is used.
-r, –rate=# : Sampling rate in Hertz. The default rate is 8000 Hertz.
-d, –duration=# : Interrupt after # seconds.
-s, –sleep-min=# : Min ticks to sleep. The default is not to sleep.
-M, –mmap : Use memory-mapped (mmap) I/O mode for the audio stream. If this option is not set, the read/write I/O mode will be used.
-N, –nonblock : Open the audio device in non-blocking mode. If the device is busy the program will exit immediately.
```

## Examples

0. record audio from the default audio input device and save it to a file called recording.wav:

```sh
# arecord -f cd -t wav recording.wav
```

1. Record a snippet in “CD” quality (finish with Ctrl-C when done):
```sh
# arecord -vv --format=cd {{path/to/file.wav}}
```
2. Record a snippet in “CD” quality, with a fixed duration of 10 seconds:

```sh
# arecord -vv --format=cd --duration={{10}} {{path/to/file.wav}}
```

3. Record a snippet and save it as an MP3 (finish with Ctrl-C when done):
```sh
# arecord -vv --format=cd --file-type raw | lame -r - {{path/to/file.mp3}}
```

4. List all sound cards and digital audio devices:
```sh
# arecord --list-devices
```

5. Allow interactive interface (e.g. use space-bar or enter to play or pause):
```sh
# arecord --interactive
```
# Audio Pipeline


## samples
### 录制语音

```sh
gst-launch-1.0 -vv alsasrc device=hw:2,0 ! audioconvert ! audioresample ! capsfilter caps="audio/x-raw,format=S16LE,channels=1,rate=16000" ! wavenc ! filesink location="test.wav"
```

### 回放语音

```sh

gst-launch-1.0 -vv alsasrc device=hw:2,0 ! audioconvert ! audioresample ! capsfilter caps="audio/x-raw,format=S16LE,channels=1,rate=16000" ! autoaudiosink
```


### 播放一段测试语音

```sh
gst-launch-1.0 audiotestsrc wave=5 volume=0.3 ! audioconvert ! autoaudiosink
```

the wave parameters can be:

```
 wave                : Oscillator waveform
    flags: readable, writable, controllable
    Enum "GstAudioTestSrcWave" Default: 0, "sine"
        (0): sine             - Sine
        (1): square           - Square
        (2): saw              - Saw
        (3): triangle         - Triangle
        (4): silence          - Silence
        (5): white-noise      - White uniform noise
        (6): pink-noise       - Pink noise
        (7): sine-table       - Sine table
        (8): ticks            - Periodic Ticks
        (9): gaussian-noise   - White Gaussian noise
        (10): red-noise        - Red (brownian) noise
        (11): blue-noise       - Blue noise
        (12): violet-noise     - Violet noise
```


* 播放来自麦克风的声音

```sh
gst-launch-1.0 pulsesrc device=$MIC_DEV_ID ! "audio/x-raw,format=S16LE,rate=16000,channels=1" ! autoaudiosink

gst-launch-1.0 alsasrc device=hw:2,0 ! autoaudiosink

```
- MIC_DEVICE_ID can be got by `gst-device-monitor-1.0 Audio/Source "audio/x-raw,format=S16LE,rate=16000,channels=1"`

* 将收到的语音进行 AGC(自动增益), AEC(回声消除) and ANS(噪声抑制) 处理

```
gst-launch-1.0 pulsesrc device=$MIC_DEVICE_ID ! "audio/x-raw,format=S16LE,rate=16000,channels=1" ! webrtcdsp ! webrtcechoprobe ! autoaudiosink
```

* 转换语音包为 g.711 编码

```sh
gst-launch-1.0 pulsesrc device=$MIC_DEVICE_ID ! "audio/x-raw,format=S16LE,rate=16000,channels=1" ! alawenc
```

* write out audio file to disk (using same tone):

```sh

gst-launch-1.0 audiotestsrc ! audioconvert ! capsfilter caps = "audio/x-raw,format=S16LE,channels=1,rate=16000" ! filesink location = "audio.raw"
```

* read audio file from disk (should play the same tone):

```sh
gst-launch-1.0 filesrc location="audio.raw" ! capsfilter caps = "audio/x-raw,format=S16LE,channels=1,rate=16000" ! audioconvert ! audoaudiosink
```


* decode mp3 and write to disk:

```sh
gst-launch-1.0 filesrc location = 16k16bit.mp3 ! decodebin ! audioconvert ! capsfilter caps = "audio/x-raw,format=S16LE,channels=1,rate=16000" ! filesink location = "audio.raw"
```


* 将来自麦克风的音频录制成 wave file

```sh
# macos
gst-launch-1.0 osxaudiosrc ! wavenc ! filesink location=audio.wav

# linux
gst-launch-1.0 pulsesrc device=xxx ! wavenc ! filesink location=test.wav

gst-launch-1.0 alsasrc device=hw:3,0 ! wavenc ! filesink location=test.wav
```

* 将语音录制为 hls 文件

```sh
gst-launch-1.0 pulsesrc device=xxx ! avenc_aac ! hlssink2 max-files=5

gst-launch-1.0 osxaudiosrc device=xxx ! avenc_aac ! hlssink2 max-files=5
```

* 将语音录制为 hls 文件以及 wave file
```sh

gst-launch-1.0 pulsesrc device=$MIC_DEVICE_ID ! "audio/x-raw,format=S16LE,rate=16000,channels=1" ! webrtcdsp ! webrtcechoprobe ! audioconvert ! tee name=t ! queue ! avenc_aac ! hlssink2 max-files=5 playlist-location=waltertest.m3u8 location=waltertest_%05d.ts t. ! queue ! wavenc ! filesink location=waltertest.wav

```


* transmit audio 

gst-launch-1.0 -v wasapisrc loopback=true ! audioconvert ! udpsink host=239.0.0.2 port=9998

* receive audio

```sh
gst-launch-1.0 -v udpsrc address=239.0.0.2 port=9998 multicast-group=239.0.0.1 caps="audio/x-raw,format=F32LE,rate=48000,channels=2" ! queue ! audioconvert ! autoaudiosink
# or 
gst-launch-1.0 -v udpsrc address=239.0.0.2 port=9998 multicast-group=239.0.0.1 caps="audio/x-raw,format=S16LE,rate=48000,channels=2" ! queue ! audioconvert ! autoaudiosink
```

* play pcm file
```sh
gst-launch-1.0 filesrc location=iat_pcm_16k.pcm ! audio/x-raw, format=S16LE, channels=1, layout=interleaved, rate=16000 ! autoaudiosink
 
```

* send mp3 audio to remote over udp

```sh
gst-launch-1.0 -v filesrc location=/tmp/musicfile.mp3 ! mpegaudioparse ! mpg123audiodec ! audioconvert ! audioresample ! audio/x-raw, rate=16000, channels=1, format=S16LE ! audiomixer blocksize=320 ! udpsink host=192.168.1.10 port=10000


gst-launch-1.0 -v udpsrc port=10000 ! rawaudioparse use-sink-caps=false format=pcm pcm-format=s16le sample-rate=16000 num-channels=1 ! queue ! audioconvert ! audioresample ! autoaudiosink
```

* convert wav to pcm

```sh
gst-launch filesrc location=input.wav ! wavparse ! audioresample ! audioconvert \
! audio/x-raw-int, rate=8000, channels=1, endianness=4321, width=16, depth=16, signed=true \
! filesink location=output.pcm 

gst-launch-1.0 filesrc location=file.wav ! wavparse ! audioresample ! audioconvert \
! audio/x-raw,format=S16BE,channels=1,rate=8000 ! filesink location=file.pcm

```


* convert mp3 to hls(m3u8 + ts)

```sh
gst-launch-1.0 -v filesrc location=material/16k16bit.mp3 ! mpegaudioparse ! mpg123audiodec ! audioconvert ! avenc_aac ! hlssink2 max-files=5

gst-launch-1.0 -v filesrc location=material/16k16bit.mp3 ! mpegaudioparse ! mpg123audiodec ! audioconvert ! alawenc ! hlssink2 max-files=5
```

* display audio visualisation 音频可视化

```sh
gst-launch-1.0 audiotestsrc wave=9 ! audioconvert ! spacescope ! glimagesink

gst-launch-1.0 audiotestsrc ! audioconvert ! wavescope ! glimagesink

# on ubuntu, the device id can be got by arecord -l
gst-launch-1.0 alsasrc device=hw:3,0 ! audioconvert ! wavescope ! videoconvert ! ximagesink
```

* Convert WAV to PCM

```sh
gst-launch-1.0 filesrc location=file.wav ! wavparse ! audioresample ! audioconvert ! audio/x-raw,format=S16LE,channels=1,rate=8000 ! filesink location=file.pcm
```


* Play PCM

```sh
gst-launch-1.0 filesrc location=/tmp/test.pcm ! capsfilter caps="audio/x-raw,format=S16LE,channels=1,rate=16000" ! audioconvert ! audioresample ! autoaudiosink
```

* split mp3 to multiple wav files

```sh
gst-launch-1.0 filesrc location=audio/audio.mp3 ! decodebin ! audioconvert ! splitmuxsink location=/tmp/out_%d.wav muxer=wavenc max-size-time=10000000000
```


* encodes a test audio and video stream and muxes both into an FLV file.

```sh
gst-launch-1.0 -v flvmux name=mux ! filesink location=test.flv  \
audiotestsrc samplesperbuffer=44100 num-buffers=10 ! faac ! mux.  \
videotestsrc num-buffers=250 ! video/x-raw,framerate=25/1 ! x264enc ! mux.
```

* send audio stream to rtpmp

```sh
export RTMP_DEST="rtmp://192.168.104.199:1935/live/testaudio"

gst-launch-1.0 audiotestsrc is-live=true ! \
    audioconvert ! audioresample ! audio/x-raw,rate=48000 ! \
    voaacenc bitrate=96000 ! audio/mpeg ! aacparse ! audio/mpeg, mpegversion=4 ! \
    flvmux name=mux ! \
    rtmpsink location=$RTMP_DEST

gst-launch-1.0 alsasrc device=hw:2,0 ! audioconvert ! wavescope ! videoconvert \
  ! x264enc ! flvmux name=muxer ! rtmpsink location="$RTMP_DEST live=1"

```

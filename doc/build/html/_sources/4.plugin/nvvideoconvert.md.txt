# DeepStream Overview


## Example

* display test video

```

 DISPLAY=:0 gst-launch-1.0 -v videotestsrc pattern=21 ! video/x-raw,width=1920, height=1080, framerate=30/1 \
! nvvideoconvert ! nvegltransform \
! "video/x-raw(memory:EGLImage),format=RGBA" ! nveglglessink sync=false
```

* crop test video

```

DISPLAY=:0 gst-launch-1.0 -v videotestsrc pattern=21 ! video/x-raw,width=1920, height=1080, framerate=30/1 ! nvvideoconvert src-crop=0:244:1920:591 ! "video/x-raw(memory:NVMM),format=I420" ! nvegltransform ! "video/x-raw(memory:EGLImage),format=RGBA" ! nveglglessink sync=false
```

* crop test video and display croped video

```
DISPLAY=:0 gst-launch-1.0 -v videotestsrc pattern=21 ! video/x-raw,width=1920, height=1080, framerate=30/1 \
! nvvideoconvert src-crop=0:244:1920:591 \
! "video/x-raw(memory:NVMM),width=1248,height=384,format=I420" \
! nvegltransform ! "video/x-raw(memory:EGLImage),format=RGBA" ! nveglglessink sync=false
```


* crop by videocrop extension

```

DISPLAY=:0 gst-launch-1.0 -v videotestsrc pattern=21 ! video/x-raw,width=1920, height=1080, framerate=30/1 \
! videocrop top=244 bottom=245 ! videoscale add-borders=false ! nvvideoconvert \
! "video/x-raw(memory:NVMM),width=1248,height=384,format=I420" ! nveglglessink sync=false
```

* another example of crop
```
DISPLAY=:0 gst-launch-1.0 -v v4l2src device=/dev/video0 ! image/jpeg, width=1920, height=1080 \
! jpegdec ! video/x-raw,width=1920,height=1080,format=I420 ! videocrop top=244 bottom=245 ! videoscale \
! video/x-raw, width=1248,height=384 ! videorate ! video/x-raw,width=1248,height=384,framerate=4/1 \
! nvvideoconvert ! "video/x-raw(memory:NVMM),format=I420" ! nveglglessink
```


## updated

* original

```
DISPLAY=:0 gst-launch-1.0 -ev v4l2src device=/dev/video0 ! capsfilter caps="video/x-raw,width=1920,height=1080" \
! nvvideoconvert src-crop=0:100:800:800 \
! nvegltransform ! "video/x-raw(memory:EGLImage),format=RGBA" ! nveglglessink sync=false
```


* example 2


```
DISPLAY=:0 gst-launch-1.0 -ev v4l2src device=/dev/video0 ! capsfilter caps="video/x-raw,width=1920,height=1080" \
! nvvideoconvert src-crop=0:100:800:800 dest-crop=0:100:800:800 \
! nvegltransform ! "video/x-raw(memory:EGLImage),format=RGBA" ! nveglglessink sync=false
```


Environment=JOURNAL_STREAM=null

* updated

```
DISPLAY=:0 gst-launch-1.0 -ev v4l2src device=/dev/video0 ! capsfilter caps="video/x-raw,width=1920,height=1080" \
! nvvideoconvert src-crop=0:100:800:800 \
! "video/x-raw(memory:NVMM),width=800,height=800,format=I420" \
! nvegltransform ! "video/x-raw(memory:EGLImage),format=RGBA" ! nveglglessink sync=false
```


## Reference
* https://forums.developer.nvidia.com/t/nvvideoconvert-src-crop-dest-crop-question/260766/4
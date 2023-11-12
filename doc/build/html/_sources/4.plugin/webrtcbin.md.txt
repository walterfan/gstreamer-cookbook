# webrtcbin

## WebrtcBin 简介

webrtcbin 是由 Matthew Waters 编写的 GStreamer 插件，使用此插件，您可以连接到网络浏览器或者其他 WebRTC 端点或服务器进行音视频的实时传输。  它实现点对点连接握手（使用 ICE 和外部 STUN 服务器）、或者在无法直连时重新路由数据包（使用外部 TURN 服务器中转），然后维护传输音视频数据包的会话(DTLS, SCTP 和 SRTP)

不过需要设计自己的信令协议, 实现自己的信令服务器(这里有一个简单的[信令服务的例子](https://github.com/centricular/gstwebrtc-demos/blob/master/signalling/simple_server.py))来交换 SDP 和 ICE candidate，并处理数据包丢失和重传，管理网络拥塞并调整编码比特率，以在不同质量的网络上保持可接受的用户体验。例如常用的 NACK/PLI, FEC, RTX, Congestion Control method(REMB and TWCC)
 
参见 https://gstreamer.freedesktop.org/documentation/webrtc/index.html?gi-language=c

```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──GstBin
                    ╰──webrtcbin
```
它有一个sink pad 和一个 src pad, 收发的格式是 “application/x-rtp”

* GstWebRTCBinSinkPad
```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstPad
                ╰──GstProxyPad
                    ╰──GstGhostPad
                        ╰──GstWebRTCBinPad
                            ╰──GstWebRTCBinSinkPad
```

* GstWebRTCBinSrcPad

```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstPad
                ╰──GstProxyPad
                    ╰──GstGhostPad
                        ╰──GstWebRTCBinPad
                            ╰──GstWebRTCBinSrcPad
```

## WebrtcBin 使用实例

-- 注：这里的例子编译自 https://blog.nirbheek.in/2018/02/gstreamer-webrtc.html

* 首先我们从摄像头中捕获视频，并将视频流发送至 webrtc 的对端，并从对端接收视频流，第一步是构建视频流水线， 由插件 v4l2src 捕获视频流，放在 queue 中，由 vp8enc 来进行 vp8 编码，由 rtpvp8pay 将编码过的视频流封装成 rtp 包，再由 webrtcbin 插件通过 RTCPeerConnection 发送出去(DTLS/SRTP), 这其中要经过 SDP 协商和 ICE Candidate 协商

```
GstElement *pipe;

pipe = gst_parse_launch ("v4l2src ! queue ! vp8enc ! rtpvp8pay ! "
    "application/x-rtp,media=video,encoding-name=VP8,payload=96 !"
    " webrtcbin name=sendrecv", NULL);

```

* 获取一个 webrtcbin 的引用，并设置它的一些回调.
```
GstElement *webrtc;

webrtc = gst_bin_get_by_name (GST_BIN (pipe), "sendrecv");
g_assert (webrtc != NULL);

/* This is the gstwebrtc entry point where we create the offer.
 * It will be called when the pipeline goes to PLAYING. */
g_signal_connect (webrtc, "on-negotiation-needed",
    G_CALLBACK (on_negotiation_needed), NULL);
/* We will transmit this ICE candidate to the remote using some
 * signalling. Incoming ICE candidates from the remote need to be
 * added by us too. */
g_signal_connect (webrtc, "on-ice-candidate",
    G_CALLBACK (send_ice_candidate_message), NULL);
/* Incoming streams will be exposed via this signal */
g_signal_connect (webrtc, "pad-added",
    G_CALLBACK (on_incoming_stream), pipe);
/* Lifetime is the same as the pipeline itself */
gst_object_unref (webrtc);
```

* 当 pipeline 状态变成 PLAYING, `on_negotiation_needed() ` 回调函数将被调用， 我们将请求 webrtcbin 去创建一个匹配以上 pipeline 的 offer
```
static void
on_negotiation_needed (GstElement * webrtc, gpointer user_data)
{
  GstPromise *promise;

  promise = gst_promise_new_with_change_func (on_offer_created,
      user_data, NULL);
  g_signal_emit_by_name (webrtc, "create-offer", NULL,
      promise);
}
```

* 当 webrtcbin 创建 SDP Offer 后，其会调用 ` on_offer_created()`  回调函数

```
static void
on_offer_created (GstPromise * promise, GstElement * webrtc)
{
  GstWebRTCSessionDescription *offer = NULL;
  const GstStructure *reply;
  gchar *desc;

  reply = gst_promise_get_reply (promise);
  gst_structure_get (reply, "offer",
      GST_TYPE_WEBRTC_SESSION_DESCRIPTION, 
      &offer, NULL);
  gst_promise_unref (promise);

  /* We can edit this offer before setting and sending */
  g_signal_emit_by_name (webrtc,
      "set-local-description", offer, NULL);

  /* Implement this and send offer to peer using signalling */
  send_sdp_offer (offer);
  gst_webrtc_session_description_free (offer);
}
```

* 类似的，当我们从远端接收到 SDP answer 时，我们必须调用  webrtcbin 的 `set-remote-description` 方法.

```
answer = gst_webrtc_session_description_new (
    GST_WEBRTC_SDP_TYPE_ANSWER, sdp);
g_assert (answer);

/* Set remote description on our pipeline */
g_signal_emit_by_name (webrtc, "set-remote-description",  answer, NULL);
```

ICE 的处理也是类似的，当"on-ice-candidate" 信号触发时，我们会获得一个 local ICE candidate, 并需要发送到远端，而当我们从远端收到一个 ICE candidate 时，我们需要调用 webrtcbin 的 "add-ice-candidate" 方法。

现在拼图只剩下一块了，那就是处理从远端收到的媒体流。为此，我们要将 `on_incoming_stream() ` 附加到 webrtcbin 有 "pad-added" 信号。

```
static void
on_incoming_stream (GstElement * webrtc, GstPad * pad,
    GstElement * pipe)
{
  GstElement *play;

  play = gst_parse_bin_from_description (
      "queue ! vp8dec ! videoconvert ! autovideosink",
      TRUE, NULL);
  gst_bin_add (GST_BIN (pipe), play);

  /* Start displaying video */
  gst_element_sync_state_with_parent (play);
  gst_element_link (webrtc, play);
}
```

这就是基本的 webrtc 工作流程。 那些以前使用过 PeerConnection API 的人会很高兴看到这与调用 Web JS API 的流程非常接近。

1.  SDP 协商

![sdp negotiation](https://upload-images.jianshu.io/upload_images/1598924-cc2325953073bd82.png?imageMogr2/auto-orient/strip|imageView2/2/w/207/format/webp)

2.  ICE 检查

![ice candidate exchange](https://upload-images.jianshu.io/upload_images/1598924-3390ea3ce1951b13.png?imageMogr2/auto-orient/strip|imageView2/2/w/315/format/webp)


## 快速测试

参考 https://github.com/centricular/gstwebrtc-demos 的示例 sendrecv: 发送和接收音视频
* 复制示例代码 
```
git clone https://github.com/centricular/gstwebrtc-demos.git
```
*  将 `js/` 目录放到网站的根目录下，或者打开 [https://webrtc.nirbheek.in](https://webrtc.nirbheek.in/)

*  此段 JS 代码假设信令服务在 Web 服务器的端口 8443 侦听
*  打开网站，确保状态为 "Registered with server, waiting for call", 并记下 `id` 
*  编译 C 代码

```bash
$ cd gst
$ gcc webrtc-sendrecv.c $(pkg-config --cflags --libs gstreamer-webrtc-1.0 gstreamer-sdp-1.0 libsoup-2.4 json-glib-1.0) -o webrtc-sendrecv
```

*   运行 `webrtc-sendrecv --peer-id=ID` ，  `id` 即在浏览器上显示的 peerId. 

例如:

1. 打开 https://webrtc.nirbheek.in/, 显示的 peer id 为 5323
2. 运行 ` ./webrtc-sendrecv --peer-id=5323`
3. 在浏览器上可以看到跳动的小球视频

```
> GET  HTTP/1.1
> Soup-Debug-Timestamp: 1692837597
> Soup-Debug: SoupSession 1 (0x557842e85100), SoupMessage 1 (0x5578431cf8e0), SoupSocket 1 (0x557842d51c40)
> Host: webrtc.nirbheek.in:8443
> Upgrade: websocket
> Connection: Upgrade
> Sec-WebSocket-Key: e/WoYP6Gr85M4iqaZYTd3g==
> Sec-WebSocket-Version: 13
> Accept-Encoding: gzip, deflate

< HTTP/1.1 101 Switching Protocols
< Soup-Debug-Timestamp: 1692837597
< Soup-Debug: SoupMessage 1 (0x5578431cf8e0)
< Upgrade: websocket
< Connection: Upgrade
< Sec-WebSocket-Accept: jnGmBhPRHb9szcxQnGoSZAe5WMc=
< Date: Thu, 24 Aug 2023 00:39:08 GMT
< Server: Python/3.11 websockets/11.0.2

Connected to signalling server
Registering id 8634 with server
Registered with server
Setting up signalling server call with 5323
Created data channel
Starting pipeline
Sending offer:
v=0
o=- 4050401179331307259 0 IN IP4 0.0.0.0
s=-
t=0 0
a=ice-options:trickle
a=group:BUNDLE video0 audio1 application2
m=video 9 UDP/TLS/RTP/SAVPF 96
c=IN IP4 0.0.0.0
a=setup:actpass
a=ice-ufrag:SSUHGjApzvSrB6fACYzhFG6WFQpL4qrh
a=ice-pwd:scHMe5CJ57NpqnHp70OzrGSmAI1kJoCB
a=rtcp-mux
a=rtcp-rsize
a=sendrecv
a=rtpmap:96 VP8/90000
a=rtcp-fb:96 nack pli
a=rtcp-fb:96 transport-cc
a=framerate:30
a=ssrc:199344106 msid:user3365093335@host-d6d503be webrtctransceiver0
a=ssrc:199344106 cname:user3365093335@host-d6d503be
a=mid:video0
a=fingerprint:sha-256 3B:D3:71:E5:B4:AA:A1:A3:D9:6A:22:8E:D3:49:27:6F:55:83:AC:A7:A2:E2:6B:88:3C:53:F7:E0:1D:8D:A7:B0
a=rtcp-mux-only
m=audio 0 UDP/TLS/RTP/SAVPF 97
c=IN IP4 0.0.0.0
a=setup:actpass
a=ice-ufrag:SSUHGjApzvSrB6fACYzhFG6WFQpL4qrh
a=ice-pwd:scHMe5CJ57NpqnHp70OzrGSmAI1kJoCB
a=bundle-only
a=rtcp-mux
a=rtcp-rsize
a=sendrecv
a=rtpmap:97 OPUS/48000/2
a=rtcp-fb:97 transport-cc
a=fmtp:97 sprop-stereo=0;sprop-maxcapturerate=48000
a=ssrc:1019736707 msid:user3365093335@host-d6d503be webrtctransceiver1
a=ssrc:1019736707 cname:user3365093335@host-d6d503be
a=mid:audio1
a=fingerprint:sha-256 3B:D3:71:E5:B4:AA:A1:A3:D9:6A:22:8E:D3:49:27:6F:55:83:AC:A7:A2:E2:6B:88:3C:53:F7:E0:1D:8D:A7:B0
a=rtcp-mux-only
m=application 0 UDP/DTLS/SCTP webrtc-datachannel
c=IN IP4 0.0.0.0
a=setup:actpass
a=ice-ufrag:SSUHGjApzvSrB6fACYzhFG6WFQpL4qrh
a=ice-pwd:scHMe5CJ57NpqnHp70OzrGSmAI1kJoCB
a=bundle-only
a=mid:application2
a=sctp-port:5000
a=fingerprint:sha-256 3B:D3:71:E5:B4:AA:A1:A3:D9:6A:22:8E:D3:49:27:6F:55:83:AC:A7:A2:E2:6B:88:3C:53:F7:E0:1D:8D:A7:B0

ICE gathering state changed to gathering
Received answer:
v=0
o=- 5928460106182311303 2 IN IP4 127.0.0.1
s=-
t=0 0
a=group:BUNDLE video0 audio1 application2
a=msid-semantic: WMS 80f0e9a6-a2ba-434e-94f0-4d8d0d0e18fe
m=video 9 UDP/TLS/RTP/SAVPF 96
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:aV9x
a=ice-pwd:1hBp2fLGi7bW+QZvEs9C58Du
a=ice-options:trickle
a=fingerprint:sha-256 59:8C:C4:46:92:99:D6:7B:71:D9:7C:AD:CB:8C:5D:50:96:B6:70:37:CC:F9:EF:25:FF:34:F2:6E:34:52:8F:CA
a=setup:active
a=mid:video0
a=sendrecv
a=rtcp-mux
a=rtcp-rsize
a=rtpmap:96 VP8/90000
a=rtcp-fb:96 transport-cc
a=rtcp-fb:96 nack pli
a=ssrc:1621313799 cname:C+l5r9FJua3urdCO
a=ssrc:1621313799 msid:80f0e9a6-a2ba-434e-94f0-4d8d0d0e18fe 19830fc8-a2d5-4d54-8aa7-cfe22b1720d3
m=audio 9 UDP/TLS/RTP/SAVPF 97
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:aV9x
a=ice-pwd:1hBp2fLGi7bW+QZvEs9C58Du
a=ice-options:trickle
a=fingerprint:sha-256 59:8C:C4:46:92:99:D6:7B:71:D9:7C:AD:CB:8C:5D:50:96:B6:70:37:CC:F9:EF:25:FF:34:F2:6E:34:52:8F:CA
a=setup:active
a=mid:audio1
a=sendrecv
a=rtcp-mux
a=rtpmap:97 OPUS/48000/2
a=rtcp-fb:97 transport-cc
a=fmtp:97 minptime=10;useinbandfec=1
a=ssrc:1489599020 cname:C+l5r9FJua3urdCO
a=ssrc:1489599020 msid:80f0e9a6-a2ba-434e-94f0-4d8d0d0e18fe c64ee897-790a-497c-8edd-06d08d1691c3
m=application 9 UDP/DTLS/SCTP webrtc-datachannel
c=IN IP4 0.0.0.0
a=ice-ufrag:aV9x
a=ice-pwd:1hBp2fLGi7bW+QZvEs9C58Du
a=ice-options:trickle
a=fingerprint:sha-256 59:8C:C4:46:92:99:D6:7B:71:D9:7C:AD:CB:8C:5D:50:96:B6:70:37:CC:F9:EF:25:FF:34:F2:6E:34:52:8F:CA
a=setup:active
a=mid:application2
a=sctp-port:5000

data channel opened
data channel opened
Received data channel message: Hi! (from browser)
```

## 参考资料
* WebRTC 插件介绍 https://blog.nirbheek.in/2018/02/gstreamer-webrtc.html
* WebRTC 插件源码 
  - [gst v1.8 webrtcbin plugin](https://gitlab.freedesktop.org/gstreamer/gst-plugins-bad/-/tree/1.18/ext/webrtc?ref_type=heads)
  - [gst newest webrtcbin plugin](https://gitlab.freedesktop.org/gstreamer/gstreamer/-/tree/main/subprojects/gst-plugins-bad/gst-libs/gst/webrtc?ref_type=heads)
* [bidirectional audio-video demos](https://gitlab.freedesktop.org/gstreamer/gst-examples/-/tree/1.18/webrtc).

* [webrtcbin demo](https://gitlab.freedesktop.org/gstreamer/gst-examples/-/tree/1.18/webrtc)
* [webrtcbin example](https://gitlab.freedesktop.org/gstreamer/gst-plugins-bad/-/tree/1.18/tests/examples/webrtc)

* https://gstreamer.freedesktop.org/documentation/webrtc/index.html?gi-language=c
* https://blogs.igalia.com/llepage/webrtc-gstreamer-and-html5-part-1
* https://blogs.igalia.com/llepage/webrtc-gstreamer-and-html5-part-2
* https://gitlab.freedesktop.org/gstreamer/gst-plugins-rs/-/tree/main/net/webrtc
* https://developer.ridgerun.com/wiki/index.php/GstWebRTC_-_Vp8-Opus_Examples 

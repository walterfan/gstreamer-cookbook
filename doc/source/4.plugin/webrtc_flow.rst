######################
webrtcbin flow
######################

.. include:: ../links.ref
.. include:: ../tags.ref
.. include:: ../abbrs.ref

============ ==========================
**Abstract** webrtcbin flow
**Category** Learning note
**Authors**  Walter Fan
**Status**   WIP
**Updated**  |date|
============ ==========================

.. contents::
   :local:


Test Flow
=======================================

1. connect_to_websocket_server_async

以 websocket 异步连接 signal server,  一旦连接好，会回调 on_server_connected 方法
在 on_server_connected 方法注册 on_server_closed 和 on_server_message 两个回调函数

2. register_with_server

连接上 signal server 后，client生成一个 10000 以内的随机数为 clentId, 发送 "HELLO $clientId" 消息给服务器
在收到 server 回复的 HELLO 消息后，再发送 "SESSION $peerId" 消息给服务器, 这个 peerId 也可称作 sessionId
服务器收到 "SESSION $peerId" 后会回复一个 "SESSION OK" 消息，至此，一个 WebRTC signal session 建立好了


3. start_pipeline

创建 pipeline:

 - webrtcbin bundle-policy=max-bundle name=sendrecv " STUN_SERVER 
 - videotestsrc ! videoconvert ! queue ! vp8enc ! webrtcbin
 - autiotestsrc ! audioconvert ! audioresample ! queue ! webrtcbin

注册如下回调

.. code-block:: 

    g_signal_connect (webrtc1, "on-negotiation-needed", ...)
    g_signal_connect (webrtc1, "on-ice-candidate",   G_CALLBACK (send_ice_candidate_message), NULL);
    g_signal_connect (webrtc1, "notify::ice-gathering-state",
    g_signal_emit_by_name (webrtc1, "create-data-channel", "channel", NULL,  &send_channel)
    g_signal_connect (webrtc1, "on-data-channel", G_CALLBACK (on_data_channel),
    g_signal_connect (webrtc1, "pad-added", G_CALLBACK (on_incoming_stream),  pipe1);


随后，就通过 signal server 以 websocket 来交换 SDP 和 ICE candidate

   - 发起端调用 createOffer 创建一个offer，并调用 setLocalDescription 设置本地的 SDP.
   - 通过signal server,将含有 SDP 的 offer 设置给要进行通讯的另一个peer.
   - 应答端peer拿到发起端的offer后,调用 setRemoteDescription,保存此 SDP 信息.
   - 应答端调用createAnswer 创建一个answer,并调用 setLocalDescription 设置本地的 SDP.
   - 通过signal server将含有应答端peer的SDP信息的 offer answer发送给发起端的peer.
   - 发起端peer调用setRemoteDescription 保存应答端peer的SDP信息.


4. 通过 on_incoming_stream 处理媒体流

   - 对于 Webrtcbin 的事件 pad-added, 注册回调函数 on_media_stream_incoming

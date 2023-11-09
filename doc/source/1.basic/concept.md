# GStreamer 基本概念

## 什么是 Element？

GStreamer应用中最重要的对象是GstElement对象，element是多媒体pipeline基本的构建元素，所有的高级元素都集成自GstElement。

Gstreamer中主要有三种elements：sink element，src element，filter-like element，element的类型由其具备哪些pads决定

元素(element)是GStreamer中最重要的概念。可以通过创建一系列的元素, 并把它们连接起来, 从而让 数据流在这个被连接的各个元素之间传输。 

每个元素都有一个特殊的函数接口,对于有些元素的函数接口它们是用于能够读取文件的数据,译码文件数据的。而有些元素的函数接口只是输出相应的数据到具体的设备上(例如,声卡设备)。

可以将若干个元素连接在一起,从而创建一个管道来完成一个特殊的任务,例如,媒体播放或者录音。GStreamer已经默认安装了很多有用的元素 ,通过使用这些元素(Elements)你能够构建一个具有多种功能的应用程序。

## 什么 bin ？

GstBin可以将一系列elements组合形成一个逻辑上的element，以便从整体上操控和管理elements。也就是说 Bin是一个可以启动的element的集合，包含source、filter和sink。

GstBin管理它内部elements的状态。


## 什么是 pipeline

最外层的bin即使pipeline。管道 pipeline 由元素 (elements) 和链接 (links) 组成。 

Elements 可以放入不同种类的容器 (Bin) 中。 元素elements、链接links和容器Bin可以在管道描述中以任意顺序指定。

## 什么是 Bus
GstBus 是将stream线程消息转发给应用程序线程的系统。

GstBus 本身运行在应用程序的上下文中，但能够自动监听GStreamer内的线程。
每条 pipeline 都自带一条 GstBus，开发人员仅需为其设定handler以便在接收到消息是能或者正确的处理。

## 什么是Pad?

Pad 是一个element与外部交互的接口，数据从一个element的src-pad传递给另一个element的sink-pad。Pad的 Capabilities 表明element能处理的数据。

Pads 在GStreamer中被用于多个元素的链接，从而让数据流能在这样的链接中流动。

一个 Pads 可以被看作是一个元素(element)插座或者端口，元素(element)之间的链接就是依靠着衬垫(Pads)。

Pads 有处理特殊数据的能力：一个 Pads 能够限制数据流类型的通过。

链接成功的条件是：只有在两个衬垫(Pads)允许通过的数据类型一致的时候才被建立。

## 什么是Capabilities？

Capabilities是用于描述一个pad能够处理或正在处理的数据类型的机制。GStreamer使用GstCaps描述pads的capabilities，一个GstCaps将含有一个或多个GStructure来描述媒体类型，但对于已经完成negotiation的pad，其GstCaps的GStructure是唯一的，并且属性值是固定的





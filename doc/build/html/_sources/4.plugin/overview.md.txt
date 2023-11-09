# GStreamer的插件机制

GStreamer 是一个极其强大且多功能的框架，用于创建流媒体应用程序。 GStreamer 框架的许多优点都来自于其模块化：GStreamer 可以无缝合并新的插件模块。 但由于模块化和强大功能通常会以更高的复杂性为代价，因此编写新应用程序并不总是那么容易。


## Elements and Plugins

元素是 GStreamer 的核心。 在插件开发的上下文中，元素是从 GstElement 类派生的对象。 元素与其他元素链接时提供某种功能：例如，源元素向流提供数据，过滤器元素作用于流中的数据。 如果没有元素，GStreamer 只是一堆概念性的管道配件，没有任何链接。 GStreamer 附带了大量元素，但也可以写入额外的元素。

然而，仅编写一个新元素还不够：您需要将元素封装在插件中以使 GStreamer 能够使用它。 插件本质上是一个可加载的代码块，通常称为共享对象文件或动态链接库。 单个插件可能包含多个元素的实现，或者仅包含一个元素。 为简单起见，本指南主要关注包含一个元素的插件。

过滤器是处理数据流的一种重要元素。 数据的生产者和消费者分别称为源元素和接收器元素。 Bin 元素包含其他元素。 一种类型的 bin 负责同步它们所包含的元素，以便数据顺利流动。 另一种类型的 bin 称为 autoplugger 元素，它会自动将其他元素添加到 bin 并将它们链接在一起，以便它们充当两个任意流类型之间的过滤器。

即使只使用标准包，GStreamer 中到处都使用插件机制。 一些非常基本的功能驻留在核心库中，所有其他功能都在插件中实现。 插件注册表用于将插件的详细信息存储在二进制注册表文件中。 这样，使用 GStreamer 的程序不必加载所有插件来确定需要哪些插件。 仅当请求插件提供的元素时才会加载插件。

## Pads

Pad 用于协商 GStreamer 中元素之间的链接和数据流。 pad 可以被视为元素上的“位置”或“端口”，可以在其中与其他元素建立链接，并且数据可以通过它流入或流出这些元素。 Pad 具有特定的数据处理功能：Pad 可以限制流经它的数据类型。 仅当两个焊盘允许的数据类型兼容时，才允许两个焊盘之间存在链接。

在这里做一个类比可能会有所帮助。Pad 类似于物理设备上的插头或插孔。 例如，考虑一个由放大器、DVD 播放器和（静音）视频投影仪组成的家庭影院系统。 允许将 DVD 播放器连接到放大器，因为这两个设备都有音频插孔，并且允许将投影仪连接到 DVD 播放器，因为这两个设备都有兼容的视频插孔。 由于投影仪和放大器具有不同类型的插孔，因此投影仪和放大器之间可能无法建立链接。 GStreamer 中的 Pad 与家庭影院系统中的插孔具有相同的用途。

大多数情况下，GStreamer 中的所有数据都通过元素之间的链接以一种方式流动。 数据通过一个或多个source pad 流出一个 element，并且 element 通过一个或多个 sink pad 接受传入数据。 source element 只有 src pad, sink element 仅具有 sink pad。

## GstMiniObject, Buffers and Events

GStreamer 中的所有数据流都被分成块，这些块从一个元素上的source pad传递到另一元素上的sink pad。 GstMiniObject 是用于保存这些数据块的结构。

GstMiniObject 包含以下重要类型：

* 指示此 GstMiniObject 的数据类型（事件、缓冲区等）的确切类型。
* 引用计数指示当前持有对微型对象的引用的元素的数量。 当引用计数降至零时，GstMiniObject将被释放，并且其内存将在某种意义上被释放（更多详细信息请参见下文）。

对于数据传输，定义了两种类型的 GstMiniObject：event（控制）和 buffer（内容）。

缓冲区可以包含任何类型的数据, 两个连接在一起的 pads 知道如何处理这些数据。 通常，缓冲区包含从一个元素流向另一个元素的某种音频或视频数据块。

缓冲区还包含描述缓冲区内容的元数据 metadata。 一些重要的元数据类型是：

* 指向一个或多个 GstMemory 对象的指针。 GstMemory 对象是封装内存区域的重新计数对象。

指示缓冲区中内容的首选显示时间戳的时间戳。


## How to write a plugin

1. 构建样板代码
Constructing the Boilerplate

2. 指定 pads
Specifying the pads

3. 实现 chain 函数
The chain function

4. 实现 event 函数
The event function

5. 实现 query 函数
The query function

6. 实现 state 处理
What are states?

7. 增加属性
Adding Properties

8. 处理信号
Signals

9. 构建一个测试程序
Building a Test Application
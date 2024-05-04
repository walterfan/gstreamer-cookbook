# Transform elements

转换元素是GStreamer中的一种元素类型，它们根据接收到的媒体流的接收端（sink）和发送端（source）的格式（caps）来转换输入缓冲区（input buffers）到输出缓冲区（output buffers）。

## 转换元件

转换元素根据接收端和发送端的格式（caps）来转换输入缓冲区到输出缓冲区。对于转换元素来说，一个重要的要求是输出格式完全由输入格式决定，反之亦然。这意味着，例如，典型的解码器元素不能使用转换元素来实现，因为输出格式（如解压视频帧的宽度和高度）是编码在流中的，不是由输入格式定义的。

典型的转换元素包括：
- 音频转换器（audioconvert, audioresample等）
- 视频转换器（colorspace, videoscale等）
- 过滤器（capsfilter, volume, colorbalance等）

转换元素的实现需要考虑以下事项：
- 高效的上下游协商
- 高效的缓冲区分配和其他缓冲区管理

一些转换元素可以在不同的模式下操作：
- 直通（passthrough，不对输入缓冲区进行任何更改）
- 原地（in-place，直接对传入的缓冲区进行更改，无需复制或分配新缓冲区）
- 仅更改元数据

根据操作模式的不同，缓冲区分配策略可能会发生变化。转换元素应该能够在任何时候重新协商接收端和发送端的格式，以及更改操作模式。

此外，转换元素通常还需要处理以下事项：
- 刷新、寻找
- 状态变化
- 时间戳处理，通常通过将输入时间戳复制到输出缓冲区，但子类可以覆盖这一点
- 服务质量（QoS），避免调用子类的转换函数
- 处理调度问题，如基于推送和拉取的操作

接下来的章节将描述转换元素在上述用例中的行为。我们主要关注缓冲区分配策略和格式协商。

## 处理 Processing

转换元素有两个主要的处理函数：
- `transform()`：将输入缓冲区转换到输出缓冲区。输出缓冲区保证可写且与输入缓冲区不同。
- `transform_ip()`：原地转换输入缓冲区。输入缓冲区可写且大小至少与输出缓冲区相同。

转换元素可以在以下模式下操作：
- 直通（passthrough）：元素不会对缓冲区进行更改，缓冲区直接通过，两端的格式需要相同。元素可以选择性地实现`transform_ip()`函数来查看数据，缓冲区不需要可写。
- 原地（in-place）：可以直接修改输入缓冲区以获得输出缓冲区。转换必须实现`transform_ip()`函数。
- 复制-转换（copy-transform）：通过复制和转换输入缓冲区到新的输出缓冲区来执行转换。转换必须实现`transform()`函数。

如果没有提供`transform()`函数，只允许原地和直通操作，这意味着源和目标格式必须相同，或者源缓冲区大小至少与目标缓冲区大小相同。
如果没有提供`transform_ip()`函数，只支持直通和复制-转换。提供这个函数可以避免缓冲区复制的优化。
如果没有提供任何函数，我们只能以直通模式处理。

## 协商 Negotiation

在推送模式下，转换元素的典型（重新）协商总是从接收端到发送端进行，这触发以下序列：
- 接收端垫（sinkpad）接收到新的格式事件。
- 转换函数计算出它可以转换这些格式到什么。
- 尝试查看是否可以在对等方上不修改配置格式。我们需要这样做，因为我们倾向于不做任何事情。
- 转换配置自己以从新的接收端格式转换到目标发送端格式。
- 转换处理并设置发送端垫（src pad）上的输出格式。

我们称这个过程为下游协商（Downstream Negotiation, DN），大致如下所示：

```
          sinkpad              transform               srcpad
CAPS event   |                    |                      |
------------>|  find_transform()  |                      |
             |------------------->|                      |
             |                    |       CAPS event     |
             |                    |<---------------------|
             | <configure caps> <-|                      |
```

这些步骤配置了元素，使其能够从输入格式转换到输出格式。

转换有三个函数来执行协商：
- `transform_caps()`：将某个垫上的格式转换为另一个垫上所有可能支持的格式。输入格式保证是一个简单的格式，只有一个结构。格式不必固定。
- `fixate_caps()`：给定一个垫上的格式，固定另一个垫上的格式。目标格式是可写的。
- `set_caps()`：配置转换以在源格式和目标格式之间进行转换。两个格式都保证是固定的格式。

如果没有定义`transform_caps()`，我们只能执行默认的身份转换。
如果没有定义`set_caps()`，我们不关心格式。在这种情况下，我们还假设不会有人写入缓冲区，我们也不强制要求`transform_ip()`函数（如果存在）的缓冲区是可写的。

转换元素需要的一个常见功能是找到从一种格式（源）到另一种格式（目标）的最佳转换。这个函数的一些要求是：
- 有固定的源格式。
- 找到转换元素可以转换到的固定目标格式。
- 目标格式是兼容的，并且可以被对等元素接受。
- 转换函数更倾向于使源格式==目标格式。
- 转换函数可以选择性地固定目标格式。

`find_transform()`函数的工作流程如下：
- 从源应用服务器（src aps）开始，这些格式是固定的。
- 检查格式是否可接受作为我们的源格式。这通常由元素的padtemplate强制执行。
- 使用`transform_caps()`计算我们可以转换的所有格式。
- 如果原始格式是转换格式的子集，尝试查看这些格式是否对等元素可接受。如果是这样，我们可以执行直通并使源==目标。这通过简单地调用`gst_pad_peer_query_accept_caps()`来完成。
- 如果格式不是固定的，我们需要固定它，从对等元素的格式开始并与它们相交。
- 对于使用`transform_caps()`检索到的每个转换格式：
- 尝试使用`fixate_caps()`固定格式。
- 如果格式被固定，使用`_peer_query_accept_caps()`检查对等元素是否接受它们，如果对等元素接受，我们找到了目标格式。
- 如果我们用完了格式，我们找不到转换。
- 如果我们找到了目标格式，使用`set_caps()`配置转换。

在这个协商过程之后，转换元素通常处于稳定状态。我们可以识别这些稳定状态：
- 源和接收端垫都有相同的格式。注意，当两端的格式相同时，输入和输出缓冲区自动具有相同的大小。元素可以以以下方式操作缓冲区：（相同的格式，SC）
  - 直通：缓冲区被检查，但元数据或缓冲区数据未更改。输入缓冲区不需要可写。输入缓冲区只是简单地推出，不做修改。（SCP）
  - 原地：缓冲区原地修改，这意味着输入缓冲区被修改以产生新的输出缓冲区。这需要输入缓冲区可写。如果输入缓冲区不可写，则必须从缓冲池分配新缓冲区。（SCI）
  - 复制转换：从缓冲池分配新的输出缓冲区，并将输入缓冲区中的数据转换到输出缓冲区。（SCC）

- 源和接收端垫有不同的格式。元素可以以以下方式操作缓冲区：（不同的格式，DC）
  - 原地：输入缓冲区原地修改。这意味着输入缓冲区的大小至少与输出大小相同。输入缓冲区将被调整到输出缓冲区的大小。如果输入缓冲区不可写或输出大小大于输入大小，我们需要填充分配一个新的缓冲区。（DCI）
  - 复制转换：分配新的输出缓冲区，并将输入缓冲区中的数据转换到输出缓冲区。流程与相同格式协商的情况完全相同。（DCC）

我们可以立即观察到，复制转换状态将需要从缓冲池分配一个新的缓冲区。当转换元素在原地状态下接收到不可写的缓冲区时，它也需要执行分配。直通状态没有分配的原因。

这个稳定状态在以下操作发生时会发生变化：
- 接收端垫接收到新的格式，这触发了上述下游重新协商过程，具体流程见上文。
- 转换元素想要重新协商（因为属性变化，例如）。这基本上清除了当前的稳定状态，并触发了下游和上游的重新协商过程。当转换元素的源垫接收到RECONFIGURE事件时，也会发生这种情况。

### 分配 Allocation

配置了格式的转换元素后，需要协商缓冲池以执行缓冲区的分配。我们有两种情况：
- 元素处于直通操作，我们不需要在转换元素中分配缓冲区。
- 元素不处于直通操作，需要分配输出缓冲区。

在第一种情况下，我们不查询和配置池。我们让上游决定是否要使用缓冲池，然后我们将从下游代理缓冲池到上游。
在第二种情况下，我们查询并在源垫上设置缓冲池，用于执行分配。

为了执行分配，我们需要能够在转换后获取输出缓冲区的大小。我们需要额外的函数来检索大小。有两个函数：
- `transform_size()`：给定一个垫上的格式和大小，以及另一个垫上的格式，计算另一个缓冲区的大小。这个
函数能够执行所有大小转换，是转换大小的首选方法。
- `get_unit_size()`：当输入大小和输出大小总是彼此的倍数时（音频转换等），我们可以定义一个更简单的`get_unit_size()`函数。转换将使用这个函数在源和目标缓冲区中获取相同数量的单元。出于性能原因，格式和大小之间的映射被缓存。

这些是转换元素在GStreamer中的基本操作和协商机制。通过这些机制，GStreamer能够灵活地处理各种媒体流，并在不同的媒体格式之间进行转换。
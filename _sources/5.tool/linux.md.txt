# Linux


## Device

`/proc/bus/input/devices` 是 Ubuntu 系统中的一个特殊文件，它记录了所有已注册的 input 设备的信息。这个文件是 Linux input 子系统的一部分，input 子系统负责处理各种输入设备，比如鼠标、键盘、触摸屏等。通过查看这个文件，你可以获取到这些设备的详细信息，例如设备名称、物理位置、系统文件系统路径、唯一标识、处理程序以及它们支持的事件类型等 。

文件中的每个设备信息通常包含以下几个字段：
- **I**：设备ID，包括总线类型、供应商、产品和版本信息。
- **N**：设备名称。
- **P**：物理位置。
- **S**：系统文件系统路径。
- **U**：唯一标识。
- **H**：处理程序，例如哪个驱动程序正在处理这个设备。
- **B**：设备属性，包括支持的事件类型和键位等。

例如，如果你连接了一个鼠标和一个键盘，`/proc/bus/input/devices` 文件将显示如下内容：
```
I: Bus=0003 Vendor=093a Product=2510 Version=0111
N: Name="PixArt USB Optical Mouse"
P: Phys=usb-hiusb-xhci.0-1/input0
S: Sysfs=/devices/platform/hiusb-xhci.0/usb3/3-1/3-1:1.0/input/input0
U: Uniq=
H: Handlers=mouse0 event0
B: PROP=0
B: EV=17
B: KEY=ff0000 0 0 0 0 0 0 0 0
B: REL=103
B: MSC=10

I: Bus=0003 Vendor=1c4f Product=0002 Version=0110
N: Name="SIGMACHIP USB Keyboard"
P: Phys=usb-hiusb-ohci-1/input0
S: Sysfs=/devices/platform/hiusb-ohci.0/usb2/2-1/2-1:1.0/input/input1
U: Uniq=
H: Handlers=kbd event1
B: PROP=0
B: EV=120013
B: KEY=10000 7 ff800000 7ff febeffdf f3cfffff ffffffff fffffffe
B: MSC=10
B: LED=7
```
这些信息可以帮助系统管理员或开发者诊断硬件问题，或者了解系统中的输入设备配置 。



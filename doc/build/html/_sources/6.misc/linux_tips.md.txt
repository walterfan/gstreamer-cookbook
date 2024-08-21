# Linux Tips


## Network

在 Ubuntu 系统上，`/etc/network/interfaces` 文件是网络配置文件，用于定义网络接口的配置。这个文件通常用于配置网络接口的 IP 地址、子网掩码、默认网关、DNS 服务器等信息。以下是一些基本用法和示例。

### 基本结构

`/etc/network/interfaces` 文件的基本结构如下：

```bash
# The primary network interface
auto eth0
iface eth0 inet static
    address 192.168.1.100
    netmask 255.255.255.0
    gateway 192.168.1.1
    dns-nameservers 8.8.8.8 8.8.4.4

# The secondary network interface
auto eth1
iface eth1 inet static
    address 192.168.2.100
    netmask 255.255.255.0
    gateway 192.168.2.1
    dns-nameservers 8.8.8.8 8.8.4.4

# Wireless interface
auto wlan0
iface wlan0 inet dhcp
    wireless-essid MyNetwork
    wireless-mode Managed
    wireless-channel 11
```

### 详细解释

1. **auto**: 这行指定了哪些接口在启动时应该被自动激活。如果某个接口不在这里列出，它将不会被自动激活。

2. **iface**: 这行指定了接口的名称和它应该使用的网络配置方法。常见的配置方法有：
   - `inet static`: 静态 IP 地址
   - `inet dhcp`: 动态 IP 地址（通过 DHCP 服务器获取）
   - `inet6 static`: 静态 IPv6 地址
   - `inet6 auto`: 自动获取 IPv6 地址

3. **address**: 指定接口的 IP 地址。

4. **netmask**: 指定网络的子网掩码。

5. **gateway**: 指定默认网关，即默认路由。

6. **dns-nameservers**: 指定 DNS 服务器的地址。

7. **wireless-essid**: 指定无线网络的 SSID（服务集标识）。

8. **wireless-mode**: 指定无线网络的模式，如 Managed（管理模式）或 Ad-Hoc（对等模式）。

9. **wireless-channel**: 指定无线网络的频道。

### 示例

假设你有一个以太网接口 `eth0` 和一个无线接口 `wlan0`，你希望以太网接口使用静态 IP 地址，无线接口使用 DHCP 获取 IP 地址，配置如下：

```bash
# The primary network interface
auto eth0
iface eth0 inet static
    address 192.168.1.100
    netmask 255.255.255.0
    gateway 192.168.1.1
    dns-nameservers 8.8.8.8 8.8.4.4

# Wireless interface
auto wlan0
iface wlan0 inet dhcp
    wireless-essid MyNetwork
    wireless-mode Managed
```

### 注意事项

- 修改 `/etc/network/interfaces` 文件后，通常需要重启网络服务或重启计算机来使更改生效。
- 确保在配置文件中使用正确的语法和参数，否则可能会导致网络连接问题。

通过这些基本的用法和示例，你可以根据自己的需求配置 Ubuntu 系统的网络接口。


在较新的 Ubuntu 版本中，`/etc/network/interfaces.d/` 目录用于存放网络接口的配置文件。这种方式允许系统管理员将每个网络接口的配置分开管理，而不是将所有配置集中在单个 `/etc/network/interfaces` 文件中。


### 配置文件示例

在 `/etc/network/interfaces.d/` 目录下，每个文件通常对应一个网络接口或一组配置。例如，你可以为 `eth0` 接口创建一个名为 `eth0.cfg` 的文件：

```bash
# /etc/network/interfaces.d/eth0.cfg
auto eth0
iface eth0 inet static
    address 192.168.1.100
    netmask 255.255.255.0
    gateway 192.168.1.1
```

对于无线接口 `wlan0`，你可以创建一个名为 `wlan0.cfg` 的文件：

```bash
# /etc/network/interfaces.d/wlan0.cfg
auto wlan0
iface wlan0 inet dhcp
    wireless-essid MyNetwork
    wireless-mode Managed
```

### 注意事项

- 如果 `/etc/network/interfaces.d/` 目录存在，并且其中包含配置文件，`/etc/network/interfaces` 文件将不再被使用，除非它被明确地包含在 `/etc/network/interfaces.d/50-default.cfg` 文件中。
- 确保每个配置文件的权限设置正确，通常应该是 `644`。
- 在使用 `/etc/network/interfaces.d/` 目录时，重启网络服务或计算机将重新加载所有配置文件。

通过使用 `/etc/network/interfaces.d/` 目录，Ubuntu 系统提供了一种更加灵活和模块化的网络配置方法。


## 在系统启动时自动播放视频

```bash

$ mkdir -p $HOME/.config/autostart

$ cat > auto_play.desktop <<EOF
[Desktop Entry]
Type=Application
Exec=/home/walter/gstreamer-cookbook/script/playback.sh
Hidden=false
NoDisplay=false
X-GNOME-Autostart-enabled=true
Name[en_US]=/home/walter/gstreamer-cookbook/script/playback.sh
Name=/home/walter/gstreamer-cookbook/script/playback.sh
Comment[en_US]=
Comment=
EOF

```

## 在系统启动模拟系统按键

```
$ mkdir -p $HOME/.config/autostart
$ cp auto_lock.desktop $HOME/.config/autostart
```


```

[Desktop Entry]
Type=Application
Exec=$HOME/auto_lock.sh
Hidden=false
NoDisplay=false
X-GNOME-Autostart-enabled=true
Name[en_US]=auto_lock
Name=auto_lock
Comment[en_US]=lock screen and active monitor
Comment=
```

* auth_lock.sh

```
#!/bin/bash
DISPLAY=:0 xdotool key super+l
DISPLAY=:0 xdotool click 1

```

## 在网卡启动时添加路由

```sh
$ vim /etc/network/if-up.d/eth0-routes
#!/bin/sh
# /etc/network/if-up.d/eth0-routes

if [ "$IFACE" = "eth0" ]; then
    ip route add 192.168.104.180 via 192.168.104.1
fi
```

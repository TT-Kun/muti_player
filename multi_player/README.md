# UOSplayer

UOSplayer是一个现代的媒体播放器应用程序，专为UOS/Deepin Linux设计。

## 功能特点

- 音频和视频播放支持
- 浅色和深色主题
- 自动跟随系统主题
- 播放列表管理
- 支持多种媒体格式

## 系统要求

- UOS/Deepin Linux系统
- Qt 5.14或更高版本
- 安装必要的多媒体库

## 安装

### 方法1：使用安装脚本

1. 确保您拥有root权限
2. 打开终端，进入UOSplayer目录
3. 运行安装脚本：

```bash
sudo ./install.sh
```

### 方法2：手动安装

手动安装步骤：

1. 编译应用程序（如果尚未编译）：
```bash
qmake
make
```

2. 以root权限复制文件：
```bash
sudo cp spark_player /usr/bin/mutl_player
sudo mkdir -p /usr/share/applications
sudo cp mutl_player/debian/mutl_player.desktop /usr/share/applications/
sudo mkdir -p /usr/share/icons/hicolor/scalable/apps
sudo cp player.svg /usr/share/icons/hicolor/scalable/apps/mutl_player.svg
```

## 卸载

要卸载UOSplayer，请运行卸载脚本：

```bash
sudo ./uninstall.sh
```

## 使用方法

启动UOSplayer：
- 从应用程序菜单中打开UOSplayer
- 或者在终端中运行 `mutl_player`

## 许可证

本项目使用GPL-3.0许可证发布

# 播放器
qt5.15.2版本运行

# 流媒体地址
https://test-streams.mux.dev/x36xhzz/x36xhzz.m3u8
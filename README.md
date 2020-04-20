# gst-tutorials

gstreamer tutorials

## prerequisite

### ubuntu

```bash
sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa\
gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
```

## troubleshooting

### basic-tutorial-8

#### 现象

```bash
gcc basic-tutorial-8.c -o basic-tutorial-8 `pkg-config --cflags --libs gstreamer-1.0 gstreamer-audio-1.0`
```

1. 使用上面命令编译的时候报错`gst/gst.h: No such file or directory`
2. 使用`pkg-config --cflags --libs gstreamer-1.0 gstreamer-audio-1.0`报错`Package gstreamer-audio-1.0 was not found in the pkg-config search path`.

#### 原因

缺少依赖库`gstreamer-audio-1.0`

#### 解决方法

使用`sudo apt-get install libgstreamer-plugins-base1.0-dev`命令安装`libgstreamer-plugins-base1.0-dev`依赖库

# gst-tutorials

gstreamer tutorials

## prerequisite

### ubuntu

```bash
sudo apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa\
gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
```

## tutorials

### rtsp-server

编译需要使用`meson`和`ninja`这两个工具,安装命令分别如下:

- `sudo python3 -m pip install meson`
- `sudo apt-get install ninja-build`

- 因为依赖`gst-rtsp-server`模块所以需要先使用源码编译安装,先去[仓库](https://gitlab.freedesktop.org/gstreamer/gst-rtsp-server.git)将代码克隆到本地。
- `cd gst-rtsp-server`
- 因为版本不兼容所以切换到`1.14.5`tag
  - todo
- 运行`emson build`命令构造编译环境，并使用`build`文件夹存放相关配置
- `cd build && ninja` 在`build`目录运行`ninja`编译项目
- `sudo ninja install`将库安装到系统
  - 注意这里默认将库安装到`/usr/local/lib/x86_64-linux-gnu/`路径下而非`/lib`或`/usr/lib`
  - 这时候直接引用会报错找不到该库，这是因为程序按照默认共享库路径找不到该共享库文件
  - 需要执行一下`ldconfig`命令
    - `ldconfig`命令的用途主要是在默认搜寻目录(`/lib`,`/usr/lib`)以及动态库配置文件`/etc/ld.so.conf`内所列的目录下，搜索出可共享的动态链接库(lib*.so.*)
    - 进而创建出动态装入程序(ld.so)所需要的连接和缓存文件。缓存文件默认为`/etc/ld.so.cache`，此文件保存已经排序好的动态链接库名字列表。
    - 如果共享库安装到了`/usr/local/bin`或其他非(`lib`,`/usr/lib`)目录下，那么在执行`ldconfig`命令前，还要把新共享库目录加入到共享库配置文件`/etc/ld.so.conf`中:`echo "/usr/local/lib" >> /etc/ld.so.conf`
  - 方法二：是将库路径配置到`LD_LIBRARY_PATH`环境变量中去，这只是权宜之计
  - [参考](https://www.cnblogs.com/zealousness/p/10643004.html)

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

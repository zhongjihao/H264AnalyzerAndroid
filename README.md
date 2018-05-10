本项目是基于开源工程h264bitstream在Android平台上开发的H.264分析器，可以简单分析本机上的h264碼流文件

软件运行后，首先打开一个码流文件。然后单击“开始”，可以解析出一系列NAL，在左侧的列表中显示出来。列表中显示出了每一个NAL的nal_reference_idc、nal_type等信息，
不同种类的NAL被标记成了不同的颜色。单击任意一个NAL，可以在右边显示出其详细信息

编译 \
   进入jni目录 \
   执行 ndk-build

运行 \
   直接用AS运行即可

软件运行界面如下

 ![dsaf](Selection_003.png);
 
点击打开按钮，在文件浏览器上选择要分析的h264碼流文件,选择的文件路径会在软件界面编辑框上显示出来，界面如下
![asfd](Selection_006.png);

点击开始按钮，此时会展示h264碼流文件中所有的nalu信息，界面如下
![fasd](Selection_007.png);

单击每个nalu条目，右边会列出该nalu的详细信息，界面如下

![fasd](Selection_008.png);



附加上一个h264碼流文件cuc_ieschool.h264，具体在app/src/main/jni/h264analyzer/h264bitstream/下，测试时将它拷贝到手机上即可


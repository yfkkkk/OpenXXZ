# OpenXXZ
小信子语义理解API例程 for widora
# 小信子语义理解api例程(已支持语音输入, 还没把播放的源码移过来)

功能：人机对话，智能助手。
### 2. 源码编译：
  
  ```
  本工程使用Cmake构建。
      推荐使用Clion集成编译环境，git clone并直接打开源码根目录即可编译。
  ```
  > CMakeLists.txt配置，此处根据自己的工具链位置修改：
  ![image](https://github.com/yfkkkk/OpenXXZ/raw/master/snapshot/工具链.png)
  
  > CMakeLists.txt配置，各库文件和头文件，请根据自己的openwrt源码路径修改：
  ![image](https://github.com/yfkkkk/OpenXXZ/raw/master/snapshot/路径.png)
  
  > 没有错误，进度到100%即编译完成：
  ![image](https://github.com/yfkkkk/OpenXXZ/raw/master/snapshot/build.png)
  
### 3. 运行：
  
  > ①会看到
  ![image](https://github.com/yfkkkk/OpenXXZ/raw/master/snapshot/等待.png)
  
  > ②然后看到登陆讯飞，即可开始录音
  ![image](https://github.com/yfkkkk/OpenXXZ/raw/master/snapshot/登录讯飞.png)
  
  > ③录音时会自动判断是否有效语音，最后显示回答语句。
  ![image](https://github.com/yfkkkk/OpenXXZ/raw/master/snapshot/回复.png)
  
### 4. 源码目录：
  - OpenXXZ
      - App
          - OpenXXZ //编译完成的二进制文件
      - CJSON
          - cJSON.c //json解析
          - cJSON.h
      - Debug
          - Debug.h //调试用printf函数别名定义
      - Ifly //讯飞头文件目录
          - msp_cmn.h
          - msp_errors.h
          - msp_types.h
          - qisr.h
          - qtts.h
      - Libs
          - mips //讯飞mips库
          - x64 //讯飞x64库
          - x86 //讯飞x86库
      - NetUtil
          - HttpRequest.c //http请求
          - HttpRequest.h
      - OpenXXZ
          - XXZGlobal.h
          - XXZMain.c //widora小信子主函数
      - SoundCardDriver //声卡驱动
      - SoundUtils
          - Recognize.c //语音识别
          - Recorder.c //录音
          - SoundUtils.h //声音结构定义
      - Utils
          - TimeUtils.c //时间显示工具
      - CMakeLists.txt //Cmake配置文件
    1, 编译好的程序在 App目录
    
    2, 运行-》通过麦克风输入语音
  
注意: 

    1, 把Libs/mips中讯飞库放到widora  /lib目录
    
    2, 如果要支持同时播放录音, 把SoundCardDriver下内核模块放到widora   /lib/modules/3.18.29/目录
  
  官网：www.xiaoxinzi.com

## 许可证 GPL

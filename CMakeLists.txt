cmake_minimum_required(VERSION 3.5)
project(test)
add_definitions(-DHAVE_CONFIG_H)

SET(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")

SET(STAGING_DIR )

####################################################
## mt7688使用
#SET(CMAKE_C_COMPILER "/home/yf/openwrt_widora/toolchain/bin/mipsel-openwrt-linux-uclibc-gcc")
#include_directories( "./Ifly" )
#include_directories( "~/openwrt_widora/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7688/linux-3.18.29/include" )
##include_directories( "~/openwrt_widora/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/libmad-0.15.1b" )
##include_directories( "~/openwrt_widora/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/libid3tag-0.15.1b" )
#include_directories( "~/openwrt_widora/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/alsa-lib-1.0.28/include" )
#
## 动态链接库
#link_directories( "~/openwrt_widora/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/lib/" )
#link_directories( "~/openwrt_widora/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/lib/" )
##link_libraries( mad )
#link_libraries( z )
#link_libraries( rt )
#link_libraries( dl )
##link_libraries( id3tag )
#link_libraries( m )
#link_libraries( asound )
#link_libraries( pthread )
#
## 讯飞库
#link_directories( "./Libs/mips" )
#link_libraries( msc )

##############################################################
#ubuntu本地使用
# 包含路径
include_directories( "./Ifly" )
include_directories( "~/openwrt_widora/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/alsa-lib-1.0.28/include" )
# 动态链接库
link_libraries( z )
link_libraries( rt )
link_libraries( dl )
link_libraries( m )
link_libraries( asound )
link_libraries( pthread )
# 讯飞库
link_directories( "./Libs/x64" )
link_libraries( msc )

## 开源小信子
set(OpenXXZ_SOURCE_FILES
        OpenXXZ/XXZMain.c OpenXXZ/XXZGlobal.h
        NetUtil/HttpRequest.c
        CJSON/cJSON.c
        SoundUtils/Recorder.c SoundUtils/SoundUtils.h Debug/Debug.h Utils/TimeUtils.c SoundUtils/Recognize.c)
add_executable(OpenXXZAPP ${OpenXXZ_SOURCE_FILES})

### 测试
#set(TCPTest_SOURCE_FILES
#        OpenXXZ/tuobao_tcpclient.c OpenXXZ/tuobao_tcpclient.h)
#add_executable(TCPTest ${TCPTest_SOURCE_FILES})
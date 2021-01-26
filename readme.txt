telegram 机器人消息功能

在日常维护管理中需要用到telegram机器人消息发送功能，

应用场景：
1. 异常报警通知
2. 数据信息发送到需求账户

功能实现依赖：
   1.telegram机器人账户
   2.需要借助第三方库()
   
  telegram机器人：
  参考文档：https://blog.csdn.net/hc13097240190/article/details/80745446
  
 第三方库：  
  linux C/C++要实现https访问借助于libcurl工具，如果是实现自定义SSL证书通信校验的，可以通过C调用openssl来实现，也可以通过libcurl来。记得linux上提前安装好openssl库。

1.编译安装libcurl

curl库的代码下载地址：https://curl.haxx.se/download.html

第一步：进入curl工程目录执行./buidconf产生configure配置文件；
第二步：执行产生的configure脚本： ./configure
第三步：make
第四步：sudo make install

2.到/usr/local/lib/即可查看到安装好的库文件
<s /usr/local/lib250;48;5                          
libcurl.a   libcurl.so    libcurl.so.4.5.0  python2.7  python3.6
libcurl.la  libcurl.so.4  pkgconfig         python3.5


监测程序的运行状态，并对进程产生的日志修改进行监测，如果在正常时间段修改则通知，如果非正常时间段修改则报警

1. 程序会在0点05分更新日志Allspread.log，此程序在0点10分前如果未监测到日志改变则发送告警消息到telegram
2. 在非更新时间段(非0点0分 - 0点10分)更新日志发送一条通知消息到telegram
3. 如果被监测的进程不存在，发送告警到telegram
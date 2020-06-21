# 网络编程大作业——基于多播的聊天程序

 由于本项目中使用了第三方库cryptopp实现非对称加密，需要在qt的mingw对应的目录下加入头文件和dll文件(相关文件在目录“源代码/cryptopp”中)。具体存放方式：

1. “源代码/cryptopp”下所有的头文件放在“mingw/include”目录中

2. “源代码/cryptopp/debug”和“源代码/cryptopp/release”下的两个dll文件放在“mingw/bin”目录中

3. “源代码/cryptopp/debug”和“源代码/cryptopp/release”下的两个.a文件放在“mingw/lib”目录中

完成后即可在qt creator中编译运行

-----------

运行注意事项：

1. 需要先运行udpmanager管理端，再运行udp用户端，否则用户端会因为无法获取唯一标识和加密锁而无法启动
2. 需要修改在udp目录下的mainwindow.cpp，将managerAddr修改为udpmanager所在的主机的IP地址
3. 代码中的组播地址为硬编码，为“224.0.0.2”，端口为9999；udp用户端监听端口为9998，udpmanager管理端监听端口为9997，若与已有软件冲突，需要同时在udp用户端和udpmanager管理端进行修改


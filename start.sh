#!/bin/bash
# 1-- 生成测试数据程序
# 参数：运行的bin+要读取文件路径+经处理后文件的存放路径+log存放路径+生成新文件的时间间隔
./Create_TestData /home/xiaofeng/Item/ini/stcode.ini /home/xiaofeng/Item/tmp /home/xiaofeng/Item/log/Createdata.log 60 &

# 2-- 通过ftp将远程服务器上的文件下载到本地
# 参数：<logfile>/home/xiaofeng/Item/log/FtpGet.log</logfile> 本程序运行的日志文件。
#    <Server_ip>192.168.198.129</Server_ip> 远程服务器的IP。
#    <Server_post>21</Server_post> 远程服务器的端口。
#    <mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被模式。
#    <username>xiaofeng</username> 远程服务器FTP的用户名。
#    <password>asdf1234</password> 远程服务器FTP的密码。
#    <Localpath>/home/xiaofeng/Item/local_tmp</Localpath> 本地文件存放的目录。
#    <Serverpath>/home/xiaofeng/Item/tmp</Serverpath> 远程服务器存放文件的目录。
#    <matchname>*.GIF</matchname> 待采集文件匹配的文件名，采用大写匹配，不匹配的文件不会被采集，本字段尽可能设置精确，不允许用*匹配全部的文件。
#    <type>1</type> 文件采集成功后，远程服务器文件的处理方式：3-什么也不做；2-删除；1-备份，如果为1，还要指定备份的目录。
#    <Serverbakpath>/home/xiaofeng/Item/baktmp</Serverbakpath> 文件采集成功后，服务器文件的备份目录，此参数只有当ptype=3时才有效。
#    <listfilename>/home/xiaofeng/Item/Data_swtiching_center/listfilename.txt</listfilename> 采集前列出服务器文件名的文件。
#    <okfilename></home/xiaofeng/Item/Data_swtiching_center/okfilename.txt/okfilename> 已采集成功文件名清单，此参数只有当ptype=1时有效。
#    <timetvl>30</timetvl> 采集时间间隔，单位：秒，建议大于10。
./Ftp_get "<Server_ip>192.168.198.129</Server_ip><timetvl>20</timetvl><matchname>*.txt,*log</matchname><type>3</type><Serverbakpath>/home/xiaofeng/Item/baktmp</Serverbakpath><logfile>/home/xiaofeng/Item/log/FtpGet.log</logfile><okfilename>/home/xiaofeng/Item/Data_swtiching_center/okfilename.txt</okfilename><listfilename>/home/xiaofeng/Item/Data_swtiching_center/listfilename.txt</listfilename><Server_post>21</Server_post><username>xiaofeng</username><password>asdf1234</password><mode>1</mode><Localpath>/home/xiaofeng/Item/local_tmp</Localpath><Serverpath>/home/xiaofeng/Item/tmp</Serverpath>" &

# 3-- 数据入库程序
# 参数：运行的bin+存放日志的路径+读取目录路径+读取间隔+sql账号+sql字符集
./DataIntoOracle /home/xiaofeng/Item/log/DataIntoDb.log /home/xiaofeng/Item/local_tmp 10 test/test123@snorcl11g_192 AMERICAN_AMERICA.AL32UTF8 &

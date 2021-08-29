#include"/home/xiaofeng/Item/Code_frame/htidc1127/htidc/c/_public.h"
#include "ftp.h"

Cftp ftp;
CFile File;
CLogFile Logfile;

struct arg
{
    char Server_ip[51];
    int Server_post;
    char username[31];
    char password[31];
    unsigned int mode;
    char Localpath[301];
    char Serverpath[301];
    char Serverbakpath[301];
    char listfilename[301];
    char okfilename[301];
    char logfile[301];
    char matchname[31];
    int type;
    int timetvl;
}st_arg;

struct listfile
{
    char fileName[301];
    char fileTime[31];
};

vector<struct listfile> vlistfilename;
vector<struct listfile> vnewlistfilename;
vector<struct listfile> vokfilename;



void EXIT(int sig);
bool Compare();
bool LoadToVokfilename(char *okfilename);
bool LoadToVlistfilename(char *listfilename);
bool GetFileType();
void _help();
bool analysisXML(char *xmlbuf);

int main(int argc,char *argv[])
{
    
    if(argc!=2){ _help(); return false; } // 判断输入参数，若不正确，则打印帮助信息

    CloseIOAndSignal();     // 关闭全部信号的输入和输出

    signal(SIGINT,EXIT);  signal(SIGTERM,EXIT); // 处理程序退出的信号
  
    analysisXML(argv[1]);  // 解析XML函数
    
    Logfile.Open(st_arg.logfile,"a+");    // 打开用来写入log的文件

    while(true)
    {

        if( ftp.login(st_arg.Server_ip,st_arg.Server_post,st_arg.username,st_arg.password) == false) Logfile.Write("ftp.login false\n"); // ftp 登录

        if( ftp.chdir(st_arg.Serverpath) == false) Logfile.Write("ftp.chdir false\n"); // ftp 进入目标目录

        if( ftp.nlist(".",st_arg.listfilename)==false) Logfile.Write("nlist false\n"); // ftp获取到指定服务器目录下的所有文件，并将文件名写入到文件listfilename

        LoadToVlistfilename(st_arg.listfilename); // 将ftp.nlist获取到的文件名加载到容器vlistfilename里面 

        GetFileType(); // 获取文件方式的功能函数：1-将服务器上已获取的文件转移到备份目录 2-将服务器上已获取的文件删除 3-对已获取的服务器上的文件不做任何操作，并下次获取时不会获取重复文件

        ftp.logout();

        sleep(st_arg.timetvl);

    }
}

bool LoadToVlistfilename(char *listfilename)
{
    struct listfile st_listfile;
    vlistfilename.clear();      // 先清理容器

    if( File.OpenForRead(listfilename,"r") == false) printf("open %s fail\n",listfilename);

    while(true)
    {
      if( File.Fgets(st_listfile.fileName,300,true) == false) break;              // 获取listfilename文件的一行信息
      
      if (MatchFileName(st_listfile.fileName,st_arg.matchname)==false) continue;  // 匹配固定类型的文件：判断文件名是否和MatchFileName匹配，如果不匹配，返回失败

      if( ftp.mtime(st_listfile.fileName)==false )                                // 添加文件修改时间参数：将ftp.mtime获取到的文件时间复制到结构体st_listfile.fileTime里面
      {
          Logfile.Write("ftp.mtime false\n"); return false;
      }
      strcpy(st_listfile.fileTime,ftp.m_mtime); 

      vlistfilename.push_back(st_listfile);                                       // 将带来文件名和文件修改时间的结构体存到容器里
    }

    return true;

}

bool LoadToVokfilename(char *okfilename)
{
    struct listfile st_oklistfile;
    char okbuf[301];
    vokfilename.clear();

    if( File.OpenForRead(okfilename,"r") == false) { Logfile.Write("%s open %s fail\n",__func__,okfilename); return false;}

    while(true)  // 因为okfilename在写入的时候就包含了文件名和文件修改时间，所以下面获取的时候也要分开获取
    {
      if( File.Fgets(okbuf,300,true) == false) break;

      GetXMLBuffer(okbuf,"fileName",st_oklistfile.fileName,301); // 获取文件名
      GetXMLBuffer(okbuf,"fileTime",st_oklistfile.fileTime,20);  // 获取文件修改时间
      vokfilename.push_back(st_oklistfile);                      // 将带有文件名和文件修改时间的结构体存到容器vokfilename里面
    }

    return true;

}

bool Compare()
{
    vnewlistfilename.clear();
    unsigned long jj;

    for(unsigned long ii=0; ii<vlistfilename.size(); ii++)  // 对比vlistfilename和vokfilename，将新增加的文件名添加到容器vnewlistfilename里面
    {
        if(vokfilename.size()==0)
                vnewlistfilename.push_back(vlistfilename[ii]);
        else
        {
            for(jj=0; jj<vokfilename.size(); jj++)
            {
                if(  (strcmp(vlistfilename[ii].fileName,vokfilename[jj].fileName)==0) &&
                     (strcmp(vlistfilename[ii].fileTime,vokfilename[jj].fileTime)==0) ) // 如果在vokfilename里面没有和vlistfilename[ii]一样的文件名和修改时间，则说明vlistfilename[ii]不是新的文件名
                {

                    break;

                }
            }

            if(jj==vokfilename.size()) 
            {
                vnewlistfilename.push_back(vlistfilename[ii]); // 如果vokfilename里面没有和vlistfilename[ii]一样的文件名，说明是新增的文件名
            }
        }
    }
}

bool GetFileType()
{

    if(st_arg.type==3)                         // 获取服务器上的文件但不对已获取的文件进行任何操作，并且在下次获取的时候不获取已获取的文件
    {
        LoadToVokfilename(st_arg.okfilename);  // 将okfilename (包含从服务器上已获取的文件名)里面的文件名加载到容器vokfilename里面
        Compare();                             // 对比vokfilename和vlistfilename并将新增加的文件名加载到vnewlistfilename里面
        vlistfilename.clear();                 // 清空容器
        vlistfilename.swap(vnewlistfilename);  // 将vnewlistfilename复制到vlistfilename里面
    }

    for(unsigned long ii=0;ii<vlistfilename.size();ii++)           // 通过ftp.get获取vlistfilename里文件名对应的文件
    {
        char strServerfilename[301],strLocalfilename[301];
        SNPRINTF(strLocalfilename,300,"%s/%s",st_arg.Localpath,vlistfilename[ii].fileName);   // 本地目录
        SNPRINTF(strServerfilename,300,"%s/%s",st_arg.Serverpath,vlistfilename[ii].fileName); // 服务器目录
            
        if(st_arg.type==1)                     // 获取服务器上的文件并将已获取的文件移动到备份目录
        {
            char strServerfilenamebak[301];
            SNPRINTF(strServerfilenamebak,300,"%s/%s",st_arg.Serverbakpath,vlistfilename[ii].fileName);
            Logfile.Write("get listfile.....%s ",vlistfilename[ii].fileName);
            if(ftp.get(strServerfilename,strLocalfilename)==true)  // 获取新增加的文件
            {
                Logfile.WriteEx("ok\n");
                if(ftp.ftprename(strServerfilename,strServerfilenamebak)==false) { Logfile.Write("ftprename %s false",vlistfilename[ii].fileName); }
            }
        }

        else if(st_arg.type==2)                // 获取服务器上的文件并将已获取的文件删除
        {
            Logfile.Write("get listfile.....%s ",vlistfilename[ii].fileName);
            if(ftp.get(strServerfilename,strLocalfilename)==true)  // 获取新增加的文件
            {
                Logfile.WriteEx("ok\n");
                if(ftp.ftpdelete(vlistfilename[ii].fileName)==false) { Logfile.Write("ftpdelete %s false",vlistfilename[ii].fileName); }
            }
        }
        else if(st_arg.type==3)                // 获取服务器上新增的文件，并将已获取到的文件名追加到okfilename里面
        {
            Logfile.Write("get newlistfile......%s ",vlistfilename[ii].fileName);
            if(ftp.get(strServerfilename,strLocalfilename)==true) // 获取新增加的文件
            {
                Logfile.WriteEx("ok\n");
                File.OpenForWrite(st_arg.okfilename,"a+");    
                File.Fprintf("<fileName>%s</fileName><fileTime>%s</fileTime>\n",vlistfilename[ii].fileName,vlistfilename[ii].fileTime);
            }

        }

    }

}

bool analysisXML(char *xmlbuf)
{
    memset(&st_arg,0,sizeof(struct arg));
    GetXMLBuffer(xmlbuf,"Server_ip",st_arg.Server_ip);
    if (strlen(st_arg.Server_ip)==0) { Logfile.Write("Server_ip is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"Server_post",&st_arg.Server_post);
    if(st_arg.Server_post!=21) st_arg.Server_post=21;
    
    GetXMLBuffer(xmlbuf,"username",st_arg.username);
    if (strlen(st_arg.username)==0) { Logfile.Write("username is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"password",st_arg.password);
    if (strlen(st_arg.password)==0) { Logfile.Write("password is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"mode",&st_arg.mode);
    if ((st_arg.mode!=1)&&(st_arg.mode!=2))  st_arg.mode=1;

    GetXMLBuffer(xmlbuf,"Localpath",st_arg.Localpath);
    if (strlen(st_arg.Localpath)==0) { Logfile.Write("Localpath is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"Serverpath",st_arg.Serverpath);
    if (strlen(st_arg.Serverpath)==0) { Logfile.Write("Serverpath is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"Serverbakpath",st_arg.Serverbakpath);
    if ( (strlen(st_arg.Serverbakpath)==0)&&(st_arg.type==1) ) { Logfile.Write("Serverbakpath is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"listfilename",st_arg.listfilename);
    if (strlen(st_arg.listfilename)==0) { Logfile.Write("listfilename is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"okfilename",st_arg.okfilename);
    if (strlen(st_arg.okfilename)==0) { Logfile.Write("okfilename is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"logfile",st_arg.logfile);
    if (strlen(st_arg.logfile)==0) { Logfile.Write("logfile is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"matchname",st_arg.matchname);
    if (strlen(st_arg.matchname)==0) { Logfile.Write("matchname is null.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"type",&st_arg.type);
    if ((st_arg.type!=1)&&(st_arg.type!=2)&&(st_arg.type!=3)) { Logfile.Write("type is error.\n"); return false;  }

    GetXMLBuffer(xmlbuf,"timetvl",&st_arg.timetvl);
    if (strlen(st_arg.matchname)==0) { Logfile.Write("matchname is null.\n"); return false;  }

    return true;

}

void _help()
{
  printf("\n");

  printf("./Ftp_get \"<Server_ip>192.168.198.129</Server_ip><timetvl>20</timetvl><matchname>*.txt,*log</matchname><type>3</type><Serverbakpath>/home/xiaofeng/Item/baktmp</Serverbakpath><logfile>/home/xiaofeng/Item/log/logfile.txt</logfile><okfilename>/home/xiaofeng/Item/Data_swtiching_center/okfilename.txt</okfilename><listfilename>/home/xiaofeng/Item/Data_swtiching_center/listfilename.txt</listfilename><Server_post>21</Server_post><username>xiaofeng</username><password>asdf1234</password><mode>1</mode><Localpath>/home/xiaofeng/Item/local_tmp</Localpath><Serverpath>/home/xiaofeng/Item/tmp</Serverpath>\"\n");

  printf("本程序是数据中心的公共功能模块，用于把远程FTP服务器的文件采集到本地目录。\n");
  printf("xmlbuffer为文件传输的参数，如下：\n");
  printf("<logfile>/home/xiaofeng/Item/log/logfile.txt</logfile> 本程序运行的日志文件。\n");
  printf("<Server_ip>192.168.198.129</Server_ip> 远程服务器的IP。\n");
  printf("<Server_post>21</Server_post> 远程服务器的端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被模式。\n");
  printf("<username>xiaofeng</username> 远程服务器FTP的用户名。\n");
  printf("<password>asdf1234</password> 远程服务器FTP的密码。\n");
  printf("<Localpath>/home/xiaofeng/Item/local_tmp</Localpath> 本地文件存放的目录。\n");
  printf("<Serverpath>/home/xiaofeng/Item/tmp</Serverpath> 远程服务器存放文件的目录。\n");
  printf("<matchname>*.GIF</matchname> 待采集文件匹配的文件名，采用大写匹配，"\
         "不匹配的文件不会被采集，本字段尽可能设置精确，不允许用*匹配全部的文件。\n");
  printf("<type>1</type> 文件采集成功后，远程服务器文件的处理方式：3-什么也不做；2-删除；1-备份，如果为1，还要指定备份的目录。\n");
  printf("<Serverbakpath>/home/xiaofeng/Item/baktmp</Serverbakpath> 文件采集成功后，服务器文件的备份目录，此参数只有当ptype=3时才有效。\n");
  printf("<listfilename>/home/xiaofeng/Item/Data_swtiching_center/listfilename.txt</listfilename> 采集前列出服务器文件名的文件。\n");
  printf("<okfilename></home/xiaofeng/Item/Data_swtiching_center/okfilename.txt/okfilename> 已采集成功文件名清单，此参数只有当ptype=1时有效。\n");
  printf("<timetvl>30</timetvl> 采集时间间隔，单位：秒，建议大于10。\n\n");

}


void EXIT(int sig)
{
    Logfile.Write("程序退出,sig=%d\n",sig);
    exit(0);
}

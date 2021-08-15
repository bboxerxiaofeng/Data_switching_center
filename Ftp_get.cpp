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
}st_arg;

struct listfile
{
    char fileName[301];
    char fileTime[31];
};

vector<struct listfile> vlistfilename;
vector<struct listfile> vnewlistfilename;
vector<struct listfile> vokfilename;



bool Compare();
bool LoadToVokfilename(char *okfilename);
bool LoadToVlistfilename(char *listfilename);
bool GetFileType(int type);

int main(int argc,char *argv[])
{
    printf("./Ftp_get \"<Server_ip>192.168.198.129</Server_ip><Serverbakpath>/home/xiaofeng/Item/baktmp</Serverbakpath><logfile>logfile.txt</logfile><okfilename>okfilename.txt</okfilename><listfilename>/home/xiaofeng/Item/Data_swtiching_center/listfilename.txt</listfilename><Server_post>21</Server_post><username>xiaofeng</username><password>asdf1234</password><mode>1</mode><Localpath>/home/xiaofeng/Item/local_tmp</Localpath><Serverpath>/home/xiaofeng/Item/tmp</Serverpath>\"\n");


    GetXMLBuffer(argv[1],"Server_ip",st_arg.Server_ip);
    GetXMLBuffer(argv[1],"Server_post",&st_arg.Server_post);
    GetXMLBuffer(argv[1],"username",st_arg.username);
    GetXMLBuffer(argv[1],"password",st_arg.password);
    GetXMLBuffer(argv[1],"mode",&st_arg.mode);
    GetXMLBuffer(argv[1],"Localpath",st_arg.Localpath);
    GetXMLBuffer(argv[1],"Serverpath",st_arg.Serverpath);
    GetXMLBuffer(argv[1],"Serverbakpath",st_arg.Serverbakpath);
    GetXMLBuffer(argv[1],"listfilename",st_arg.listfilename);
    GetXMLBuffer(argv[1],"okfilename",st_arg.okfilename);
    GetXMLBuffer(argv[1],"logfile",st_arg.logfile);
    
    Logfile.Open(st_arg.logfile,"a+");

    if( ftp.login(st_arg.Server_ip,st_arg.Server_post,st_arg.username,st_arg.password) == false)
    {
       Logfile.Write("ftp.login false\n");
    }

    ftp.chdir(st_arg.Serverpath);

    if(ftp.nlist(".",st_arg.listfilename)==false) Logfile.Write("nlist false\n"); // ftp获取到指定服务器目录下的所有文件，并将文件名写入到文件listfilename

    LoadToVlistfilename(st_arg.listfilename); // 将ftp.nlist获取到的文件名加载到容器vlistfilename里面 

    GetFileType(3);
}

bool LoadToVlistfilename(char *listfilename)
{
    struct listfile st_listfile;
    vlistfilename.clear();

    if( File.OpenForRead(listfilename,"r") == false) printf("open %s fail\n",listfilename);

    while(true)
    {
      if( File.Fgets(st_listfile.fileName,300,true) == false) break;
      if( ftp.mtime(st_listfile.fileName)==false ) return false;
      strcpy(st_listfile.fileTime,ftp.m_mtime); // 将ftp.mtime获取到的文件时间复制到结构体st_listfile.fileTime里面

      vlistfilename.push_back(st_listfile);
    }

    printf("vlistfilename=%ld\n",vlistfilename.size());
    return true;

}

bool LoadToVokfilename(char *okfilename)
{
    struct listfile st_oklistfile;
    char okbuf[301];
    vokfilename.clear();

    if( File.OpenForRead(okfilename,"r") == false) { Logfile.Write("%s open %s fail\n",__func__,okfilename); return false;}

    while(true)
    {
      if( File.Fgets(okbuf,300,true) == false) break;

      GetXMLBuffer(okbuf,"fileName",st_oklistfile.fileName,301);
      GetXMLBuffer(okbuf,"fileTime",st_oklistfile.fileTime,20);
      vokfilename.push_back(st_oklistfile);
    }

    printf("vokfilename====%ld\n",vokfilename.size());
    return true;

}

bool Compare()
{
    vnewlistfilename.clear();
    unsigned long jj;

    for(unsigned long ii=0; ii<vlistfilename.size(); ii++)
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

bool GetFileType(int type)
{

    if(type==3) // 获取服务器上的文件但不对已获取的文件进行任何操作，并且在下次获取的时候不获取已获取的文件
    {
    // 将okfilename (包含从服务器上已获取的文件名)里面的文件名加载到容器vokfilename里面
        LoadToVokfilename(st_arg.okfilename);          

    // 对比vokfilename和vlistfilename并将新增加的文件名加载到vnewlistfilename里面
        Compare();                           

    // 将vnewlistfilename复制到vlistfilename里面
        vlistfilename.clear();
        vlistfilename.swap(vnewlistfilename);
    }

    for(unsigned long ii=0;ii<vlistfilename.size();ii++)
    {
        char strServerfilename[301],strLocalfilename[301];
        SNPRINTF(strLocalfilename,300,"%s/%s",st_arg.Localpath,vlistfilename[ii].fileName); // 本地目录
        SNPRINTF(strServerfilename,300,"%s/%s",st_arg.Serverpath,vlistfilename[ii].fileName); // 服务器目录
            
        if(type==1) // 获取服务器上的文件并将已获取的文件移动到备份目录
        {
            char strServerfilenamebak[301];
            SNPRINTF(strServerfilenamebak,300,"%s/%s",st_arg.Serverbakpath,vlistfilename[ii].fileName);
            if(ftp.get(strServerfilename,strLocalfilename)==true) // 获取新增加的文件
            {
                Logfile.Write("get listfile--%s ok\n",vlistfilename[ii].fileName);
                if(ftp.ftprename(strServerfilename,strServerfilenamebak)==false) { Logfile.Write("ftprename %s false",vlistfilename[ii].fileName); }
            }
        }

        else if(type==2) // 获取服务器上的文件并将已获取的文件删除
        {
            if(ftp.get(strServerfilename,strLocalfilename)==true) // 获取新增加的文件
            {
                Logfile.Write("get listfile--%s ok\n",vlistfilename[ii].fileName);
                if(ftp.ftpdelete(vlistfilename[ii].fileName)==false) { Logfile.Write("ftpdelete %s false",vlistfilename[ii].fileName); }
            }
        }
        else if(type==3) // 获取服务器上新增的文件，并将已获取到的文件名追加到okfilename里面
        {
            if(ftp.get(strServerfilename,strLocalfilename)==true) // 获取新增加的文件
            {
                Logfile.Write("get newlistfile......%s ok\n",vlistfilename[ii].fileName);
                File.OpenForWrite(st_arg.okfilename,"a+");    
                File.Fprintf("<fileName>%s</fileName><fileTime>%s</fileTime>\n",vlistfilename[ii].fileName,vlistfilename[ii].fileTime);
            }

        }

    }

}


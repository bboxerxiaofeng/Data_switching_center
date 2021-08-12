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
    char listfilename[301];
    char okfilename[301];
    char logfile[301];
}st_arg;

vector<string> vlistfilename;
vector<string> vnewlistfilename;
vector<string> vokfilename;


bool GetNewfile_and_AddOkfile();
bool Compare();
bool Load_vokfilename(char *okfilename);
bool Load_vlistfilename(char *listfilename);

int main(int argc,char *argv[])
{
    printf("./Ftp_get \"<Server_ip>192.168.198.129</Server_ip><logfile>logfile.txt</logfile><okfilename>okfilename.txt</okfilename><listfilename>/home/xiaofeng/Item/Data_swtiching_center/listfilename.txt</listfilename><Server_post>21</Server_post><username>xiaofeng</username><password>asdf1234</password><mode>1</mode><Localpath>/home/xiaofeng/Item/local_tmp</Localpath><Serverpath>/home/xiaofeng/Item/tmp</Serverpath>\"\n");

    GetXMLBuffer(argv[1],"Server_ip",st_arg.Server_ip);
    GetXMLBuffer(argv[1],"Server_post",&st_arg.Server_post);
    GetXMLBuffer(argv[1],"username",st_arg.username);
    GetXMLBuffer(argv[1],"password",st_arg.password);
    GetXMLBuffer(argv[1],"mode",&st_arg.mode);
    GetXMLBuffer(argv[1],"Localpath",st_arg.Localpath);
    GetXMLBuffer(argv[1],"Serverpath",st_arg.Serverpath);
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

    Load_vokfilename(st_arg.okfilename); // 将okfilename (包含从服务器上已获取的文件名)里面的文件名加载到容器vokfilename里面
    Load_vlistfilename(st_arg.listfilename); // 将ftp.nlist获取到的文件名加载到容器vlistfilename里面 
    Compare(); // 对比vokfilename和vlistfilename并将新增加的文件名加载到vnewlistfilename里面
    GetNewfile_and_AddOkfile(); // 通过ftp获取新增的文件，获取完成之后将新增的文件名追加到文件okfilename

}

bool Load_vlistfilename(char *listfilename)
{
    char listbuf[301];

    if( File.OpenForRead(listfilename,"r") == false) printf("open %s fail\n",listfilename);
    vlistfilename.clear();
    while(true)
    {
      if( File.Fgets(listbuf,300,true) == false) break;
      vlistfilename.push_back(listbuf);
    }

    printf("vlistfilename=%ld\n",vlistfilename.size());

}

bool Load_vokfilename(char *okfilename)
{
    char okbuf[301];
    vokfilename.clear();

    if( File.OpenForRead(okfilename,"r") == false) { Logfile.Write("%s open %s fail\n",__func__,okfilename); return false;}
    while(true)
    {
      if( File.Fgets(okbuf,300,true) == false) break;
      vokfilename.push_back(okbuf);
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
                if(strcmp(vlistfilename[ii].c_str(),vokfilename[jj].c_str())==0) // if vlistfilename[ii] is the same to vokfilename number,it mean that vlistfilename[ii] is not a new filename
                {

                    break;

                }
            }

            if(jj==vokfilename.size()) 
                    vnewlistfilename.push_back(vlistfilename[ii]); // 如果vokfilename里面没有和vlistfilename[ii]一样的文件名，说明是新增的文件名
        }
    }
}

bool GetNewfile_and_AddOkfile()
{
    printf("vnewlistfilename===%ld\n",vnewlistfilename.size());
    for(unsigned long ii=0;ii<vnewlistfilename.size();ii++)
    {
        char strServerfilename[301],strLocalfilename[301];
        SNPRINTF(strLocalfilename,300,"%s/%s",st_arg.Localpath,vnewlistfilename[ii].c_str()); // 本地目录
        SNPRINTF(strServerfilename,300,"%s/%s",st_arg.Serverpath,vnewlistfilename[ii].c_str()); // 服务器目录
        if(ftp.get(strServerfilename,strLocalfilename)==true) // 获取新增加的文件
        {

            Logfile.Write("get newlistfile--%s ok\n",vnewlistfilename[ii].c_str());
            File.OpenForWrite(st_arg.okfilename,"a+");   // 
            File.Fprintf("%s\n",vnewlistfilename[ii].c_str());
        }

    }
    return true;
}

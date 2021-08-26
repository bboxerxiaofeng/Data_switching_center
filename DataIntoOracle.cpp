
#include <iostream>
#include "/home/xiaofeng/Item/Code_frame/htidc1127/htidc/c/_public.h"
#include <vector>
#include "string.h"
#include "/home/xiaofeng/Item/Local_Cloud_server/_ooci.h"



// 定义适应处理后数据格式的结构体
struct St_stcode_change
{
    int Station_number;
    char Data_time[11];
    float Temperature;
    float Air_pressure;
    float Relative_humidity;
    float Wind_direction;
    float Wind_speech;
    float Rainfall;
    float Visibility;
}stcode_change;


CFile File;
CLogFile LogFile;
CDir Dir;
connection conn;

int DataIntoDb();

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
       printf("该程序的功能为生成测试数据,请按照以下格式进行重新输入：\n "\
                "运行的bin+存放日志的路径\n" \
                "比如：./DataIntoOracle /home/xiaofeng/Item/Data_swtiching_center/log2.txt \n");
        return false;
    }

    
    Dir.OpenDir("/home/xiaofeng/Item/local_tmp");
    LogFile.Open(argv[1],"a+");

    while(true)
    {
        if(Dir.ReadDir() == false) 
        {
            LogFile.Write("没有读取到目录下的文件\n");
            break;

        }
        
        if( conn.connecttodb("test/test123@snorcl11g_192","AMERICAN_AMERICA.AL32UTF8") !=0 )
        {
            LogFile.Write("connect to db failed\n");
        }

        LogFile.Write("开始处理文件%s...",Dir.m_FileName);

        if(DataIntoDb()==false) 
        {
            LogFile.WriteEx("失败\n");
            break;
        }

        LogFile.WriteEx("成功\n");

    }

    conn.disconnect();

    return 0;


}

int DataIntoDb()
{

    CCmdStr CmdStr;
    char File_Row_Buffer[100];
    if(File.OpenForRead(Dir.m_FullFileName,"r")==false) return false;

    sqlstatement stmtins(&conn);
    stmtins.prepare("insert into scene (Station_number,Data_time,Temperature,Air_pressure,Relative_humidity,Wind_direction,Wind_speech,Rainfall,Visibility,crttime,keyid) values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,:4,:5,:6,:7,:8,:9,sysdate,SEQ_SURFDATA.nextval)");
    stmtins.bindin(1,&stcode_change.Station_number);
    stmtins.bindin(2,stcode_change.Data_time,19);
    stmtins.bindin(3,&stcode_change.Temperature);
    stmtins.bindin(4,&stcode_change.Air_pressure);
    stmtins.bindin(5,&stcode_change.Relative_humidity);
    stmtins.bindin(6,&stcode_change.Wind_direction);
    stmtins.bindin(7,&stcode_change.Wind_speech);
    stmtins.bindin(8,&stcode_change.Rainfall);
    stmtins.bindin(9,&stcode_change.Visibility);

    while(true)
    {
        memset(File_Row_Buffer,0,sizeof(File_Row_Buffer));
     // 读取每一行数据
        if(File.FFGETS(File_Row_Buffer,300)==false) break; 

     // 对每一行数据进行拆分
        CmdStr.SplitToCmd(File_Row_Buffer,",",true);

     // 对拆分的数据进行提取,并用结构体进行存储
        CmdStr.GetValue(0,&stcode_change.Station_number);
        CmdStr.GetValue(1,stcode_change.Data_time);
        CmdStr.GetValue(2,&stcode_change.Temperature);
        CmdStr.GetValue(3,&stcode_change.Air_pressure);
        CmdStr.GetValue(4,&stcode_change.Relative_humidity);
        CmdStr.GetValue(5,&stcode_change.Wind_direction);
        CmdStr.GetValue(6,&stcode_change.Wind_speech);
        CmdStr.GetValue(7,&stcode_change.Rainfall);
        CmdStr.GetValue(8,&stcode_change.Visibility);

        if(stmtins.execute()!=0)
        {
            //LogFile.Write("stmtins execute fails\n%s\n%s",stmtins.m_sql,stmtins.m_cda.message);
            //return false;
            if(stmtins.m_cda.rc !=1)
            {
                return false;

            }
        }
        
    }
    conn.commit();

    //File.CloseAndRemove();

    return true;

}

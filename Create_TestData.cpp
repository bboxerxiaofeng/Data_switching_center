#include <iostream>
//#include "/home/xiaofeng/Item/Code_frame/htidc1127/freec++-1.0.0/public/_public.h"
#include "/home/xiaofeng/Item/Code_frame/htidc1127/htidc/c/_public.h"
#include <vector>
#include "string.h"


// 定义适应原始数据中的一行数据的结构体
struct St_stcode
{
    int Station_number;
    char Province_name[31];
    char City_name[31];
    double Dimension;
    double Longitude;
    double Altitude;
};

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
};

vector<struct St_stcode_change> VSt_stcode_change;
CFile File;
CCmdStr CmdStr;
CLogFile LogFile;
St_stcode stcode;
St_stcode_change stcode_change;
int Load_Local_Data(char *filename);

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("该程序的功能为生成测试数据,请按照以下格式进行重新输入：\n "\
                "运行的bin+要读取文件路径+经处理后文件的存放路径\n" \
                "比如：./Create_TestData /home/xiaofeng/Item/ini/stcode.ini /home/xiaofeng/Item/tmp\n");
        return false;
    }

    Load_Local_Data(argv[1]);

    char strLocalTime[31];
    LocalTime(strLocalTime,"yyyymmddhh24miss");

    char Filename[31];
    memset(Filename,0,sizeof(Filename));
    snprintf(Filename,300,"%s/%s-%d",argv[2],strLocalTime,getpid());

    if(File.OpenForRename(Filename,"w")==false) { LogFile.Write("打开文件%s失败\n",argv[2]); return false; }
    for(int ii=0;ii<VSt_stcode_change.size();ii++)
    {
        File.Fprintf("%d,%s,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f\n",VSt_stcode_change[ii].Station_number,VSt_stcode_change[ii].Data_time, \
                     VSt_stcode_change[ii].Temperature,VSt_stcode_change[ii].Air_pressure,VSt_stcode_change[ii].Relative_humidity, \
                     VSt_stcode_change[ii].Wind_direction,VSt_stcode_change[ii].Wind_speech,VSt_stcode_change[ii].Rainfall, \
                     VSt_stcode_change[ii].Visibility );
    }

    File.CloseAndRename(); // 关闭文件

    VSt_stcode_change.clear(); // 清空容器

    return 0;
}

int Load_Local_Data(char *filename)
{


    if(File.OpenForRead(filename,"r")==false)       { LogFile.Write("打开文件%s失败\n",filename); return false; }
    char File_Row_Buffer[100];

    while(true)
    {
        memset(File_Row_Buffer,0,sizeof(File_Row_Buffer));
     // 读取每一行数据
        if(File.FFGETS(File_Row_Buffer,300)==false) { LogFile.Write("读取文件%s失败\n",filename);  return false; } 

     // 对每一行数据进行拆分
        CmdStr.SplitToCmd(File_Row_Buffer,",",true);

     // 对拆分的数据进行提取,并用结构体进行存储
        CmdStr.GetValue(1,&stcode.Station_number);
        
     // 对取出来的数据进行处理,并生成随机数
        stcode_change.Station_number=stcode.Station_number;
        LocalTime(stcode_change.Data_time,"yyyymmddhh24miss");
        stcode_change.Temperature=rand()%100;
        stcode_change.Air_pressure=rand()%100;
        stcode_change.Relative_humidity=rand()%100;
        stcode_change.Wind_direction=rand()%100;
        stcode_change.Wind_speech=rand()%100;
        stcode_change.Rainfall=rand()%100;
        stcode_change.Visibility=rand()%100;

     // 将处理好的数据存到容器中
        VSt_stcode_change.push_back(stcode_change);
        
        
    }

}

#include "iostream"
#include "vector"
#include "_ooci.h"
#include "/home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.h"



// 定义用于查询数据的结构，与表中的字段对应
struct st_person
{
  long id;        // 序列号，主键
  char name[31];     // 用户名称，用char对应oracle的varchar2，注意，表中字段的长度是30，char定义的长度是31，要留C语言的结束符
  char password[31]; // 用户密码
};
extern struct st_person stperson;
extern vector<struct st_person> V_st_person;
extern CLogFile LogFile;

// 本程序演示从商品表中查询数据
bool select_oracle();
// 本程序演示向商品表中插入数据。
int insert_oracle(char *Str_Read,char *Str_Write);
// 本程序演示创建一个表，用于存放商品信息。
int create_oracle(char *Str_Read,char *Str_Write);
// 本程序演示删除商品表中数据
int delete_oracle(char *Str_Read,char *Str_Write);
// 本程序演示更新商品表中数据
int update_oracle(char *new_password,long password_id);

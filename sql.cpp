
#include "sql.h"

//
// 本程序演示从商品表中查询数据
//
bool select_oracle()
{
  // 数据库连接池
  connection conn;
  
  // 连接数据库，返回值0-成功，其它-失败
  // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
  if (conn.connecttodb("scott/13011021@snorcl11g_192","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
  
  // SQL语言操作类
  sqlstatement stmt(&conn);

  // 准备查询数据的SQL，不需要判断返回值
  stmt.prepare("\
    select name,password,no from person");

  // 为SQL语句绑定输出变量的地址
  stmt.bindout(1, stperson.name,30);
  stmt.bindout(2, stperson.password,30);
  stmt.bindout(3, &stperson.id);

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
  }

  while (1)
  {
    // 先把结构体变量初始化，然后才获取记录
    memset(&stperson,0,sizeof(stperson));

    // 获取一条记录，一定要判断返回值，0-成功，1403-无记录，其它-失败
    // 在实际应用中，除了0和1403，其它的情况极少出现。
    if (stmt.next() !=0) break;
    
    // 将获取到的值存放到容器里
    V_st_person.push_back(stperson);

    // 把获取到的记录的值打印出来
    LogFile.Write("name=%s=,password=%s=,id=%ld=\n",stperson.name,stperson.password,stperson.id);
  }

  // 请注意，stmt.m_cda.rpc变量非常重要，它保存了SQL被执行后影响的记录数。
  LogFile.Write("本次查询了goods表%ld条记录。\n",stmt.m_cda.rpc);
  
  return 0;
}


//
// 本程序演示向商品表中插入10条记录。
//

int insert_oracle(char *Str_Read,char *Str_Write)
{
  // 数据库连接池
  connection conn;

  // 连接数据库，返回值0-成功，其它-失败
  // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
  if (conn.connecttodb("scott/13011021@snorcl11g_192","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
  
  // SQL语言操作类
  sqlstatement stmt(&conn);

  // 准备插入数据的SQL，不需要判断返回值
  stmt.prepare("\
    insert into person(id,name,password) \
                values(:1,:2,:3))");
                //values(:1,:2,:3,to_date(:4,'yyyy-mm-dd hh24:mi:ss'),to_null(:5))");
  // 为SQL语句绑定输入变量的地址
  stmt.bindin(1,&stperson.id);
  stmt.bindin(2, stperson.name,30);
  stmt.bindin(3,stperson.password,30);
  //stmt.bindin(4, stgoods.btime,19);
  //stmt.bindin(5, stgoods.t,10);

  // 模拟商品数据，向表中插入10条测试信息
  for (int ii=13;ii<=15;ii++)
  {
    // 结构体变量初始化
    memset(&stperson,0,sizeof(stperson));

    // 为结构体的变量指定值
    stperson.id=ii;
    sprintf(stperson.name,"商品名称%02d",ii);
    //stgoods.sal=ii*2.11;
    //strcpy(stgoods.btime,"2018-03-01 12:25:31");

    // 每次指定变量的值后，执行SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmt.execute() != 0)
    {
      //printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
    }

    printf("insert ok(id=%d,rpc=%ld).\n",ii,stmt.m_cda.rpc);
  }

  printf("insert table goods ok.\n");

  // 提交数据库事务
  conn.commit();

  return 0;
}

//
// 本程序演示创建一个表，用于存放商品信息。
//


int create_oracle(char *Str_Read,char *Str_Write)
{
  // 数据库连接池
  connection conn;

  // 连接数据库，返回值0-成功，其它-失败
  // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
  if (conn.connecttodb("scott/13011021@snorcl11g_192","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
  
  // SQL语言操作类
  sqlstatement stmt(&conn);

  // 准备创建表的SQL，商品表：商品编号id，商品名称name，价格sal
  // 入库时间btime，商品说明memo，商品图片pic
  // prepare方法不需要判断返回值
  stmt.prepare("\
    create table goods(id    id(10),\
                       name  varchar2(30),\
                       sal   number(10,2),\
                       btime date,\
                       memo  clob,\
                       pic   blob,\
                       primary key (id))");

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
  }

  printf("create table goods ok.\n");

  return 0;
}

//
// 本程序演示删除商品表中数据
//

int delete_oracle(char *Str_Read,char *Str_Write)
{
  // 数据库连接池
  connection conn;
  
  // 连接数据库，返回值0-成功，其它-失败
  // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
  if (conn.connecttodb("scott/tiger@snorcl11g_198","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
  
  // SQL语言操作类
  sqlstatement stmt(&conn);

  int iminid,imaxid;

  
  // 准备删除数据的SQL，不需要判断返回值
  stmt.prepare("delete from goods where id>:1 and id<:2");
  // 为SQL语句绑定输入变量的地址
  stmt.bindin(1,&iminid);
  stmt.bindin(2,&imaxid);

  // 手工指定id的范围为1到5
  iminid=1;
  imaxid=5;
  

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
  }

  // 请注意，stmt.m_cda.rpc变量非常重要，它保存了SQL被执行后影响的记录数。
  printf("本次从goods表中删除了%ld条记录。\n",stmt.m_cda.rpc); 

  // 提交事务
  conn.commit();

  return 0;
}

//
// 本程序演示更新商品表中数据
//

int update_oracle(char *new_password,long password_id)
{

  // 数据库连接池
  connection conn;
  
  // 连接数据库，返回值0-成功，其它-失败
  // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
  if (conn.connecttodb("scott/13011021@snorcl11g_192","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return false;
  }
  
  // SQL语言操作类
  sqlstatement stmt(&conn);

  LogFile.Write("updatepassword-%s id-%ld\n",new_password,password_id);
  // 准备更新数据的SQL，不需要判断返回值
  stmt.prepare("\
    update person set password=:1 where no=:2");

  stmt.bindin(1,new_password,strlen(new_password));
  stmt.bindin(2,&password_id);

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return false;
  }

  // 请注意，stmt.m_cda.rpc变量非常重要，它保存了SQL被执行后影响的记录数。
  LogFile.Write("update person%ld。\n",stmt.m_cda.rpc);

  // 提交事务
  conn.commit();

  return true;
}


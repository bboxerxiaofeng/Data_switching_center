
ORACLE_HOME=/oracle/home
    
# oracle头文件路径
ORAINCL = -I$(ORACLE_HOME)/rdbms/public 
   
# oracle库文件路径
ORALIB =  -L$(ORACLE_HOME)/lib -L.
    
# oracle的oci库
ORALIBS = -lclntsh 

CFLAGS = -g -Wall -Wno-write-strings #-Wno-unused-variable

FreeH=/home/xiaofeng/Item/Code_frame/htidc1127/htidc/c/_public.h
FreeC=/home/xiaofeng/Item/Code_frame/htidc1127/htidc/c/_public.cpp


all:Create_TestData Ftp_get DataIntoOracle libftp.a

Create_TestData:Create_TestData.cpp $(FreeC) $(FreeH)
	g++ -g -o Create_TestData Create_TestData.cpp $(FreeC) 

libftp.a:ftplib.c ftplib.h
	gcc -c -o libftp.a ftplib.c

Ftp_get:Ftp_get.cpp $(FreeC) $(FreeH) ftp.cpp ftp.h libftp.a
	g++ -g -o Ftp_get Ftp_get.cpp $(FreeC) ftp.cpp libftp.a

DataIntoOracle:DataIntoOracle.cpp $(FreeC) $(FreeH) _ooci.h 
	g++ $(CFLAGS) -o DataIntoOracle DataIntoOracle.cpp $(FreeC) $(ORALIB) $(ORAINCL) $(ORALIBS) _ooci.h _ooci.cpp -lm -lc

Clean:
	rm -f Create_TestData


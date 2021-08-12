FreeH=/home/xiaofeng/Item/Code_frame/htidc1127/htidc/c/_public.h
FreeC=/home/xiaofeng/Item/Code_frame/htidc1127/htidc/c/_public.cpp


all:Create_TestData Ftp_get libftp.a

Create_TestData:Create_TestData.cpp $(FreeC) $(FreeH)
	g++ -g -o Create_TestData Create_TestData.cpp $(FreeC) 

libftp.a:ftplib.c ftplib.h
	gcc -c -o libftp.a ftplib.c

Ftp_get:Ftp_get.cpp $(FreeC) $(FreeH) ftp.cpp ftp.h libftp.a
	g++ -g -o Ftp_get Ftp_get.cpp $(FreeC) ftp.cpp libftp.a

Clean:
	rm -f Create_TestData


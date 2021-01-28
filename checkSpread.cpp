#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include<cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

using namespace std;

#define warning_hour  0  //时
#define warning_min   6  //分

const char* file_name = "/usr/local/spread/Log/AllSpread.log";
//const char* file_name = "/mnt/hgfs/ThirdSoftware/test_proj/checkSpread/test.log";

int sendTelegramMessage(string sendstr)
{
	CURL *curl;
	CURLcode res;
	
	//拼接字段 
	//https://api.telegram.org/bot1116069769:AAGX0xRcq1LLDEXjXKzsaTt0rl9jfU53L94/sendMessage?chat_id=-321010467&text=I am is Notice Robot	
	const string telegramboturl = "https://api.telegram.org/bot"; //api
	const string robotid = "1116069769"; //robotid
	const string token = "AAGX0xRcq1LLDEXjXKzsaTt0rl9jfU53L94";  //telegram token
	const string opstyle = "sendmessage";  //操作方式
	//const string chatid = "-451359637";  // 发给个人andres(639086835405)的chatid
	const string chatid = "-321010467"; //(server 棋牌群)
	
	string buf;
	buf.append(telegramboturl);
	buf.append(robotid);
	buf.append(":");
	buf.append(token);
	buf.append("/");
	buf.append(opstyle);
	buf.append("?chat_id=");
	buf.append(chatid);
	buf.append("&text=");
	buf.append(sendstr);
	
	const char *sendMessage = buf.c_str(); 
	cout << sendMessage << endl; 
	
	curl_global_init(CURL_GLOBAL_DEFAULT);
	
	curl = curl_easy_init();
	if(curl) {	 
	curl_easy_setopt(curl, CURLOPT_URL, sendMessage); 
	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
	
	/* always cleanup */
	curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	
	return 0;
}

//获取进程ID
pid_t getProcessPidByName(const char *proc_name)
{
	FILE *fp;
	char buf[100];
	char cmd[200] = {'\0'};
	pid_t pid = -1;
	sprintf(cmd, "pidof %s", proc_name);
	
	if((fp = popen(cmd, "r")) != NULL)
	{
		if(fgets(buf, 255, fp) != NULL)
			pid = atoi(buf);
	}
	//printf("pid = %d \n", pid);
	pclose(fp);
	return pid;
}

//获取文件大小和更新时间
int fileTime(time_t& modify_time)
{
	FILE * fp;
	struct stat buf;
	
	fp=fopen(file_name,"r");
	if(fp != NULL)
	{
	int fd=fileno(fp);
	fstat(fd, &buf);
	//file_size = buf.st_size; 
	//time_t now_time = time(NULL);
   // int d = (int)((now_time + 8 * 60 * 60) / (60 * 60 * 24)) - 1; //yesterday
	modify_time=buf.st_mtime; //latest modification time (seconds passed from 01/01/00:00:00 1970 UTC)
	//cout << "buf.st_mtime:" << buf.st_mtime << endl;
	fclose(fp);
	
	return 0;
	}
	cout << "日志文件" << file_name << "不存在!" << endl;
	return -1;
}

//正常更新时间是0点6分之前，如果超过0点6分没更新则报警一次，在非更新时间(0点0 - 0点6分)更新则发通知消息
//侦测一个进程，如果不存在则报警
int main(int argc, char** argv)
{
	if(argc < 2)
	{
		cout << "参数传入错误退出！" << endl;
		cout << "如 ./checkSpread tuiguang (监听tuiguang进程)" << endl;
		return 1;
	}
	
	char* process_name = argv[1];  //需要侦测的进程名参数
	while(1)
	{	 
		pid_t pid = getProcessPidByName((const char*)process_name);
		if(pid <= 0)
		{
			string sendstr = "Warning Message: 服务器(103.223.121.226)推广进程 tuiguang 未启动或退出!";
			sendTelegramMessage(sendstr);
		}
		
		/* 获取当前时间 */
		time_t t;
		struct tm *tmp;
		time(&t);
		tmp = localtime(&t);
		int curryear = tmp->tm_year; //当前年
		int currmon = tmp->tm_mon; //当前月
		int currday = tmp->tm_mday;//当前系统天
		int currhour = tmp->tm_hour; //当前系统小时
		int currmin = tmp->tm_min; //当前系统分钟
		
		//读取日志修改时间和日志大小
		time_t modify_time = 0L;
		if(fileTime(modify_time) == -1)return 1;
		
		struct tm *ttime;
		ttime = localtime(&modify_time);
		int year = ttime->tm_year; //当前文件修改年
		int mon = ttime->tm_mon; //当前文件修改月
		int day = ttime->tm_mday;//当前文件修改天
		int hour = ttime->tm_hour; //当前文件修改小时
		int min = ttime->tm_min; //当前文件修改分钟

		//每天0点开始获取推广日志更新情况,非0点时间段，1分钟检测一次，
		if(curryear == year && currmon == mon && currday == day 
		&& currhour == hour && currhour == warning_hour && currmin == min)
		{
			//检测不超过6分钟
			if(currmin <= warning_min) //0点小于6分
			{
        		string sendstr = "Notice Message: 服务器(103.223.121.226)日志文件 AllSpread.log 已更新!";
        		sendTelegramMessage(sendstr);
        		sleep(50);	//已更新，休眠(50秒)
			}
			else //0点大于6分
			{
				string sendstr = "Warning Message: 服务器(103.223.121.226)日志文件 AllSpread.log 未正常更新!";
       			sendTelegramMessage(sendstr);
       			sleep(50);  	//已更新，休眠(50秒)
			}
		}
		else if(curryear == year && currmon == mon && currday == day 
		&& currhour == hour && currmin == min && currhour != warning_hour ) //非0点
		{	
			//如果有更新，则发一条通知消息
			string sendstr = "Notice Message: 服务器(103.223.121.226)日志文件 AllSpread.log 在非更新时间段更新!";
			sendTelegramMessage(sendstr);
			sleep(50);  //休眠(50秒)
		}
		sleep(5);
	}
	return 0;
}

/* 
 * File:   prob2.c
 * Author: Gunj Manseta
 *
 * Created: 01/29/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/stat.h>
#include <sys/stat.h>
#include <string.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <linux/unistd.h>   
#include <sys/time.h> 
#include <time.h>
#include <malloc.h>

#define __sys_getpid		(20)	
#define __sys_getuid		(199)	
#define __sys_gettimeofday	(78)
#define GET_PID()			syscall(__sys_getpid)
#define GET_UID()			syscall(__sys_getuid)
#define GET_TIMEOFDAY(x,y)	syscall(__sys_gettimeofday,x,y)
#define FILE_NAME			"homework2.temp"

void get_time_string(char *timeString)
{
	struct timeval tv;
	struct tm* ptm;
	char time_string[40] = {0};
	long milliseconds;

 	/* Obtain the time of day using the system call */
	unsigned long ret = GET_TIMEOFDAY(&tv,NULL);
	if(ret != 0)
	{
		printf("Error.\n");
		memset(timeString,0,1);
	}
	ptm = localtime (&tv.tv_sec);
	/* Format the date and time. */
	//strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	strftime (time_string, sizeof (time_string), "%x - %X", ptm);
    memcpy(timeString,time_string,40);
}


int main(int argc, char** argv) {

    printf("Hello Cruel World.\n");

    printf("Process ID:\t %ld\n", GET_PID());
    printf("User ID:\t %ld\n", GET_UID());  
    char timeString[40] = {0};
    get_time_string(timeString);
    printf("Date - Time:\t %s\n", timeString); 
    
    const char * filename = FILE_NAME;
    FILE *fp = NULL;
    
    /**
     * @brief 
     * The file opened in write mode so it can create a
     * file if it does not exist  
     * @param Filename
     * @param Open mode.
     * @return 
    **/
    fp = fopen(filename,"w");
    if(!fp)
        printf("File open failed\n");
    fclose(fp);
    fp = NULL;
    
    int ret = chmod(filename, S_IRUSR | S_IWUSR); 
    if(ret != 0)
        printf("Error while changing file permission\n");
    
    fp = fopen(filename, "w");
    if(!fp)
        printf("File open error. Does the file exist?\n");
    
    char user_char = 'c';
    fwrite(&user_char,sizeof(char),1,fp);
    fclose(fp);
    fp = NULL;
    
    fp = fopen(filename, "a");
    if(!fp)
        printf("File open error.\n");
    
    char *user_string = (char*)malloc(sizeof(char)*50);
    memset(user_string,0,50);
    
    printf("Enter a string to write to the file.\n->");
    //using fgets instead of gets just to keep in check out of bound error 
    fgets(user_string,50,stdin);
    
    fwrite(user_string,strlen(user_string),1,fp);
    //to make sure the file stream flushes the data into a file
    fflush(fp);
    fclose(fp);
    fp = NULL;
    
    memset(user_string,0,50);
    
    fp = fopen(filename, "r");
    if(!fp)
        printf("File open error. Does file exist?\n");
    
    user_char = fgetc(fp);
    printf("Char read: %c\n",user_char);
    
    fgets(user_string,50,fp);
    printf("String Read: %s",user_string);
    
    fclose(fp);
    fp = NULL;
    
    free(user_string);
       
    
    return (0);
}

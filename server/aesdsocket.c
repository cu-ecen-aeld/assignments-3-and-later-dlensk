#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
//#include <sys/stat.h>
#include <fcntl.h>

#define DATAFILEPATH "/var/tmp/aesdsocketdata"

#define BACKLOG 10 
#define BUFFERSIZE 1024
#define PORT "9000"

int sockfd = -1;
int terminate_connection = 0;

void SignalHandler(int signal)
{
	if (signal == SIGTERM || signal == SIGINT)
	{
		syslog(LOG_INFO, "Caught signal, exiting");
		remove(DATAFILEPATH);
		terminate_connection = 1;
	}
	
}

void StartDaemon()
{
	pid_t pid = fork();
	
	if (pid < 0) 
	{
		syslog(LOG_ERR, "Fork was unsuccessfull");
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0) 
	{
		exit(EXIT_FAILURE);
	}
	
	if (setsid() == -1)
	{
		syslog(LOG_ERR, "Session creation was unsuccessful");
		exit(EXIT_FAILURE);
	}
	
	if (chdir("/") == -1)
	{
		syslog(LOG_ERR, "Unable to change to root directory");
		exit(EXIT_FAILURE);
	}
	
	int devNull = open("/dev/null", O_RDWR);
	
	if(devNull == -1)
	{
		syslog(LOG_ERR, "Unable to open /dev/null file");
		exit(EXIT_FAILURE);
	}
	
	dup2(devNull, STDOUT_FILENO);
	dup2(devNull, STDERR_FILENO);
	dup2(devNull, STDIN_FILENO);
	close(devNull);
}


int main( int argc, char *argv[] )
{
	int daemonActive = 0;
	char dataBuff[BUFFERSIZE];
	struct addrinfo hints, *servinfo;
        struct sockaddr_storage clientAddr; // connector's address information
        socklen_t addr_size;
        int yes=1; 
        
        signal(SIGTERM, SignalHandler);
        signal(SIGINT, SignalHandler);
        
        
        for (int arg = 1; arg < argc; arg++)
        {
	        if (strcmp(argv[arg], "-d") == 0)
	        {
	        	daemonActive = 1;
	        }
        }
        
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        
        if ( getaddrinfo(NULL, PORT, &hints, &servinfo) != 0)
        {
        	syslog(LOG_ERR, "Unable to get address info");
        	return -1;
        } 
        
        if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        {
        	syslog(LOG_ERR, "Socket creation unsuccessful");
        	return -1;
        }
        
        if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) ) == -1)
        {
        	syslog(LOG_ERR, "Socket binding unsuccessful");
        	close(sockfd);
        	freeaddrinfo(servinfo);
        	return -1;
        }
        
        if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        {
        	syslog(LOG_ERR, "Socket binding unsuccessful");
        	close(sockfd);
        	freeaddrinfo(servinfo);
        	return -1;
        }
        
        freeaddrinfo(servinfo);
             
        if (listen(sockfd, BACKLOG) == -1)
        {
        	syslog(LOG_ERR, "Unable to listen on socket");
        	return -1;	
        }
        
        //Check if daemon needs to be started
        if (daemonActive)
        {
        	StartDaemon();
        }
        
        while(!terminate_connection)
        {
        	int new_fd;
        	addr_size = sizeof clientAddr;
        	
        	new_fd = accept(sockfd, (struct sockaddr *)&clientAddr, &addr_size);
        	if (new_fd == -1) 
        	{
            		syslog(LOG_ERR, "Connection not accepted");
            		continue;     
            	}     
            	
            	int file_desc = open(DATAFILEPATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
            	if (file_desc == -1)
            	{
            		syslog(LOG_ERR, "Unable to open file");
            		close(new_fd);
            		continue;
            	}
            	
            	ssize_t recvBytes;
            	while ((recvBytes = recv(new_fd, dataBuff, BUFFERSIZE-1, 0)) > 0)
            	{
            		dataBuff[recvBytes] = '\0';
            		write(file_desc, dataBuff, recvBytes);
            		if (strchr(dataBuff, '\n'))
            		{
            		
            			break;
            		}
            	}
            	close(file_desc);
            	
            	file_desc = open(DATAFILEPATH, O_RDONLY);
            	
            	if (file_desc != -1)
            	{
            		while ( (recvBytes = read(file_desc, dataBuff, BUFFERSIZE)) > 0 )
            		{
            			send(new_fd, dataBuff, recvBytes, 0);
            		}
            		
            		close(file_desc);
            	} 
            	
            	close(new_fd);	        		
        }
        
        return 0;       
}




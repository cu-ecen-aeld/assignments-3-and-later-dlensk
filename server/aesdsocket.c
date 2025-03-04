#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

int srvrSock = -1;
int cliSock = -1;
#define DATAFILEPATH "/var/tmp/aesdsocketdata"
#define PORT "9000"
#define BACKLOG 10
#define BUFFERSIZE 1024


void StartDaemon() 
{
    pid_t pid = fork();
    
    if ( pid == -1 ) 
    {
        syslog(LOG_ERR, "Unable to fork");
        exit(EXIT_FAILURE);
    }
    
    if ( pid > 0 ) 
    {
        exit(EXIT_SUCCESS);
    }
    
    if ( setsid() == -1 ) 
    {
        syslog(LOG_ERR, "Unable to create session");
        exit(EXIT_FAILURE);
    }
    
    if ( chdir("/") == -1 )
    {
        syslog(LOG_ERR, "Unable to chdir to root");
        exit(EXIT_FAILURE);
    }
    
    
    close(STDERR_FILENO);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
}

void SigHandler( int sig )
{
    printf("Caught signal, exiting\n");
    
    if (srvrSock != -1)
    {
    close(srvrSock);
    }
    
    if (cliSock != -1) 
    {
    close(cliSock);
    }
    
    remove(DATAFILEPATH);
    
    exit(0);
}


int main( int argc, char *argv[] ) 
{    
    char dataBuffer[BUFFERSIZE];  
    int yes = 1;
    struct addrinfo hints, *srvrInfo;
    struct sockaddr_in cliAddr;
    int startDaemon = 0;
    socklen_t client_len = sizeof(cliAddr);    
    signal(SIGTERM, SigHandler);
    signal(SIGINT, SigHandler);
    
    
    for (int arg = 1; arg < argc; arg++)
    {
        if ( strcmp(argv[arg], "-d" ) == 0 ) 
        {
            startDaemon = 1;
        }
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &srvrInfo) != 0) 
    {
        syslog(LOG_ERR, "Unable to get address info");
        return -1;
    }

    if ( ( srvrSock = socket( srvrInfo->ai_family, srvrInfo->ai_socktype, srvrInfo->ai_protocol ) ) == -1 ) 
    {
        syslog(LOG_ERR, "Unable to create socket");
        freeaddrinfo(srvrInfo);
        return -1;
    }

    if ( setsockopt( srvrSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) ) == -1 ) 
    {
        syslog(LOG_ERR, "Socket options not set");
        close(srvrSock);
        freeaddrinfo(srvrInfo);
        return -1;
    }

    if ( bind( srvrSock, srvrInfo->ai_addr, srvrInfo->ai_addrlen ) == -1 ) 
    {
        syslog(LOG_ERR, "Unable to bind to socket");
        close(srvrSock);
        freeaddrinfo(srvrInfo);
        return -1;
    }
    
    freeaddrinfo(srvrInfo);

    if (startDaemon) 
    {
        StartDaemon();
    }

    if ( listen( srvrSock, BACKLOG ) == -1 ) 
    {
        syslog(LOG_ERR, "Unable to listen");
        return -1;
    }

    while ( 1 ) 
    {
        cliSock = accept( srvrSock, ( struct sockaddr * )&cliAddr, &client_len );
        
        if (cliSock == -1) 
        {
            syslog(LOG_ERR, "Unable to accept connection");
            continue;
        }

        printf("Accepted connection from %s\n", inet_ntoa(cliAddr.sin_addr));


        int fileDesc = open(DATAFILEPATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
        
        if (fileDesc == -1) 
        {
            syslog(LOG_ERR, "Unable to gopen file");
            close(cliSock);
            continue;
        }

	//Read until new line 
        ssize_t rcvBytes;
        
        while ( ( rcvBytes = recv( cliSock, dataBuffer, BUFFERSIZE - 1, 0 ) ) > 0 ) 
        {
            dataBuffer[rcvBytes] = '\0';
            write( fileDesc, dataBuffer, rcvBytes) ;
            if ( strchr( dataBuffer, '\n' ) )
            { 
            break;
            }
        }
        
        close(fileDesc);

	//Send back
        fileDesc = open(DATAFILEPATH, O_RDONLY);
        
        if ( fileDesc != -1 ) 
        {
            while ( ( rcvBytes = read( fileDesc, dataBuffer, BUFFERSIZE ) ) > 0 ) 
            {
                send(cliSock, dataBuffer, rcvBytes, 0);
            }
            
            close(fileDesc);
        }

        printf( "Closed connection from %s\n", inet_ntoa( cliAddr.sin_addr ) );
        
        close(cliSock);
    }

    return 0;
}

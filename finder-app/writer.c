#include <syslog.h>
#include <stdio.h>
#include <errno.h>

int main( int argc, char *argv[])
{
	openlog("writer", 0, LOG_USER);
	
	char *writeFile = argv[1];
	char *writeString = argv[2];
	
	
	if ( argc < 3 )
	{
		syslog( LOG_ERR, "Incorrect number of arguments");
		return 1;
	}
	
	FILE* fptr;
	
	// open file in write mode
    fptr = fopen( writeFile, "w" );
	
	if ( fptr == NULL) 
	{
		syslog( LOG_ERR, "Failed to write file" );
    }
    else 
	{
		fprintf( fptr, "%s", writeString );
		fclose( fptr );
		openlog("writer", 0, LOG_USER);
		syslog( LOG_DEBUG, "Writing %s to %s", writeString, writeFile );
		closelog();
     }

		
	return 0;
}
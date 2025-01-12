
#include <stdarg.h>
#include <stdio.h>

#include "utils.h"

void log_file_init() {
    FILE* fp = fopen( LOG_FILE, "w" );
    fprintf( fp, "vaxRaptor log_init()\n" );
    fclose( fp );
}

void log_to_screen( char* msg, ... ) {
    va_list args;
    va_start( args, msg );
    vprintf( msg, args );
    va_end( args );
    printf( "\n" );
}

void log_to_file( char* msg, ... ) {
    va_list args;

    FILE* fp = fopen( LOG_FILE, "a" );

    va_start( args, msg );
    vfprintf( fp, msg, args );
    va_end( args );
    fprintf( fp, "\n" );

    fclose( fp );
}

void log_to_file_and_screen( char* msg, ... ) {
    va_list args;

    FILE* fp = fopen( LOG_FILE, "a" );

    va_start( args, msg );
    vfprintf( fp, msg, args );
    vprintf( msg, args );
    va_end( args );
    fprintf( fp, "\n" );
    printf( "\n" );

    fclose( fp );
}

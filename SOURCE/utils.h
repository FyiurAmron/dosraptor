//
// Created by m on 2025-01-06.
//

#ifndef UTILS_H
#define UTILS_H

#define LOG_FILE "raptor.log"

void log_file_init();
void log_to_screen( char* msg, ... );
void log_to_file( char* msg, ... );
void log_to_file_and_screen( char* msg, ... );

#endif //UTILS_H

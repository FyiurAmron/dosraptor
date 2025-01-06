
#ifndef TSMAPI_H
#define TSMAPI_H

void TSM_Install();
int TSM_NewService( void ( *function )( void ), int rate, int priority, int pause );
void TSM_DelService( int id );
void TSM_Remove( void );
void TSM_PauseService( int id );
void TSM_ResumeService( int id );

#endif

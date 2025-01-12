
#include <string.h>

char* serial = "32768GLB";
#define SEED 0x0019

void GLB_DeCrypt(
    char* key,
    char* buffer,
    size_t length
) {
    size_t klen = strlen( key );
    int kidx;
    char prev_byte, dchr;

    kidx = SEED % klen;
    prev_byte = key[kidx];
    while ( length-- ) {
        dchr = ( *buffer - key[kidx] - prev_byte ) % 256;
        prev_byte = *buffer;
        *buffer++ = dchr;

        if ( ++kidx >= klen ) {
            kidx = 0;
        }
    }
}

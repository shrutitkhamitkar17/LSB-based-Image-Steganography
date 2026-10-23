#ifndef DECODE_H
#define DECODE_H

#include "types.h"  // Contains user defined types



/* 
 * Structure to store information required for
 * decoding secret file from encoded source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_FILE_EXTN 5

typedef struct _DecodeInfo
{
    /* Source Image Info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Output File Info*/
    char *secret_fname;
    FILE *fptr_secret;
    uint secret_file_extn_size;	
    char secret_file_extn[MAX_FILE_EXTN];         	
    uint secret_file_size;
    
}DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open Required files */
Status open_files_decode(DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(FILE *fptr_src_image, DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(FILE *fptr_src_image, DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(FILE *fptr_src_image, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(FILE *fptr_src_image, DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(FILE *fptr_src_image, FILE *fptr_secret, DecodeInfo *decInfo);

/* Decode a character from LSB's of each byte from stego image */
char decode_byte_from_lsb( char *image_buffer );

/* Decode function, which does real decoding */
char decode_data_from_image( DecodeInfo *decinfo );

#endif
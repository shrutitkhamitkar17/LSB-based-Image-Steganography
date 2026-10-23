#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Reads and validates the encoded bmp image and Checks for output file, then Reads and validates
 * bmp image name from command line input is verified and read into structure.
 * output image name from command line input is verified and read into structure if present
 * If no output file name is given, stores it as NULL
 * Returns e_success upon verifying .bmp file and output file else returns e_failure
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //input files .bmp
    if(strstr(argv[2], ".bmp") != NULL)
    {
        decInfo -> src_image_fname = argv[2];
    }
    else{
        return e_failure;
    }
    
    //Check output file passed or not and also check extension
    if(argv[3] != NULL)
    {
        if(strstr(argv[3], ".txt") != NULL)
        {
            decInfo -> secret_fname = argv[3];
        }   
        else
        {
            return e_failure;
        }
    }
    else
    {
        decInfo -> secret_fname = "decoded_secret.txt";

        printf("INFO: Output File not mentioned. Creating decoded_secret.txt as default\n");
    }
    

    return e_success;
}

Status open_files_decode(DecodeInfo *decInfo)
{
    //Src image file
    decInfo -> fptr_src_image = fopen(decInfo -> src_image_fname, "r");

    // Do Error handling
    if (decInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n", decInfo -> src_image_fname);
    }

    
    //secret_file.txt
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");

    // Do Error handling
    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n", decInfo -> secret_fname);
    }

    // No failure return e_success
    return e_success;
}

/* Decodes 8 byte in image_buffer to character */
char decode_byte_from_lsb( char *image_buffer )
{
    char ch = 0;

    /* Decode 1 character from 8 byte bit by bit */ 
    for( int i = 0; i < 8; i++ )
    {
        ch = ch | ( image_buffer[i] & 1 ) << i;
    }

    return ch; // Return decoded character
}

/* Reads 8 byte from stego and decodes */
char decode_data_from_image( DecodeInfo *decinfo )
{
    char decode_buff[8];

    fread( decode_buff, 8, 1, decinfo -> fptr_src_image ); // Read 8 byte from stego image
    
    char ch = decode_byte_from_lsb( decode_buff ); // Decode the 1 byte chara from 8 byte

    return ch;
}

/* Decode the magic string to identify if file is stegged */
Status decode_magic_string(FILE *fptr_src_image, DecodeInfo *decInfo)
{
    int len = strlen(MAGIC_STRING);
    char ch;
    
    for( int i = 0; i < len; i++ )
    {
        ch = decode_data_from_image( decInfo );

        if( i == 0 && ch != '#')
        {
            return e_failure;
        }
        if( i == 1 && ch != '*')
        {
            return e_failure;
        }
    }

    return e_success;
}

/* Decodes the secret file extension size */
Status decode_secret_file_extn_size(FILE *fptr_src_image, DecodeInfo *decInfo)
{
    uint size = 0;
    char* ch = ( char* )&size;  // Gets each byte to store the size byte by byte

    for( int i = 0; i < sizeof( uint ); i++ )
    {
        ch[i] = decode_data_from_image( decInfo );  // Decode each byte of size
    }

    decInfo -> secret_file_extn_size = size;

    return e_success;
}

/* Decode the extension of the encoded secret file*/
Status decode_secret_file_extn(FILE *fptr_src_image, DecodeInfo *decInfo)
{
    uint size = decInfo -> secret_file_extn_size;

    for( int i = 0; i < size; i++ )
    {
        decInfo -> secret_file_extn[i] = decode_data_from_image( decInfo );
    }
    
    decInfo -> secret_file_extn[size] = '\0'; // NULL terminate the extension name

    return e_success; 
}

/* Decode the size of encoded secret file */
Status decode_secret_file_size(FILE *fptr_src_image, DecodeInfo *decInfo)
{
    uint size;
    char* ch = (char*)&size; // Gets each byte to store the size byte by byte
    
    for( int i = 0; i < sizeof( uint ); i++ )
    {
        ch[i] = decode_data_from_image( decInfo ); // Decode size byte by byte
    }

    decInfo -> secret_file_size = size;

    return e_success;
}

/* Decode the secret message from bmp file */
Status decode_secret_file_data(FILE *fptr_src_image, FILE *fptr_secret, DecodeInfo *decInfo)
{
    uint size = decInfo -> secret_file_size;
    char image_data[size];

    rewind(fptr_secret);

    for( int i = 0; i < size; i++ )
    {
        char ch = decode_data_from_image( decInfo);       // Decode 1 byte
        
        fputc( ch, decInfo -> fptr_secret );              // Write decoded byte

        image_data[i] = ch;
    }
    
    image_data[size] = '\0'; // NULL terminate the extension name
    
    printf("Secrect file data:\n%s\n",image_data);

    return e_success; 
}

Status do_decoding(DecodeInfo *decInfo)
{
    //Start decoding
    printf("## Decoding Procedure Started ##\n");

    fseek(decInfo -> fptr_src_image, 54, SEEK_SET);

    // Decode magic string
    printf("INFO: Decoding Magic String Signature\n");
    if(decode_magic_string(decInfo -> fptr_src_image, decInfo) == e_success)
    {
        printf("INFO: Done.\n");

        // Decode file extension size
        printf("INFO: Decoding %s extension size\n", decInfo->secret_fname);
        if(decode_secret_file_extn_size(decInfo -> fptr_src_image, decInfo) == e_success)
        {
            printf("Secrect file extension size: %d\n",decInfo->secret_file_extn_size);
            printf("INFO : Done\n");

            // Decode file extension
            printf("INFO: Decoding %s extension\n", decInfo->secret_fname);
            if(decode_secret_file_extn(decInfo -> fptr_src_image, decInfo) == e_success)
            {
                printf("Secrect file extension: %s\n",decInfo->secret_file_extn);
                printf("INFO : Done\n");

                // Decode the file size
                printf("INFO: Decoding %s size\n", decInfo->secret_fname);
                if(decode_secret_file_size(decInfo -> fptr_src_image, decInfo) == e_success)
                {
                    printf("Secrect file size: %d\n",decInfo->secret_file_size);
                    printf("INFO : Done\n");

                    // Decode the encoded message from bmp file
                    printf("INFO: Decoding %s data\n",decInfo->secret_fname);
                    if(decode_secret_file_data(decInfo -> fptr_src_image, decInfo->fptr_secret, decInfo) == e_success)
                    {
                        printf("INFO : Done\n");
    
                    }
                    else
                    {
                        printf("ERROR: Failed to decode secret file data\n");
                    }
 
                }
                else
                {
                    printf("ERROR: Failed to decode secret file size\n");
                }    
            }
            else
            {
                printf("ERROR: Failed to decode secret file extension\n");
            }
        }
        else
        {
            printf("ERROR: Failed to decode secret file extension size\n");
        }
    }
    else
    {
        printf("ERROR: Failed to decode magic string \n");
    }

    // Close all the files
    fclose( decInfo -> fptr_secret );
    fclose( decInfo -> fptr_src_image );

    return e_success;
}
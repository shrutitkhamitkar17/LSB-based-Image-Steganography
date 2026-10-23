/* 
* Name : Shruti khamitkar (26011f_030)

* Description : 

* This project implements Image Steganography using the Least Significant Bit (LSB) 
* technique to securely hide secret data inside a BMP (.bmp) image. 
* The secret information is embedded into the LSBs of the image's RGB pixel data, 
* causing no noticeable change in the image quality. During decoding, 
* the hidden data is extracted by reading the modified LSBs and reconstructing the original file. 
* The project also uses a magic string to verify whether an image contains encoded data, 
* ensuring secure and reliable encoding and decoding of secret files.
*/


#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc, char *argv[])
{
    
    EncodeInfo E1;
    DecodeInfo D1;
    //Validate CLA
    if(argc >= 3 && argc <= 5)
    {
        OperationType res = check_operation_type(argv);

        if(res == e_encode)
        {
            printf("Encoding is selected\n");
            if(read_and_validate_encode_args(argv, &E1) == e_success)
            {
                printf("INFO: Opening required files\n");
                if(open_files_encode(&E1) == e_success)
                {
                    printf("INFO: DONE\n");
                    if(do_encoding(&E1) == e_success)
                    {
                        // Successfully did the encoding operation
                        printf("## Encoding Done Successfully ##\n");
                    }
                    else
                    {
                        printf("ERROR: Failed to encoding\n");
                        return 0;
                    }
                }
                else
                {
                    printf("ERROR: Failed to opening required files\n");
                    return 0;
                }
            }
            else
            {
                printf("INVALID ARGUMENTS\nUSAGE:\n");
                printf("%s: Encoding : %s -e <.bmp file> <.txt file> [.bmp output file optional]\n", argv[0], argv[0]);
                return 0;
            }
        } 

        if(res == e_decode)
        {
            printf("Decoding is selected\n");
            if(read_and_validate_decode_args(argv, &D1) == e_success)
            {
                printf("INFO: Opening required files\n");
                if(open_files_decode(&D1) == e_success)
                {
                    printf("INFO: DONE\n");
                    if(do_decoding(&D1) == e_success)
                    {
                        // Successfully did the decoding operation
                        printf("## Decoding Done Successfully ##\n");
                    }
                    else
                    {
                        printf("ERROR: Failed to decoding\n");
                        return 0;
                    }
                }
                else
                {
                    printf("ERROR: Failed to opening required files\n");
                }
            }
            else
            {
                printf("INVALID ARGUMENTS\nUSAGE:\n");
                printf("%s: Decoding : %s -d <.bmp file> [.txt output file optional]\n", argv[0], argv[0]);
                return 0;
            }
        } 

    }
    else
    {
        printf("INVALID ARGUMENTS\nUSAGE:\n");
        printf("%s: Encoding : %s -e <.bmp file> <.txt file> [.bmp output file optional]\n", argv[0], argv[0]);
        printf("%s: Decoding : %s -d <.bmp file> [.txt output file optional]\n", argv[0], argv[0]);
    } 
}

OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)
        return e_unsupported;
    else if(strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if(strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

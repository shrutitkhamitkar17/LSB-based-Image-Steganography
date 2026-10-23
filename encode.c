#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include <ctype.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_encode(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n", encInfo -> src_image_fname);
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n", encInfo -> secret_fname);
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s \n", encInfo -> stego_image_fname);
    }

    // No failure return e_success
    return e_success;
}

/* Validates the CLA and reads the file names */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //check input file .bmp
    if(strstr(argv[2], ".bmp") != NULL)
    {
        encInfo -> src_image_fname = argv[2];  // Saves .bmp file name
    }
    else 
    {
        return e_failure;   // Not .bmp file
    }

    //check input secret.txt
    if(strstr(argv[3], ".txt") != NULL)
    {
        encInfo -> secret_fname = argv[3];    // Saves the file name of any extension

        char *ext = strrchr(argv[3], '.');
        strcpy(encInfo->extn_secret_file, ext);   // Saves file extension
    }
    else
    {
        return e_failure;
    }

    //Check output file passed or not and also check extension
    if(argv[4] != NULL)
    {
        if(strstr(argv[4], ".bmp") != NULL)
        {
            encInfo -> stego_image_fname = argv[4];    // Creates stego image with input name
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        encInfo -> stego_image_fname = "steged_img.bmp"; // Creates stego image with default name

        printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
    }

    return e_success;
}

/* Gets the secret file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0 , SEEK_END);

    return ftell(fptr);
}


/* Checks if the source .bmp file has enough capacity to store data to be encoded*/
Status check_capacity(EncodeInfo *encInfo)
{
    // Store the image capacity
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    
    // Get secret file size
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    // Store secret string size
    encInfo -> size_extn = strlen(encInfo -> extn_secret_file);

    // Checks if total encoding size required is less than source file size without header size
    if(((encInfo -> image_capacity) - 54) > 16 + ((encInfo->size_extn + sizeof(encInfo->extn_secret_file) + encInfo->size_secret_file + sizeof(encInfo->size_secret_file)) * 8))
    {
        return e_success;
    }
    else
    {
        e_failure;
    }
}

/* Copies the .bmp header to stego file as it is */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[55];

    // Reset file pointers
    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    //Read 54 bytes from src image
    fread(buffer, 54, 1, fptr_src_image);

    //Write 54 bytes to dest image
    fwrite(buffer, 54, 1, fptr_dest_image);

    return e_success;
}

/* LSB of 8 bytes read will be encoded with 1 byte of secret file data */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
        // Encodes the 1byte( 8 bit ) to LSB of 8 bytes
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);
    }
}

/* Reads 8 bytes from source to encode 1 byte of secret file data */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    char image_buffer[8] = {};

    for(int i = 0; i < size; i++)
    {
        //Read 8 byte
        fread(image_buffer, 8, 1, encInfo -> fptr_src_image);

        // Steg 1 byte to 8 byte
        encode_byte_to_lsb(data[i], image_buffer);

        // Write 8 byte after encoding to output 
        fwrite(image_buffer, 8, 1, encInfo -> fptr_stego_image);
    }

    return e_success;
}

/* Encodes the magic string */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo);

    return e_success;
}

/* Encodes the size of secret file extension, which will be an integer */
Status encode_secret_file_extn_size(uint size, EncodeInfo *encInfo)
{
    // Character pointer to access each byte to encode
    char* extn_size_len = ( char* )&size;

    encode_data_to_image( extn_size_len, sizeof( uint ), encInfo );

    return e_success;
}

/* Encodes the secret file extension( .txt ) */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);

    return e_success;
}

/* Encodes the secret file size, which will be an integer */
Status encode_secret_file_size(uint file_size, EncodeInfo *encInfo)
{
     // Character pointer to access each byte to encode
    char* extn_size_len = ( char* )&file_size;

    encode_data_to_image( extn_size_len, sizeof( uint ), encInfo );

    return e_success;
}

/* Encodes the secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char data[encInfo -> size_secret_file];   // Buffer to encode data
    size_t read_bytes;

    //set the pointer of secret file to start
    rewind(encInfo -> fptr_secret);

    // Reads data from secret file
    while( ( read_bytes = fread( data, 1, sizeof( data ), encInfo -> fptr_secret ) ) > 0 )
    {
        encode_data_to_image( data, read_bytes, encInfo);
    }
    
    return e_success;
}

/* Copies the reamining data from source after completing encode to stego file */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest, EncodeInfo *encInfo)
{
    int size = 54 + encInfo -> image_capacity - ftell(fptr_src);

    char buffer[size];    // Buffer to copy data

    //Reads data to copy
    fread(buffer, size, 1, encInfo -> fptr_src_image);

    // Writes the read data 
    fwrite(buffer, size, 1, encInfo -> fptr_stego_image);

    return e_success;
}

/* To do the encoding process and calls each required function
 * Displays required informations and success, failure messages
 * Closes all the opened files
 */
Status do_encoding(EncodeInfo *encInfo)
{
    // Start encoding
    printf("## Encoding Procedure Started ##\n");

    printf("INFO: Checking for %s size\n", encInfo->secret_fname);
    
    if(get_file_size(encInfo -> fptr_secret) != 0)
    {
        printf("INFO: Done. Not Empty\n");

        printf("INFO: Checking for %s capacity to handle secret.txt\n", encInfo->src_image_fname);
        if(check_capacity(encInfo) == e_success)
        {
            printf("INFO: Done Found OK\n");

            // Copying header to stego
            printf("INFO: Copying Image Header\n");
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
            {
                printf("INFO: Done\n");

                // Encoding magic string
                printf("INFO: Encodeing Magic String Signature\n");
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("INFO: Done\n");

                    // Encoding secret file extension size
                    printf("INFO: Encoding %s File Extension Size\n", encInfo->secret_fname);
                    if(encode_secret_file_extn_size(encInfo->size_extn, encInfo) == e_success)
                    {
                        printf("INFO: Done\n");

                        // Encoding secret file extension
                        printf("INFO: Encoding %s File Extension\n", encInfo->secret_fname);
                        if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            printf("INFO: Done\n");

                            // Encoding Secret file size
                            printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
                            if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
                            {
                                printf("INFO: Done\n");

                                // Encode secret file data
                                printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("INFO: Done\n");

                                    // Copy remaining data to stego file 
                                    printf("INFO: Copying Left Over Data\n");
                                    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo) == e_success)
                                    {
                                        printf("INFO: Done\n");
                                    }
                                    else
                                    {
                                        printf("ERROR: Failed to copy remaining data\n");
                                        return e_failure;
                                    }
                                }
                            }
                            else
                            {
                                printf("ERROR: Failed to encode file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("ERROR: Failed to encode secret extn size\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Failed to encode secret file extn size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Failed to encode magic string");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Failed to copy bmp header\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: beautiful.bmp doesn't have the capacity to encode secret.txt”\n");
            return e_failure;
        }
    } 
    else
    {
        printf("ERROR: secret.txt file is empty\n");
    }

    // Close all the files
    fclose( encInfo -> fptr_src_image );
    fclose( encInfo -> fptr_secret );
    fclose( encInfo -> fptr_stego_image );


    return e_success;
}

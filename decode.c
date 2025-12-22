#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "decode.h"

/* Function Definitions */

//read arguments from command line arguments entered by user and validate
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //validate if 3rd argument is .bmp file
    if(strstr(argv[2], ".bmp") == NULL) 
    {
        
        return e_failure;
    }
    else
    {
        if(strcmp(strstr(argv[2], ".bmp"),".bmp"))
        {
            return e_failure;
        }
    }
    //store name of source image
    decInfo->src_image_fname = argv[2];

    //check for 4th argument
    //if it is entered by user 
    if(argv[3]!=NULL)
    {
        //store only name of file not the extension
        int i;
        for(i=0; argv[3][i]!='\0'; i++)
        {
            if(argv[3][i] == '.') 
            {
                break;
            }
        }
        argv[3][i] = '\0';
        strcpy(decInfo->dest_fname, argv[3]);
    }
    //if it is not entered by user store default as name without extension
    else
    {
        strcpy(decInfo->dest_fname, "default");
    }

    return e_success;
}

//skip bmp file header of input image
Status skip_bmp_header(FILE* fptr)
{
    fseek(fptr, 54, SEEK_SET);
    if(ftell(fptr) != 54)
    {
        return e_failure;
    }
    return e_success;
}

//decode size of magic string size
Status decode_magic_string_size(DecodeInfo *decInfo)
{
    //buffer to read and store 32 bytes of input image
    char buff[32];
    fread(buff, 1, 32, decInfo->fptr_src_image);

    //call decode size from LSB and store it in structure
    decInfo->magic_size = decode_size_from_lsb(buff);
    if(decInfo->magic_size == 0)
    {
        return e_failure;
    }
    return e_success;
}

//decode magic string from a image file
Status decode_magic_string(DecodeInfo *decInfo)
{
    //buffer to store 8 bytes of input image
    char buff[8], magic_string[decInfo->magic_size + 1];
    for(int i=0; i<decInfo->magic_size; i++)
    {
        //read 8 bytes from source image and store it in 8 bytes of buff
        fread(buff, 1, 8, decInfo->fptr_src_image);

        //call decode byte from LSB and store it in char array
        magic_string[i] = decode_byte_from_lsb(buff);
    }

    //ask user input of magic string
    char usr_input[10];
    printf("Enter your magic string: ");
    scanf("%s",usr_input);

    //validate user input and decoded magic string
    if(strcmp(magic_string, usr_input))
    {
        printf("Magic string unmatched\n");
        return e_failure;
    }

    return e_success;
}

//decode size of secret file extension from input image
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    //buffer to read and store 32 bytes of input image
    char buff[32];
    fread(buff, 1, 32, decInfo->fptr_src_image);

    //call decode_size_from_lsb and store it in structure
    decInfo->size_dest_file_extn = decode_size_from_lsb(buff);

    //validate extension size
    if(decInfo->size_dest_file == 0)
    {
        return e_failure;
    }
    printf("Size of extension : %d\n", decInfo->size_dest_file_extn);
    return e_success;
}

//decode extension of secret file
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    //buffer to read and store 8 bytes of input image
    char buff[8], file_extn[decInfo->size_dest_file_extn+1];

    for(int i=0; i<decInfo->size_dest_file_extn; i++)
    {
        //read 8 bytes from source image and store in 8 bytes
        fread(buff, 1, 8, decInfo->fptr_src_image);

        //call decode byte from LSB and store it in char array
        file_extn[i] = decode_byte_from_lsb(buff);
    }

    //validate decoded extension
    if(file_extn[0] != '.')
    {
        printf("Unable to decode extension\n");
        return e_failure;
    }

    //merge file name in structure with decoded extension
    int len = strlen(decInfo->dest_fname), i;
    for(i=0; i<decInfo->size_dest_file_extn; i++)
    {
        decInfo->dest_fname[len+i] = file_extn[i];
    }
    decInfo->dest_fname[len+i] = '\0';

    //open output file 
    decInfo->fptr_dest_file = fopen(decInfo->dest_fname, "w");

    //Error handling
    if(decInfo->fptr_dest_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error : opening destinatoin file %s\n", decInfo->dest_fname);
        return e_failure;
    }
    printf("Extension : %s\n", decInfo->dest_fname);
    return e_success;
}

//decode size of secret file
Status decode_secret_file_size(int* file_size, DecodeInfo *decInfo)
{
    //buffer to read and store 32 bytes of input image
    char buff[32];
    fread(buff, 1, 32, decInfo->fptr_src_image);

    //call decode size from lsb and store it as file size
    *file_size = decode_size_from_lsb(buff);

    //validate
    if(decInfo->size_dest_file == 0)
    {
        return e_failure;
    }
    return e_success;
}

//decode data of secret file
Status decode_secret_file_data(int file_size, DecodeInfo *decInfo)
{
    //buffer to read and store 8 bytes of input image
    char buff[8], data;
    for(int i=0; i<file_size; i++)
    {
        //read 8 bytes from source image and store in 8 bytes
        fread(buff, 1, 8, decInfo->fptr_src_image);

        //call decode byte from LSB and store it in data
        data = decode_byte_from_lsb(buff);

        //put that char data into ouput file
        fputc(data, decInfo->fptr_dest_file);
    }
    return e_success;
}

//decode size from LSB
int decode_size_from_lsb(char *imageBuffer)
{
    int size = 0;
    for(int i=31; i>=0; i--)
    {
        //get lsb from imagebuffer and set it to size
        if(imageBuffer[31-i] & 1)
        {
            size = size | (1<<i);
        }
    }
    return size;
}

//decode byte from LSB
char decode_byte_from_lsb(char *imageBuffer)
{
    char ch = 0;
    for(int i=7; i>=0; i--)
    {
        //get lsb from imagebuffer and set it to result character
        if(imageBuffer[7-i] & 1)
        {
            ch = ch | (1<<i);
        }
    }
    return ch;
}

//decoding operation and validation
Status do_decoding(DecodeInfo *decInfo)
{
    //open stego.bmp as input file
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");

    //Error handling
    if(decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error : opening file %s\n", decInfo->src_image_fname);
        return e_failure;
    }
    
    //call skip_bmp_header
    if(skip_bmp_header(decInfo->fptr_src_image) == e_failure)
    {
        printf("Header is not skipped correctly\n");
        return e_failure;
    }

    //call decode_magic_string_size
    if(decode_magic_string_size(decInfo) == e_failure)
    {
        printf("Unable to decode size of magic string\n");
        return e_failure;
    }

    //call decode_magic_string and validate
    if(decode_magic_string(decInfo) == e_failure)
    {
        printf("Unable to decode magic string\n");
        return e_failure;
    }

    //call decode_secret_file_extension_size and validate
    if(decode_secret_file_extn_size(decInfo) == e_failure)
    {
        printf("Unable to decode size of extension\n");
        return e_failure;
    }

    //call decode_secret_file_extn and validate
    if(decode_secret_file_extn(decInfo) == e_failure)
    {
        printf("Unable to decode extension\n");
        return e_failure;
    }

    //call decode_secret_file_size and validate
    int file_size;
    if(decode_secret_file_size(&file_size, decInfo) == e_failure)
    {
        printf("Unable to decode size of secret file\n");
        return e_failure;
    }
    printf("Size of file : %d\n", file_size);

    //call decode_secret_file_data and validate
    if(decode_secret_file_data(file_size, decInfo) == e_failure)
    {
        printf("Unable to decode secret file data\n");
        return e_failure;
    }

    return e_success;
}

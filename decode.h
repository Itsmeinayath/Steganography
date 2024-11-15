#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
   /* uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];*/

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
	int ext_size;
    char extn_secret_file[MAX_FILE_SUFFIX];
    //char secret_data[MAX_SECRET_BUF_SIZE]
    int size_secret_file;
	
} DecodeInfo;


// decoding

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(char *argv[],DecodeInfo *decInfo);

Status open_file_decode(DecodeInfo *decInfo);

Status decode_magic_string(char *magic_string,DecodeInfo *decInfo);

Status decode_ext_size(DecodeInfo *decInfo);

Status decode_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

Status decode_image_to_data(char *data, int size, FILE *fptr_src_image);

char decode_lsb_to_byte(char *image_buffer);

int decode_lsb_to_size(char *image_buffer);

#endif

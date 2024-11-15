#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"


// Function to validate command-line arguments for decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	    // Check if the second argument ends with ".bmp"
	char *ch = 	strstr(argv[2],".bmp");
	if (strcmp(ch,".bmp") == 0)
		decInfo -> src_image_fname = argv[2];
	else
	{
		printf ("Error : Enter the bmp file correctly.\n");
		return 0;
	}
		  // Argument validation successful
	return e_success;
}

// Function to carry out the decoding process
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
	 // Open the source image file
	if (open_file_decode(decInfo) == e_success)
	{
		printf ("Open file successfull\n");
	}
	else
	{
		printf ("Open file unsuccessfull\n");
		return e_failure;
	}
	 // Move file pointer to skip header
	fseek(decInfo -> fptr_src_image,54,SEEK_SET);
	 // Decode magic string from the image
	if (decode_magic_string(MAGIC_STRING,decInfo) == e_success)
	{
		printf ("Magic strings is matching\n");
	}
	else
	{
		printf ("Magic String is not matching\n");
		return e_failure;
	}
	 // Decode extension size from the image
	if (decode_ext_size(decInfo) == e_success)
	{
		printf ("Fetched the extntion size.\n");
	}
	else
	{
		printf ("Fetching of extntion size was unsuccessfull.\n");
		return e_failure;
	}
	 // Decode file extension from the image
	if (decode_file_extn(decInfo) == e_success)
	{
		printf ("File extension collected successfull.\n");
	}
	else
	{
		printf ("File extension collecting unsuccessfull.\n");
		return e_failure;
	}
	   // Handle secret file name
	char file[20] = "Decoded_sec";
	strcat(file,decInfo->extn_secret_file);
	if (argv[3] == NULL)// secret file name extn is checked if its passed in cla if not it will set the default file name
	{
		printf ("File name stored in secret_fname is Decoded_sec.\n");
		decInfo->secret_fname = file;
	}
	else
	{
		char *ch = strstr(argv[3],".");// if argv is passed then the extn are checked 
		if (strcmp(ch,decInfo->extn_secret_file) == 0)
		{
			printf ("File name stored in secret_fname.\n");
			decInfo -> secret_fname = argv[3];
		}
		else
		{
			printf ("The given file extension type is wrong the correct file with the extension is created with file name Decoded_sec.\n");
			decInfo->secret_fname = file;
		}
	}
	// Open the secret file
	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");// the secret file is opened here 
	if (decInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
		return e_failure;
	}
	 // Decode secret file size from the image
	if (decode_secret_file_size(decInfo) == e_success)
	{
		printf ("Secret file size fetched.\n");
	}
	else
	{
		printf ("Failed to fetch secret file size.\n");
		return e_failure;
	}
	 // Decode secret file data from the image
	if (decode_secret_file_data(decInfo) == e_success)
	{
		printf ("Secret file data fetched.\n");
	}
	else
	{
		printf ("Failed to fetch secret file data.\n");
		return e_failure;
	}		
	 // Close opened files
	fclose(decInfo->fptr_src_image);
	fclose(decInfo->fptr_secret);
	return e_success; 
}

// Function to open the source image file
Status open_file_decode(DecodeInfo *decInfo)
{
	decInfo -> fptr_src_image = fopen(decInfo -> src_image_fname, "r");
	// Do Error handling
	if (decInfo -> fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> src_image_fname);
		return e_failure;
	}
	return e_success;
}

// Function to decode magic string from the image
Status decode_magic_string(char *magic_string,DecodeInfo *decInfo)
{
	int size = strlen(magic_string);
	char *data;
	if (decode_image_to_data(data,size,decInfo->fptr_src_image) == e_success) // decode_image_to_data is called
	{
		data[size] = '\0';
		if (strcmp(magic_string,data) == 0) \
			return e_success;
		else
			return e_failure;
	}
	else
	{
		return e_failure;
	}
}

// Function to decode extension size from the image
Status decode_ext_size(DecodeInfo *decInfo)
{
	char src[32];
	fread(src,sizeof(char),32,decInfo->fptr_src_image);
	decInfo->ext_size = decode_lsb_to_size(src);
	return e_success;
}

// Function to decode file extension from the image
Status decode_file_extn(DecodeInfo *decInfo)
{
	if (decode_image_to_data(decInfo->extn_secret_file,decInfo->ext_size,decInfo->fptr_src_image) == e_success)
	{
		decInfo->extn_secret_file[decInfo->ext_size] = '\0';
		return e_success;
	}
	else
		return e_failure;
}

// Function to decode secret file size from the image
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	char src[32];
	fread(src,sizeof(char),32,decInfo->fptr_src_image);
	decInfo->size_secret_file = decode_lsb_to_size(src);
	return e_success;
}

// Function to decode secret file data from the image
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char sec_data[decInfo -> size_secret_file];
	if (decode_image_to_data(sec_data,decInfo->size_secret_file,decInfo->fptr_src_image) == e_success)
	{
		fwrite(sec_data,sizeof(char),decInfo->size_secret_file,decInfo->fptr_secret);
		return e_success;
	}
	else
		return e_failure;
}

// Function to decode image data
Status decode_image_to_data(char *data, int size, FILE *fptr_src_image)
{
	char src[8];
	for(int i=0;i<size;i++)
	{
		fread(src,sizeof(char),8,fptr_src_image);
		data[i] = decode_lsb_to_byte(src);
		
	}
	return e_success;
}

// Function to decode LSB to byte
char decode_lsb_to_byte(char *image_buffer)
{
	int j=0;
	char data;
	for (int i=7;i>=0;i--)
	{
		if ( image_buffer[j] & 1 )
			data = data | (1<<i);
		else
			data = data & (~(1<<i));
		j++;
	}
	return data;
}

int decode_lsb_to_size(char *image_buffer)
{
	int j=0,size=0;
	for (int i=31;i>=0;i--)
	{
		if ( image_buffer[j] & 1 )
			size = size | (1<<i);
		else
			size = size & (~(1<<i));
		j++;
	}	
	return size;
}
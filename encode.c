#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size
 is 4 bytes
 */

// if any function does not return e_success then do_encoding part will return e_failure
// each functions are called and its function does not return e_success then it will return e_failure
// if all functions are correct then do_encoding will return e_success
Status do_encoding(EncodeInfo *encInfo) // the encodeing part is writen here
{
	if (open_files(encInfo) == e_success) // the open_files funtion is called
		printf("Files opened successfully.\n");
	else
	{
		printf("Error: Unable to open files.\n");
		return e_failure;
	}
	if (check_capacity(encInfo) == e_success) // the check_capacity function is called
		printf("Capacity check successful.\n");
	else
	{
		printf("Error: Insufficient capacity.\n");
		return e_failure;
	}
	if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("BMP header copied successfully.\n");
		if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		{
			printf("Magic string encoded successfully.\n");

			// encInfo -> extn_secret_file = strstr(agrv[3],".");
			if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
			{
				printf("Secret file extension encoded successfully.\n");
				if (encode_secret_file_size(encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
				{
					printf("Secret file size encoded successfully.\n");
					if (encode_secret_file_data(encInfo) == e_success)
					{
						printf("Secret file data encoded successfully.\n");
						{
							if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
							{
								printf("Remaining image data copied successfully.\n");
							}
							else
							{
								printf("Error: Unable to copy remaining image data.\n");
								return e_failure;
							}
						}
					}
						else
						{
							printf("Error: Unable to encode secret file data.\n");
							return e_failure;
						}
					}
					else
					{
						printf("Error: Unable to encode secret file size.\n");
						return e_failure;
					}
				}
				else
				{
					printf("Error: Unable to encode secret file extension.\n");
					return e_failure;
				}
			}
			else
			{
				printf("Error: Unable to encode magic string.\n");
				return e_failure;
			}
		}
		else
		{
			printf("Error: Unable to copy BMP header.\n");
			return e_failure;
		}

		fclose(encInfo->fptr_secret);
		return e_success;
	}

	// this will check whether the passed files in commandlines are corret are not
	Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
	{
		 // Check if the input BMP file extension is correct
		char *ch = strstr(argv[2], ".bmp");
		if (strcmp(ch, ".bmp") == 0)
		     // Store the BMP file name in src_image_fname
			encInfo->src_image_fname = argv[2]; 
		else
		{
			// Print error message for incorrect BMP file extension
			printf("Error : Enter the bmp file correctly.\n"); 
			return e_failure;
		}
		
		// Check if the input TXT file extension is correct
		ch = strstr(argv[3], ".txt");
		if (strcmp(ch, ".txt") == 0)
		{
			//Copy the TXT file extension to extn_secret_file
			strcpy(encInfo->extn_secret_file, ch);
			// Store the TXT file name in secret_fname
			encInfo->secret_fname = argv[3]; 
		}
		else
		{
			 // Print error message for incorrect TXT file extension
			printf("Error : Enter the txt file correctly.\n"); 
			return e_failure;
		}
		 // Check if an output BMP file name is provided, otherwise use default
		if (argv[4] == NULL) 
		{
			// Use default BMP file name if no output is provided
			encInfo->stego_image_fname = "output.bmp";
		}
		else 
		{
			ch = strstr(argv[4], ".bmp");
			if (strcmp(ch, ".bmp") == 0)
			{
				// Store the provided BMP file name in stego_image_fname
				encInfo->stego_image_fname = argv[4]; 
			}
			else
			{
				// Use default BMP file name if provided name is incorrect
				encInfo->stego_image_fname = "output.bmp"; 
		    }
		}
		return e_success; 
	}

	uint get_image_size_for_bmp(FILE * fptr_image) 
	{
		uint width, height;
		// Seek to 18th byte:

		fseek(fptr_image, 18, SEEK_SET);

		// Read the width (an int)
		fread(&width, sizeof(int), 1, fptr_image);
		printf("width = %u\n", width);

		// Read the height (an int)
		fread(&height, sizeof(int), 1, fptr_image);
		printf("height = %u\n", height);

		// Return image capacity
		return width * height * 3; // is returned
	}

	uint get_file_size(FILE * fptr) // the txt file size is calculated
	{
		if (fseek(fptr, 0, SEEK_END) == 0)
			return ftell(fptr);
		else
			printf("fseek function unsuccessful\n");
	}

	/*
	 * Get File pointers for i/p and o/p files
	 * Inputs: Src Image file, Secret file and
	 * Stego Image file
	 * Output: FILE pointer for above files
	 * Return Value: e_success or e_failure, on file errors
	 */
	Status open_files(EncodeInfo * encInfo)
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

		// Secret file
		encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
		// Do Error handling
		if (encInfo->fptr_secret == NULL)
		{
			perror("fopen");
			fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
			return e_failure;
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

		// No failure return e_success
		return e_success;
	}

	// here the capacity encoded bits are checked whether the image_capacity is greater or the encodeing bits are greater
	Status check_capacity(EncodeInfo * encInfo)
	{
		encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
		encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
		if (encInfo->image_capacity >= 54 + (strlen(MAGIC_STRING) + 4 + 4 + 4 + encInfo->size_secret_file) * 8)
		{
			return e_success;
		}
		return e_failure;
	}

	// the header data from source image to stego_image is copied all the 54 bytes are copied byte by byte
	Status copy_bmp_header(FILE * fptr_src_image, FILE * fptr_dest_image)
	{
		/*uint raster_data;
		  fseek(fptr_src_image,10,SEEK_SET); // this is will point the pointer to 10th byte which contains header byte counts this will seet the pointer
		  fread(&raster_data,sizeof(int),1,fptr_src_image);// this will read and store it into the unsigned int raster data
		  rewind(fptr_src_image);// file ptr is rewind back to start*/
		char header[54];
		fseek(fptr_src_image, 0, SEEK_SET);
		fread(header, sizeof(char), 54, fptr_src_image); // the header data is read and stored in char arr
		if (ferror(fptr_src_image))						 // if any error occurs it will print the error msg
		{
			fprintf(stderr, "Error:While reading the data from the source image file.\n");
			clearerr(fptr_src_image);
			return e_failure;
		}
		fwrite(header, sizeof(char), 54, fptr_dest_image); // the header data is writen to the destination file char by char
		if (ferror(fptr_dest_image))					   // if error occurs then it will print the error msg
		{
			fprintf(stderr, "Error:While writing the data to the destination image file.\n");
			clearerr(fptr_dest_image);
			return e_failure;
		}
		return e_success; // returns e_success when all the 54 bytes are copied
	}

	// the magic_string is encoded into the stego_image by changeing the LSB bit of the i/p image bytes and its stored in the Stego image
	Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
	{
		if (magic_string != NULL)
			encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image); // encode_data_to_image is called
		else
			return e_failure;
		if (ftell(encInfo->fptr_src_image) == 70)
			return e_success;
		else
			return e_failure;
	}

	// secret file size of extn and extn is encoded
	Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
	{
		int size_ext = strlen(file_extn);
		char src[32];
		fread(src, sizeof(char), 32, encInfo->fptr_src_image);	  // fread data is read from the fptr_src_image and is stored in src
		encode_size_to_lsb(size_ext, src);						  // encode_size_to_lsb is called
		fwrite(src, sizeof(char), 32, encInfo->fptr_stego_image); // fwrite is called and from the src to fptr_stego_image the data copied
		// for the extn the encode_data_to_image is called
		if (encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
			return e_success;
		else
			return e_failure;
	}

	// encode_secret_file_size the size of the file is encoded
	Status encode_secret_file_size(int extn_size, FILE *fptr_src_image, FILE *fptr_stego_image)
	{
		char src[32];
		fread(src, sizeof(char), 32, fptr_src_image);	 // fread data is read from the fptr_src_image and is stored in src
		encode_size_to_lsb(extn_size, src);				 // the encode_size_to_lsb is called
		fwrite(src, sizeof(char), 32, fptr_stego_image); // fwrite is called and from the src to fptr_stego_image the data copied
		return e_success;
	}

	// encode_secret_file_data the data is encoded
	Status encode_secret_file_data(EncodeInfo * encInfo)
	{
		rewind(encInfo->fptr_secret);
		char sec_data[encInfo->size_secret_file]; // char array
		// printf("size = %d\n", encInfo -> size_secret_file);
		fread(sec_data, sizeof(char), encInfo->size_secret_file, encInfo->fptr_secret);								   // fread data is read from the fptr_src_image and is stored in sec_data
		encode_data_to_image(sec_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image); // encode_data_to_image is called
		//	fwrite(sec_data,sizeof(char),encInfo -> size_secret_file,encInfo -> fptr_stego_image);
		return e_success;
	}

	// the remaining bytes are copied to the fptr_stego_image
	Status copy_remaining_img_data(FILE * fptr_src, FILE * fptr_dest)
	{
		int ch;
		while ((ch = getc(fptr_src)) != EOF) // it will copy untill the EOF
		{
			putc(ch, fptr_dest);
		}
		return e_success;
	}

	// here the data to the image is encoded with the change
	Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
	{
		char src[8];
		for (int i = 0; i < size; i++)
		{
			fread(src, sizeof(char), 8, fptr_src_image);
			encode_byte_to_lsb(data[i], src); // encode_byte_to_lsb is called
			fwrite(src, sizeof(char), 8, fptr_stego_image);
		}
		return e_success;
	}

	// encode_byte_to_lsb is called here the lsb bit is changed
	Status encode_byte_to_lsb(char data, char *image_buffer)
	{
		int j = 0;
		for (int i = 7; i >= 0; i--)
		{
			if (data & (1 << i))					   // here it will check the i th bit whether its 1 or 0
				image_buffer[j] = image_buffer[j] | 1; // here LSB set
			else
				image_buffer[j] = image_buffer[j] & (~1); // here the LSB cleared
			j++;
		}
		return e_success;
	}

	// function for  encoding
		Status encode_size_to_lsb(int size, char *image_buffer)
	{
		int j = 0;
		for (int i = 31; i >= 0; i--)
		{
			if (size & (1 << i))					   // here it will check the i th bit whether its 1 or 0
				image_buffer[j] = image_buffer[j] | 1; // here LSB set
			else
				image_buffer[j] = image_buffer[j] & (~1); // here the LSB cleared
			j++;
		}
		return e_success;
	}

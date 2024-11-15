/*
Name:L.Mohammed Inayath
Date:08/02/2024
Description:Steganography
Sample Input: 
gcc test_encode.c encode.c decode.c
./a.out -e beautiful.bmp secret.txt output.bmp
./a.out -d output.bmp final.txt
*/

// All the header files are added 

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

int main(int argc, char *argv[]) // the argc and argv are used access the command line arguments
{
	// the structure for decode and encode are include its written in encode and decode header files
	EncodeInfo encInfo;  
	DecodeInfo decInfo;
	// enum char Status and OperationType are defined in the types.h file 
	int type = check_operation_type (argv);  
	if (type == e_unsupported)
		printf ("invalid option\nusage: ->./a.out -e or -d\n");
	else if (type == e_encode) // Encode operation is done here 
	{
		printf ("Started Encoding\n");
		if (argc == 4 || argc == 5) // in Command line arguments count should be 4 or 5 if its less or more than its a wrong i/p 
		{
			//printf ("read_and_validate_decode_args \n");
			if (read_and_validate_encode_args (argv,&encInfo) == e_success) // this will check all the files passed in the cla are correct or not 
				if (do_encoding(&encInfo) == e_success) // if correct then the encoding will start
				{
					printf ("The encoding is done successfully.\n");
					return 0;
				}
		}
		else
			printf ("Pass the command line arguments correctly.\n");
	}
	else if (type == e_decode) // Decode operationis done here
	{
		printf ("Started Decoding\n");
		if (argc == 3 || argc == 4) // in Command line arguments count should be 3 or 4 if its less or more than its a wrong i/p 
		{
			if (read_and_validate_decode_args (argv,&decInfo) == e_success) // this will check all the files passed in the cla are correct or not 
			{
				if (do_decoding(argv,&decInfo) == e_success) // if correct then the decoding will start
				{
					printf ("The decoding is done successfully.\n");
					return 0;
				}
				else
					printf ("Error in do decoding\n");
			}
			else
				printf ("Error in read_and_validate_decode_args\n");
		}
		else
			printf ("Pass the command line arguments correctly.\n");
	}
}
// the check_operation_type will check what to do encode or decode from the cla 
OperationType check_operation_type(char *argv[])
{
	if (argv[1] == NULL)
		return e_unsupported;
	else
	{
		if (strcmp(argv[1],"-e")==0)
			return e_encode;
		else if (strcmp(argv[1],"-d")==0)
			return e_decode;
		else 
			return e_unsupported;
	}
}

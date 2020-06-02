#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR                                                   \
	fprintf (stderr, "ERROR at %s:%d.\n", __FILE__, __LINE__) ;   \
	return -1 ;                                                   \

unsigned long long before, after, after1, before1;
unsigned long long times();

void filter (unsigned char * M, unsigned char * W, int width, int height);/*{
	
	int k[3][3] = {
	 {-1, -1, 0},
	 {-1, 0, 1},
	 {0, 1, 1}
	};
	int h1= height-1;
	int w1 = width-1;
	for (int h=0; h<height; h++){
	
		for(int w=0; w<width ;w++){
			
	
			W[(h+1)*width +(w+1)] = (M[h*width+w]*k[0][0] + M[h*width+(w+1)]*k[0][1] + 
			M[h*width+(w+2)]*k[0][2] + M[(h+1)*width+w]*k[1][0] + 
			M[(h+1)*width+(w+1)]*k[1][1] + M[(h+1)*width+(w+2)]*k[1][2] + 
			M[(h+2)*width+w]*k[2][0] + M[(h+2)*width+w+1]*k[2][1]
			+ M[(h+2)*width+(w+2)]*k[2][2] +765)/6;

		}
	}
	
	 
}
*/

int main (int argc, char ** argv)
{
	
	if (2 != argc)
	{
		printf ("\nUsage:\n\n%s file_name.png\n\n", argv[0]) ;

		return 0 ;
	}

	const char * file_name = argv [1] ;
	
	#define HEADER_SIZE (1)
	unsigned char header [HEADER_SIZE] ;

	FILE *fp = fopen (file_name, "rb");
	if (NULL == fp)
	{
		fprintf (stderr, "Can not open file \"%s\".\n", file_name) ;
		ERROR
	}

	if (fread (header, 1, HEADER_SIZE, fp) != HEADER_SIZE)
	{
		ERROR
	}

	if (0 != png_sig_cmp (header, 0, HEADER_SIZE))
	{
		ERROR
	}

	png_structp png_ptr = 
		png_create_read_struct
			(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if (NULL == png_ptr)
	{
		ERROR
	}

	png_infop info_ptr = png_create_info_struct (png_ptr);
	if (NULL == info_ptr)
	{
		png_destroy_read_struct (& png_ptr, NULL, NULL);

		ERROR
	}

	if (setjmp (png_jmpbuf (png_ptr))) 
	{
		png_destroy_read_struct (& png_ptr, & info_ptr, NULL);

		ERROR
	}

	png_init_io       (png_ptr, fp);
	png_set_sig_bytes (png_ptr, HEADER_SIZE);
	png_read_info     (png_ptr, info_ptr);

	png_uint_32  width, height;
	int  bit_depth, color_type;
	
	png_get_IHDR
	(
		png_ptr, info_ptr, 
		& width, & height, & bit_depth, & color_type,
		NULL, NULL, NULL
	);

	if (8 != bit_depth)
	{
		ERROR
	}
	if (0 != color_type)
	{
		ERROR
	}

	size_t size = width;
	size *= height ;

	unsigned char * M = malloc (size) ;

	png_bytep ps [height] ;
	ps [0] = M ;
	for (unsigned i = 1 ; i < height ; i++)
	{
		ps [i] = ps [i-1] + width ;
	}
	png_set_rows (png_ptr, info_ptr, ps);
	png_read_image (png_ptr, ps) ;
	
	
	printf 
	(
		"Image %s loaded:\n"
		"\twidth      = %lu\n"
		"\theight     = %lu\n"
		"\tbit_depth  = %u\n"
		"\tcolor_type = %u\n"
		, file_name, width, height, bit_depth, color_type
	) ;
	
	unsigned char * W = malloc (size);
	
	unsigned long long time1,time2;
	time1=times();
	filter (M, W, width, height);
	time2=times();
	printf("Czas wykonywania 1 piksela: %lu\n",time2-time1);


	png_structp write_png_ptr =
		png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (NULL == write_png_ptr)
	{
		ERROR
	}

	for (unsigned i = 0 ; i < height ; i++)
	{
		ps [i] += W - M ;
	}
	png_set_rows (write_png_ptr, info_ptr, ps);

	FILE *fwp = fopen ("out.png", "wb");
	if (NULL == fwp)
	{
		ERROR
	}

	png_init_io   (write_png_ptr, fwp);
	png_write_png (write_png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	fclose (fwp);

	return 0;
}

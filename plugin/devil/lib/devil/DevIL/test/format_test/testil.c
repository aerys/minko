#include <IL/il.h>

/*  This program (testil) is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#define _USE_MATH_DEFINES  // Have to add for MSVC++ to use M_PI properly.
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef WORDS_BIGENDIAN
enum colors {ALPHA, BLUE, GREEN, RED};
#else /* not WORDS_BIGENDIAN */
enum colors {RED, GREEN, BLUE, ALPHA};
#endif /* not WORDS_BIGENDIAN */

/* We would need ILU just because of iluErrorString() function... */
/* So make it possible for both with and without ILU!  */
#ifdef ILU_ENABLED
#include <IL/ilu.h>
#define ERROR_LOADING_FILE_MACRO(filename, code) fprintf(stderr, "Error loading file '%s'\nReason: %s\n", (filename), iluErrorString((code)))
#define ERROR_SAVING_FILE_MACRO(filename, code) fprintf(stderr, "Error saving file '%s'\nReason: %s\n", (filename), iluErrorString((code)))
#else /* not ILU_ENABLED */
#define ERROR_LOADING_FILE_MACRO(filename, code) fprintf(stderr, "Error loading file '%s'\nError code: 0x%X\n", (filename), (unsigned int)(code))
#define ERROR_SAVING_FILE_MACRO(filename, code) fprintf(stderr, "Error saving file '%s'\nError code: 0x%X\n", (filename), (unsigned int)(code))
#endif /* not ILU_ENABLED */

/** How did the tests ended? */
enum test_results {TEST_OK = 0, TEST_FAIL = 0x1, TEST_FAIL_QUANTIL = 0x2, TEST_FAIL_INTEGRAL = 0x4 };

/** Parsing and parsing results (Parameters.flags) related: What options were passed? */
enum {ACTION_HELP = 0x1, ACTION_VERBOSE = 0x2, ACTION_PRESERVE_TESTFILE = 0x4, ACTION_ROUGH_MODE = 0x8, ACTION_TEST_ALPHA = 0x10 };
/** Parsing only: What sort of options to expect? */ 
enum {EXPECT_EXTENSION = 0x1, EXPECT_QUANTILE = 0x2, EXPECT_FILENAME = 0x4, EXPECT_RESOLUTION = 0x8, EXPECT_IMAGES = 0x10};
/** What to test for? */
enum {TEST_NOTHING, TEST_EXTENSION, TEST_IS_TEST_IMAGE, TEST_IMAGES_ARE_SAME};

int compare_ILubyte (const void * a, const void * b)
{ return ( *(ILubyte * )a - *(ILubyte * )b ); }

/*
struct PIXEL
{
	ILubyte color[4];
};
typedef struct PIXEL pixel;*/

/** What we collect from the comand line...
 */
struct parameters
{
	/* ** Input parameters ** */
	/** In the case we want to test filename */
	char first_filename[64];
	/** In the case we want to compare two filenames */
	char second_filename[64];
	/** or test extension... */
	char * extension;

	/*** Tests parameters ***/
	/** how talkative should we be? */
	int verbose;
	/** what about preserve-testfile, rough mode? */
	int flags;
	/* How many bytes per pixel? */
	int bpp;
	/** If we want to tweak test image sizes... */
	int resolution[2];
	/** Quantile test specs */
	double quantile_spec;
	int quantile_treshold;
	/** Integral test specs */
	double integral_treshold;
};
typedef struct parameters Parameters;

/** Function generates test pattern when given data and dimensions
 * \param data Image data. 32bits per pixel
 * \param w, h Width and height of the image
 */
void generate_test_image(ILubyte * data, int w, int h, Parameters params)
{
	int test_alpha = params.flags & ACTION_TEST_ALPHA;
	int bpp = params.bpp;

	int i, j, index, j_index;
	double red, green, blue, sum;
	/* For easier notations */
	int h_5 = h / 5;
	for (j = 0, index = 0; j < h_5 * 3; j++)
		for (i = 0; i < w * bpp; i += bpp) /* We jump bpp bytes ahead with each pixel */
		{/* Making 27 samples from the RGB cube. */
			index = 9 * (j / h_5) + (i * 9) / w / bpp;
			j_index = j * w * bpp; /* just to make stuff look nice */
			data[j_index + i + RED] = 255 / 6 + (255 / 3) * (index % 3) ;
			data[j_index + i + GREEN] = 255 / 6 + (255 / 3) * ((index % 9) / 3 );
			data[j_index + i + BLUE] = 255 / 6 + (255 / 3) * (index / 9);
			if (test_alpha)
				data[j_index + i + ALPHA] = 255 - 63 * ((j * 4 / h_5) % 4);
		}
	for (j = 3 * h_5; j < 4 * h_5; j++)
		for (i = 0; i < w * bpp; i += bpp)
		{/* Making color gradient */
			red = cos(2 * M_PI / (double)w * i / bpp) / 2.0 + 0.5;
			green = cos(2 * M_PI / (double)w * i / bpp + M_PI * 1.0 / 3.0) / 2.0 + 0.5;
			blue = cos(2 * M_PI / (double)w * i / bpp + M_PI * 2.0 / 3.0) / 2.0 + 0.5;
			sum = sqrt(red * red + green * green + blue * blue);
			j_index = j * w * bpp;
			data[j_index + i + RED] = 255 * red / sum;
			data[j_index + i + GREEN] = 255 * green / sum;
			data[j_index + i + BLUE] = 255 * blue / sum;
			if (test_alpha)
				data[j_index + i + ALPHA] = 255;
		}
	for (j = 4 * h_5; j < 5 * h_5; j++)
		for (i = 0; i < w * bpp; i += bpp)
		{/* Making smooth B&W gradient */
			j_index = j * w * bpp;
			data[j_index + i + RED] = (i / bpp * 255) / w;
			data[j_index + i + GREEN] = (i / bpp * 255) / w;
			data[j_index + i + BLUE] = (i / bpp * 255) / w;
			if (test_alpha)
				data[j_index + i + ALPHA] = 255;
		}
}

/** Functions saves the test image of given dimensions and given name
 * \param name Filename
 * \param w, h Image dimensions
 *
 * The image is defined in the generate_test_image function and this function just calls it...
 */
int save_test_image(const char * name, int w, int h, Parameters params)
{
	/* Set constants so that there are no div rounding errors */
	ILuint handle;
	/* Generate the paperwork for our test image... */
	ilGenImages(1, & handle);
	ilBindImage(handle);
	/* how much memory will we need? */
	int memory_needed = w * h * params.bpp;
	ILubyte * data = (ILubyte * )malloc(memory_needed);
	if (data == NULL)
	{
		fprintf(stderr, "Out of memory, %s:%d\n", __FILE__, __LINE__);
		return IL_OUT_OF_MEMORY;
	}

	generate_test_image(data, w, h, params);

	/* finally set the image data */
	ilTexImage(w, h, 1, params.bpp, ( (params.flags & ACTION_TEST_ALPHA) == 0 ? IL_RGB : IL_RGBA), IL_UNSIGNED_BYTE, data);
	/* and dump them to the disc... */
	ILboolean saved = ilSaveImage(name);
	int return_value = IL_NO_ERROR;
	if (saved == IL_FALSE)	
	{
		return_value = ilGetError();
		ERROR_SAVING_FILE_MACRO(name,return_value);
	}
	/* Finally, clean the mess! */
	ilDeleteImages(1, & handle);
	free(data);
	return return_value;
}

/** Returns the sum value of quantile of absolute differences among subpixels.
 * \param reference Left hand side image  (LHS == RHS ?)
 * \param sample Right hand side image 
 * \param num_subpixels BPP of image, usually 4 (RGBA)
 * \param quantile What quantile value are we interested in
 * \return Value of the quantile
 *
 * First of all, absolute differences are computed.
 * Then they are stored in an array and sorted using quicksort.
 * Finally, the quantile is determined and returned.
 */
int calculate_median_pixel_distance_abs(ILubyte * reference, ILubyte * sample, int num_subpixels, double quantile)
{
	ILubyte * differences = (ILubyte *)malloc(sizeof(ILubyte) * num_subpixels);	
	if (differences == NULL)
	{
		fprintf(stderr, "Out of memory, %s:%d\n", __FILE__, __LINE__);
		return IL_OUT_OF_MEMORY;
	}
	int ii, difference;
	for (ii = 0; ii < num_subpixels; ii++)
	{/* Going through all pixel components and assigning the absolute difference to differences array */
		difference = (int)sample[ii] - reference[ii];	/* avoiding: overflow + need to have abs() */
		differences[ii] = (difference >= 0) ? difference : - difference;
	}
	/* We need the quantile, so we have to sort the array */
	qsort (differences, num_subpixels, sizeof(ILubyte), compare_ILubyte);
	if (quantile > 1.0 || quantile < 0.0)	/* The quantile has totally wrong value... */
	{
		fprintf(stderr, "Come on... Quantile should be between 0.0 and 1.0 (you wanted %.3f)\n", quantile);
		return -1;
	}
	int index = (int)(quantile * num_subpixels);	/* Which member of the sorted array is the quantile? */
	int result = differences[index];
	free(differences);	/* Clean the mess */
	differences = 0;
	return result;
}

/** Returns the sum of squared differences between component color values, normed.
 * 
 */
double calculate_averaged_distance_sqr_integral(ILubyte * reference, ILubyte * sample, int num_subpixels)
{
	long long differences = 0;
	int ii, difference;
	for (ii = 0; ii < num_subpixels; ii++)
	{/* Going through all pixel components and assigning the absolute difference to differences array */
		difference = sample[ii] - reference[ii];	/* avoiding overflow + need to have abs() */
		differences += difference * difference;
	}
	return sqrt(differences / (double)num_subpixels);
}

/** Do the comparison and return results through parameters.
 */
void compare_fields(int w, int h, ILubyte * sample, ILubyte * reference, Parameters params, int * quantile, double * integral)
{	
	* quantile = calculate_median_pixel_distance_abs(reference, sample, w *  h * params.bpp, params.quantile_spec);
	* integral = calculate_averaged_distance_sqr_integral(reference, sample, w *  h * params.bpp);
}

/** We have some results of test of similarity of two images. Are they the same?
 * \param filename What filename are we actually testing? (Needed gor report purposes only.)
 * \param quantile What is the value of the quantile we have acquired earlier?
 * \param integral And what is the value of normed integral distance?
 * \return 0 on success or TEST_FAIL or'd with the cause.
 */
int interpret_results(const char * filename, Parameters params, int quantile, double integral)
{
	int return_value = 0;
	if (quantile > params.quantile_treshold)
	{
		printf("Quantile test failed: More than %d%% of subpixels have greater error than %d (should be less than %d)\n", (int)(100 - 100 * params.quantile_spec), quantile, params.quantile_treshold);
		return_value |= TEST_FAIL_QUANTIL | TEST_FAIL;
	} 	
	
	if (integral > params.integral_treshold)
	{
		printf("Integral test failed: Deviation is %.3f, should be below %.3f\n", integral, params.integral_treshold);
		return_value |= TEST_FAIL_INTEGRAL | TEST_FAIL;
	}

	if (params.verbose > 0)
		printf("Testing file '%s'. \nQuantile[%.3f]: %d (max %d)\nIngegral: %f (max %f)\n", filename, params.quantile_spec, quantile, params.quantile_treshold, integral, params.integral_treshold);

	return return_value;
}

/** Is the passed params.first_filename a test image?
 * Useful for testing whether the various format load functions alre OK
 * if we can't save the test image in DevIL.
 * So we take some test image, convert it to another format in some exernal editor
 * and then we try to load it. Smart, isn't it? :-)
 */
int test_is_testimage(Parameters params)
{	
	/* First, we load the generated and saved image */
	ILuint handle;
	ilGenImages(1, & handle);
	ilBindImage(handle);
	int loaded = ilLoadImage(params.first_filename);
	if (loaded == IL_FALSE)
	{/* something went wrong */
		ERROR_LOADING_FILE_MACRO(params.first_filename, loaded);
		return TEST_FAIL;
	}
	/* getting image width and height */
	int w, h;
	w = ilGetInteger(IL_IMAGE_WIDTH);  
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	/* Then we save image data into arrays that we allocate now */
	ILubyte * reference, * sample;
	reference = malloc(w * h * params.bpp * sizeof(ILubyte));
	sample = malloc(w * h * params.bpp * sizeof(ILubyte));
	if (reference == NULL || sample == NULL)
	{
		if ((params.flags & ACTION_PRESERVE_TESTFILE) == 0)
			remove(params.first_filename);
		fprintf(stderr, "Out of memory, %s:%d\n", __FILE__, __LINE__);
		return IL_OUT_OF_MEMORY;
	}
	/* Allocation went fine and now we get what SHOULD BE on the loaded image... */
	generate_test_image((ILubyte *)reference, w, h, params);
	ilCopyPixels(0, 0, 0, w, h, 1, ((params.flags & ACTION_TEST_ALPHA) == 0 ? IL_RGB : IL_RGBA), IL_UNSIGNED_BYTE, sample);

	int quantile;
	double integral;
	compare_fields(w, h, sample, reference, params, & quantile, & integral);
	int return_value = interpret_results(params.first_filename, params, quantile, integral);

	free(sample);
	free(reference);
	/* We do not want to keep the saved test image by default... */
	if ((params.flags & ACTION_PRESERVE_TESTFILE) == 0)
		remove(params.first_filename);
	return return_value;
}

/** Tests whether we can save and load the test data to a specified extension
 * \param params Test options in a nice box
 * \return 0 if all is OK, or an error code (see interpret_results)
 */
int test_format(Parameters params)
{
	/* First, let's generate and save a test image */
	const char * base_name = "test.";
	char filename [64];
	if (strlen(params.extension) + strlen(base_name) >= sizeof(filename))
		return -1; /* buffer overflow */
	sprintf(filename, "%s%s", base_name, params.extension);
	int saved = save_test_image(filename, params.resolution[0], params.resolution[1], params);
	if (saved != 0) /* something went wrong, no point in continuing, the user already knows */
		return saved;
	strncpy(params.first_filename, filename, sizeof(params.first_filename));
	/* Will we be able to load that very test image? */
	return test_is_testimage(params);
}

int test_are_same(Parameters params)
{
	const int images_count = 2;
	ILuint handle[images_count];
	int i;
	ILubyte * image_data[images_count];
	for (i = 0; i < images_count; i++)
		image_data[i] = NULL;
	ilGenImages(images_count, & handle[0]);
	const char * filenames [] = {params.first_filename, params.second_filename};
	int w, h;
	for (i = 0; i < images_count; i++)
	{
		ilBindImage(handle[i]);
		int loaded = ilLoadImage(filenames[i]);
		if (loaded == IL_FALSE)
		{/* something went wrong */
			ERROR_LOADING_FILE_MACRO(filenames[i], loaded);
			for (i--; i >= 0; i--)
			{/* We might allocated something, so let's clean it up :)) */
				free(image_data[i]);
				image_data[i] = NULL;
			}
			return TEST_FAIL;
		}
		/* getting image width and height */
		if (i == 0)
		{
			w = ilGetInteger(IL_IMAGE_WIDTH);  
			h = ilGetInteger(IL_IMAGE_HEIGHT);
		}
		else
		{
			if (w != ilGetInteger(IL_IMAGE_WIDTH)
					|| h != ilGetInteger(IL_IMAGE_HEIGHT))
			{
				printf("Of course that images '%s' and '%s' are not the same. How could they be as they have different dimensions?\n", filenames[0], filenames[i]);
				return -1;
			}
		}
		/* Then we save image data into arrays that we allocate now */
		image_data[i] = malloc(w * h * params.bpp * sizeof(ILubyte));
		if (image_data[i] == NULL)
		{
			fprintf(stderr, "Out of memory, %s:%d\n", __FILE__, __LINE__);
			return IL_OUT_OF_MEMORY;
		}
		ilCopyPixels(0, 0, 0, w, h, 1, ((params.flags & ACTION_TEST_ALPHA) == 0 ? IL_RGB : IL_RGBA), IL_UNSIGNED_BYTE, image_data[i]);
	}
	int return_value = 0, result;
	int quantile;
	double integral;
	if (params.verbose > 0)
		printf("Testing against '%s'\n", filenames[0]);
	for (i = 1; i < images_count; i++)
	{
		compare_fields(w, h, image_data[0], image_data[i], params, & quantile, & integral);
		result = interpret_results(filenames[i], params, quantile, integral);
		return_value = (result > return_value) ? result : return_value;
		free(image_data[i]);
		image_data[i] = 0;
	}
	free(image_data[0]);
	image_data[0] = 0;
	ilDeleteImages(images_count, & handle[0]);
	return return_value;
}

/** Examines the command line like no tomorrow :-)
 *
 * Huge code, I am sorry for that.
 * On the other hand, cross-platformness of popt is catastrophical 
 * and usage of boost is also too much for this simple purpose.
 */
int parse_commandline(int argc, char ** argv, Parameters * params)
{
	int actions = 0, expectations = 0;
	const int long_str = 63;
	int return_value = TEST_NOTHING;

	int i;
	for (i = 1; i < argc; i++)
	{
		actions = 0;
		/* Find out what to do and what to expect */
		if (argv[i][0] == '-')
		{/* Ho, an option was passed... */
			if (argv[i][1] == '-')
			{/* Deal with long options */
				if(strncmp(argv[i], "--extension", long_str))
				{
					expectations |= EXPECT_EXTENSION;
				}
				else if (strncmp(argv[i], "--quantile", long_str))
				{
					expectations |= EXPECT_QUANTILE;
				}
				else if (strncmp(argv[i], "--help", long_str))
				{
					actions |= ACTION_HELP;
				}
				else if (strncmp(argv[i], "--verbose", long_str))
				{
					actions |= ACTION_VERBOSE;
				}
				else if (strncmp(argv[i], "--test-alpha", long_str))
				{
					actions |= ACTION_TEST_ALPHA;
				}
				else if (strncmp(argv[i], "--preserve", long_str))
				{
					actions |= ACTION_PRESERVE_TESTFILE;
				}
				else if (strncmp(argv[i], "--use-resolution", long_str))
				{
					expectations |= EXPECT_RESOLUTION;
				}
				else if (strncmp(argv[i], "--are-same", long_str))
				{
					expectations |= EXPECT_IMAGES;
				}
				else if (strncmp(argv[i], "--image-complies", long_str))
				{
					expectations |= EXPECT_FILENAME;
				}

			}
			else /* if not (argv[i][1] == '-') */
			{/* Deal with short options */
				int j;
				for (j = 1; j < strlen(argv[i]); j++)
					switch(argv[i][j])
					{
						case 'h':
						case '?':
							actions |= ACTION_HELP;
							break;
						case 'v':
							actions |= ACTION_VERBOSE;
							break;
						case 'a':
							actions |= ACTION_TEST_ALPHA;
							break;
						case 'e':
							expectations |= EXPECT_EXTENSION;
							break;
						case 'q':
							expectations |= EXPECT_QUANTILE;
							break;
						case 'p':
							actions |= ACTION_PRESERVE_TESTFILE;
							break;
						case 'r':
							expectations |= EXPECT_RESOLUTION;
							break;
						case 's':
							expectations |= EXPECT_IMAGES;
							break;
						case 'c':
							expectations |= EXPECT_FILENAME;
							break;
					}/* end switch(argv[i][j]) */
			}/* if not (argv[i][1] == '-') */
		} 
		else /* (! argv[i][0] == '-') */
		{/* Now handle expectations... */
			if (expectations & EXPECT_EXTENSION)
			{/* point extension to the extension string so we will be able to read it afterwards */
				params->extension = argv[i];
				return_value = TEST_EXTENSION;
			}
			else if (expectations & EXPECT_QUANTILE)
			{/* This parameter SHOULD BE the quantile we wanted... */
				sscanf(argv[i], "%lf", & params->quantile_spec);
			}
			else if (expectations & EXPECT_RESOLUTION)
			{/* This parameter SHOULD specify the geometry... */
				sscanf(argv[i], "%dx%d", & params->resolution[0], & params->resolution[1]);
			}
			else if (expectations & EXPECT_FILENAME)
			{/* This parameter SHOULD specify one filename... */
				strncpy(params->first_filename, argv[i], sizeof(params->first_filename));
				if ((params->flags & ACTION_PRESERVE_TESTFILE) == 0)
				{
					params->flags |= ACTION_PRESERVE_TESTFILE;
					printf("Issued a command to preserve the input file :-)\n");
				}
				return_value = TEST_IS_TEST_IMAGE;
			}
			else if (expectations & EXPECT_IMAGES)
			{/* Masterpiece, this parameter SHOULD specify two comma separated filenames... */
				sscanf(argv[i], "%64[^,],%64s", params->first_filename, params->second_filename);
				if ((params->flags & ACTION_PRESERVE_TESTFILE) == 0)
				{
					params->flags |= ACTION_PRESERVE_TESTFILE;
					printf("Issued a command to preserve the input file :-)\n");
				}
				return_value = TEST_IMAGES_ARE_SAME;
				/* just debug: * printf ("Acquired strings: %s %s\n", params->first_filename, params->second_filename); **/
 			}
			expectations = 0;
		}/* (! argv[i][0] == '-') */
		/* now handle actions... */
		if (actions & ACTION_VERBOSE)
		{
			params->verbose++;
		}
		if (actions & ACTION_HELP)
		{
			printf(" *** Beware, manually generated help (=> may not be 100%% up-to-date :-) ***\n");
			printf("     If you miss something, examine the source code\n");
			printf("Run %s with this arguments:\n", argv[0]);
			printf("\t-h, -? | --help: This help message\n");
			printf("\t-v | --verbose: Verbose run\n");
			printf("\t-a | --test-alpha: Make and test image alpha channel as well\n");
			printf("\t-e | --extension <extension, like BMP, PNG etc.>: Test saving and loading of this extension\n");
			printf("\t-p | --preserve: Don't remove any generated files\n");
			printf("\t-c | --image-complies <filename, like image.jpg>: Test whether the filename is the image of our test pattern (useful for testing opening capabilities)\n");
			printf("\t-r | --use-resolution <widthxheight, like 400x300>: If this makes sense, use this resolution), default 603x300\n");
			printf("\t-s | --are-same <first,second filename, like test.png,something.jpg>: Tests whether the first and second images are the same. Pass two filenames separated by comma.\n");

			printf("This will be helpful one day...\n");
		}
		if (actions & ACTION_PRESERVE_TESTFILE)
		{
			params->flags |= ACTION_PRESERVE_TESTFILE;
		}
		if (actions & ACTION_TEST_ALPHA)
		{
			params->flags |= ACTION_TEST_ALPHA;
			params->bpp = 4; 
		}
	}/* endfor <going through argv> */
	return return_value;
}

int main(int argc, char ** argv)
{
	/* has to be done */
	ilInit();
#ifdef ILU_ENABLED
	iluInit();
#endif 
	/* Consistent loading stuff... */
	ilEnable(IL_ORIGIN_SET);
	
	Parameters params = { /* ***SHOULD BE*** first_filename*/ "", 
		/* second filename */ "", 
		/* extension */ (char *)NULL, 
		/* verbose */ (int)0, 
		/* flags */(int)0, 
		/* bpp */(int)3, 
		/* resolution */ {603, 300}, 
		/* quantile spec */ (double)0.85, 
		/* quantile tresh */ (int)10, 
		/* integral tresh */ (double)6};

	int to_do = parse_commandline(argc, argv, & params);
	int return_value = 0;
	if (to_do & TEST_EXTENSION && params.extension != NULL)
	{
		return_value = test_format(params);
		if (return_value != 0)
			fprintf(stderr, "Format test: FAIL (.%s)\n", params.extension);
	}
	else if (to_do & TEST_IS_TEST_IMAGE 
			&& strlen(params.first_filename) > 0 
			&& strlen(params.second_filename) == 0)
	{
		if (params.verbose > 0)
			printf ("Testing whether %s is the test image\n", params.first_filename);
		return_value = test_is_testimage(params);
		if (return_value != 0)
			fprintf(stderr, "Is test image test: FAIL (%s)\n", params.first_filename);
	}
	else if (to_do & TEST_IMAGES_ARE_SAME 
			&& strlen(params.first_filename) > 0 
			&& strlen(params.second_filename) > 0 )
	{
		if (params.verbose > 0)
			printf ("Testing whether %s and %s are +- the same\n", params.first_filename, params.second_filename);
		return_value = test_are_same(params);
		if (return_value != 0)
			fprintf(stderr, "Images are the same test: FAIL ('%s' != '%s')\n", params.first_filename, params.second_filename);
	}

	return return_value;
}


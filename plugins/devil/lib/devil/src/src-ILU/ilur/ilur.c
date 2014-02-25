#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include <IL/il.h>
#include <IL/ilu.h>

/*  This program (ilur) is free software: you can redistribute it and/or modify
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

/** What are the classes of types of parameters of ILU functions?
 */
enum Parameter_types {PARAM_VOID, PARAM_ILUINT, PARAM_ILFLOAT, PARAM_OTHERS};

/** The structure wrapped around functions
 */
struct ilu_function
{
	char Name[32];	/**< Name of the function used by user nad help */
	int Parameter_type;	/**< Lots of functions take the same kind of parameters */
	void * Callback;	/**< Terribly type-unsafe... callback :-) */
};
typedef struct ilu_function Ilu_function;

/** What will we be to able to find out during arguments parsing
 */
enum {FLAG_NONE = 0x0, FLAG_HELP = 0x1, FLAG_VERBOSE = 0x2, FLAG_LOAD = 0x4, FLAG_SAVE = 0x8};
enum {ILU_ALIENIFY, ILU_BLURAVG, ILU_BLURGAUSSIAN, ILU_BUILDMIPMAPS, ILU_COMPAREIMAGE, ILU_CONTRAST, ILU_CROP, ILU_EDGEDETECTE, ILU_EDGEDETECTP, ILU_EDGEDETECTS, ILU_EMBOSS, ILU_ENLARGECANVAS, ILU_ENLARGEIMAGE, ILU_EQUALIZE, ILU_CONVOLUTION, ILU_FLIPIMAGE, ILU_GAMMACORRECT, ILU_INVERTALPHA, ILU_MIRROR, ILU_NEGATIVE, ILU_NOISIFY, ILU_PIXELIZE, ILU_REPLACECOLOUR, ILU_ROTATE, ILU_ROTATE3D, ILU_SATURATE1F, ILU_SATURATE4F, ILU_SCALE, ILU_SCALEALPHA, ILU_SCALECOLOURS, ILU_SETLANGUAGE, ILU_SHARPEN, ILU_SWAPCOLOURS, ILU_WAVE, ILU_FUN_COUNT}; 

/* security comes first */
#define short_strlen 32
#define long_strlen 128

char program_name[short_strlen]; /**< How is our executable called? */
Ilu_function ilu_functions[ILU_FUN_COUNT]; /**< What functions are we dealing with? */

/** Just dump infos about functions to the structures so we can somehow automate the processing of user input later...
 */
void init_strings()
{
	ilu_functions[ILU_ALIENIFY] = (Ilu_function){ "iluAlienify", PARAM_VOID, & iluAlienify };
	ilu_functions[ILU_BLURAVG] = (Ilu_function){ "iluBlurAvg", PARAM_ILUINT, & iluBlurAvg };
	ilu_functions[ILU_BLURGAUSSIAN] = (Ilu_function){ "iluBlurGaussian", PARAM_ILUINT, & iluBlurGaussian };
	ilu_functions[ILU_BUILDMIPMAPS] = (Ilu_function){ "iluBuildMipmaps", PARAM_VOID, & iluBuildMipmaps };
	ilu_functions[ILU_COMPAREIMAGE] = (Ilu_function){ "iluCompareImage", PARAM_ILUINT, & iluCompareImage };
	ilu_functions[ILU_CONTRAST] = (Ilu_function){ "iluContrast", PARAM_ILFLOAT, & iluContrast };
	ilu_functions[ILU_CROP] = (Ilu_function){ "iluCrop", PARAM_OTHERS, & iluCrop };
	ilu_functions[ILU_EDGEDETECTE] = (Ilu_function){ "iluEdgeDetectE", PARAM_VOID, & iluEdgeDetectE };
	ilu_functions[ILU_EDGEDETECTP] = (Ilu_function){ "iluEdgeDetectP", PARAM_VOID, & iluEdgeDetectP };
	ilu_functions[ILU_EDGEDETECTS] = (Ilu_function){ "iluEdgeDetectS", PARAM_VOID, & iluEdgeDetectS };
	ilu_functions[ILU_EMBOSS] = (Ilu_function){ "iluEmboss", PARAM_VOID, & iluEmboss };
	ilu_functions[ILU_ENLARGECANVAS] = (Ilu_function){ "iluEnlargeCanvas", PARAM_OTHERS, & iluEnlargeCanvas };
	ilu_functions[ILU_ENLARGEIMAGE] = (Ilu_function){ "iluEnlargeImage", PARAM_OTHERS, & iluEnlargeImage };
	ilu_functions[ILU_EQUALIZE] = (Ilu_function){ "iluEqualize", PARAM_VOID, & iluEqualize };
	ilu_functions[ILU_CONVOLUTION] = (Ilu_function){ "iluConvolution", PARAM_OTHERS, & iluConvolution };
	ilu_functions[ILU_FLIPIMAGE] = (Ilu_function){ "iluFlipImage", PARAM_VOID, & iluFlipImage };
	ilu_functions[ILU_GAMMACORRECT] = (Ilu_function){ "iluGammaCorrect", PARAM_ILFLOAT, & iluGammaCorrect };
	ilu_functions[ILU_INVERTALPHA] = (Ilu_function){ "iluInvertAlpha", PARAM_VOID, & iluInvertAlpha };
	ilu_functions[ILU_MIRROR] = (Ilu_function){ "iluMirror", PARAM_VOID, & iluMirror };
	ilu_functions[ILU_NEGATIVE] = (Ilu_function){ "iluNegative", PARAM_VOID, & iluNegative };
	ilu_functions[ILU_NOISIFY] = (Ilu_function){ "iluNoisify", PARAM_ILFLOAT, & iluNoisify };
	ilu_functions[ILU_PIXELIZE] = (Ilu_function){ "iluPixelize", PARAM_ILUINT, & iluPixelize };
	ilu_functions[ILU_REPLACECOLOUR] = (Ilu_function){ "iluReplaceColour", PARAM_OTHERS, & iluReplaceColour };
	ilu_functions[ILU_ROTATE] = (Ilu_function){ "iluRotate", PARAM_ILFLOAT, & iluRotate };
	ilu_functions[ILU_ROTATE3D] = (Ilu_function){ "iluRotate3D", PARAM_OTHERS, & iluRotate3D };
	ilu_functions[ILU_SATURATE1F] = (Ilu_function){ "iluSaturate1f", PARAM_ILFLOAT, & iluSaturate1f };
	ilu_functions[ILU_SATURATE4F] = (Ilu_function){ "iluSaturate4f", PARAM_OTHERS, & iluSaturate4f };
	ilu_functions[ILU_SCALE] = (Ilu_function){ "iluScale", PARAM_OTHERS, & iluScale };
	ilu_functions[ILU_SCALEALPHA] = (Ilu_function){ "iluScaleAlpha", PARAM_ILFLOAT, & iluScaleAlpha };
	ilu_functions[ILU_SCALECOLOURS] = (Ilu_function){ "iluScaleColours", PARAM_OTHERS, & iluScaleColours };
	ilu_functions[ILU_SETLANGUAGE] = (Ilu_function){ "iluSetLanguage", PARAM_OTHERS, & iluSetLanguage };
	ilu_functions[ILU_SHARPEN] = (Ilu_function){ "iluSharpen", PARAM_OTHERS, & iluSharpen };
	ilu_functions[ILU_SWAPCOLOURS] = (Ilu_function){ "iluSwapColours", PARAM_VOID, & iluSwapColours };
	ilu_functions[ILU_WAVE] = (Ilu_function){ "iluWave", PARAM_ILFLOAT, & iluWave };
}

/** What is worthy to remember from what the user can tell us...
 */
struct params
{
	char Load_filename[long_strlen];	///< Where to get the image
	char Save_filename[long_strlen];	///< And where to dump it in the end

	int Flags;	///< verbose, help, etc. 
	int Calls_count;	///< How many ILU functions are we going to apply to the image?
	char ** Calls_strings;	///< How did the user specified the function calls?
};
typedef struct params Params;

/** Create a new Params structure in the space and return a pointer to it
 */
Params * create_params()
{
	Params * ret_val = (Params *)malloc(sizeof(Params));
	ret_val->Load_filename[0] = '\0';
	ret_val->Save_filename[0] = '\0';
	ret_val->Flags = 0;
	ret_val->Calls_count = 0;
	ret_val->Calls_strings = NULL;
	return ret_val;
}

/** What was created, must be destroyed!
 */
void destroy_params(Params * to_destroy)
{
	int i;
	for(i = 0; i < to_destroy->Calls_count; i++)
	{/* The main thing to free here are the call strings */
		free(to_destroy->Calls_strings[i]);
		to_destroy->Calls_strings[i] = NULL;
	}
	/* Then free the call strings container */
	free(to_destroy->Calls_strings);
	to_destroy->Calls_strings = NULL;
	/* And let's remember that the parameter itself has to be freed */
	free(to_destroy);
	to_destroy = NULL;
}

/** This function receives the raw string passed from the command line
 * \param string The passed string
 * \param name The extracted name of the function
 * \param params The extracted string between brackets
 */
int parse_function(const char * string, char * name, char * params)
{
	int i;
	/* num of whitespaces in front of the function */
	int num_front_whitespaces = 0;
	/* num of character that bears the first parameter char - beyond '(' */
	int in_parameters = 0;
	/* trim the leading whitespaces */
	for (i = 0; i < long_strlen - 1; i++)
		if(string[i] == ' ' || string[i] == '\t')
			num_front_whitespaces++;
		else
			break;
	/* copy the function name */
	for (; i < long_strlen - 1 && string[i] != '('; i++)
		if(string[i] == ' ' || string[i] == '\t')
			break;
		else
			name[i - num_front_whitespaces] = string[i];
	/* terminate the string */
	name[i - num_front_whitespaces] = '\0';
	/* finally get the parameter */
	for (; i < long_strlen - 1 && string[i] != ')'; i++)
		if(string[i] == '(')
		{
			in_parameters = i + 1;
			continue;
		}
		else
		{
			if(in_parameters != 0)
				params[i - in_parameters] = string[i];
		}
	/* again terminate the string */
	params[i - in_parameters] = '\0';
	return 0;
}

/** Takes a string and removes whitespaces within
 * \param string The input string
 * \param nonwhitespaced The 'cleaned' string
 */
int remove_whitespaces(const char * string, char * nonwhitespaced)
{
	strncpy(nonwhitespaced, string, strlen(string) + 1);
	int i;
	/* How many whitespace chars were skipped? */
	int num_whitespaced = 0;
	/* Go from the beginning and don't stop until the either end */
	for (i = 0; string[i] != '\0' && i < long_strlen; i++)
		/* Ho, a whitespace that shouldn't be missed! */
		if( string[i] == ' ' || string[i] == '\t' )
		{
			num_whitespaced++;
			continue;
		}
		else /* No whitespace, moreover we are already inside the string... */
		{
			/* Here we copy the stuff to the output parameter */
			nonwhitespaced[i - num_whitespaced] = string[i];
		}
}

/** How to fill our Params structure?
 */
int parse_arguments(int argc, const char * argv[], Params * parameters)
{
	/* How many ILU functions are we going to apply to the image? */
	int calls_count = 0;
	/* Let's store their corresponding indexes in argv... */
	int * fun_to_call = (int *)malloc(argc / 2 * sizeof(int));

	int i;
	for (i = 1; i < argc; i++)
	{
		/* Find out what to do and what to expect */
		if (argv[i][0] == '-')
		{/* Ho, an option was passed... */
			if (argv[i][1] == '-')
			{/* Deal with long options */
				if(strncmp(argv[i], "--apply", long_strlen))
					goto apply; /* Yeah, there are GOTO's here :-) */
				else if(strncmp(argv[i], "--load-from", long_strlen))
					goto load_from;
				else if(strncmp(argv[i], "--save-to",   long_strlen))
					goto save_to;
				else if(strncmp(argv[i], "--verbose",   long_strlen))
					goto verbose;
			}
			else switch(argv[i][1]) /* Well, maybe it wasn't a long option :-) */
			{/* Deal with long options */
				case 'h':
				case '?':
					help:
					parameters->Flags |= FLAG_HELP;
					break;
				case 'a':
					apply:
					if (argc > i + 1) /* that there is maybe something like the parameter out there... */
						fun_to_call[calls_count++] = i + 1;
					break;
				case 'l':
					load_from:
					if (argc > i + 1) 
					{/* that there is maybe something like the parameter out there... */
						strncpy(parameters->Load_filename, argv[i + 1], long_strlen);
						parameters->Flags |= FLAG_LOAD;
					}
					break;
				case 's':
					save_to:
					if (argc > i + 1)
					{/* that there is maybe something like the parameter out there... */
						strncpy(parameters->Save_filename, argv[i + 1], long_strlen);
						parameters->Flags |= FLAG_SAVE;
					}
					break;
				case 'v':
					verbose:
					parameters->Flags |= FLAG_VERBOSE;
					break;
			}
			/* We don't use complicated parameters here, so no worries */
		}/* endif (argv[i][0] == '-') */
	}/* endif (argv[i][0] == '-') */
	/* let's save the valuable info to the output structure... */
	parameters->Calls_count = calls_count;
	/* and let's also store the calls as passed by the user */
	parameters->Calls_strings = (char **)malloc(parameters->Calls_count * sizeof (char *));
	for (i = 0; i < calls_count; i++)
	{
		/* Yeah, there is probably more memory allocated than needed... */
		parameters->Calls_strings[i] = (char *)malloc(sizeof(char) * long_strlen);
		strncpy(parameters->Calls_strings[i], argv[ fun_to_call[i] ], long_strlen);
	}

	/* clean the mess... */
	free(fun_to_call);
	fun_to_call = 0;
}

void print_help()
{
	printf(" *** Beware, manually generated help (=> may not be 100%% up-to-date :-) ***\n");
	printf("\tTip: If you miss something, examine the source code\n\tNext tip: You can't stack the short options. Sorry.\n");
	printf("Run %s with this arguments:\n", program_name);
	printf("\t-h, -? | --help: This help message\n");
	printf("\t-v | --verbose: Verbose run\n");
	printf("\t-l | --load_from <filename, like subject.png>: The filename of an image that will be loaded and played with\n");
	printf("\t-s | --save-to <filename, like result.jpg>: The filename of the result\n");
	printf("\t-a | --apply <C-styled function call, like iluBlurAvg(6)>: The operation to run. Beware of the braces, they annoy most shells, so you need to either enclose the parameter in quotation marks (recommended), or escape them (not recommended since it is clumsy)\n");
	printf("\tFunctions will be applied in order you have specified them, that is from left to right.\n");
	printf(" Functions we know of: ");
	int i;
	for (i = 0; i < ILU_FUN_COUNT - 1; i++)
		printf("%s, ", ilu_functions[i].Name);
	printf("%s.\n", ilu_functions[ILU_FUN_COUNT - 1].Name);
	printf("\nExample call:\n\t%s -l source_image.png -s result_image.jpg -a 'iluAlienify()' -a 'iluContrast(0.8)'\n", program_name);
}

/**
 * Assumed that the right image is bound to IL
 */
int perform_operation(const char * operation, int verbose)
{
	/* Where to store the first parsing results? */
	char function[long_strlen], params[long_strlen], solid_params[long_strlen];
	/* Get the function name string and parameters string */
	parse_function(operation, function, params);
	/* Get rid of any whitespaces from the parameters string */
	remove_whitespaces(params, solid_params);
	if (verbose)
		printf("Calling %s(%s)\n", function, solid_params);
	/* What function was wanted? -1 means that we don't know */
	int function_index = -1;
	int i;
	for (i = 0; i < ILU_FUN_COUNT; i++)
		if (strncmp(function, ilu_functions[i].Name, short_strlen) == 0)
		{/* Yeah, this function was wanted. Let's have its index from the ilu_functions array */
			function_index = i;
			break;	/* nothing to do here any more */
		}
	if (function_index == -1)
	{/* Seems we haven't found anything... */
		fprintf(stderr, "Error: You have specified an invalid function name '%s' (have you called %s).\nRun '%s --help' command to get some help\n", function, operation, program_name);
		return 1;
	}
	/* We are going to try something and we want to know how it ended */
	ILboolean return_value;
	switch (ilu_functions[function_index].Parameter_type)
	{/* First semi-automatic processing according to type of parameters */
		case PARAM_VOID:
			{
				ILboolean (* function)() = ilu_functions[function_index].Callback;
				return_value = function();
				break;
			}/* endcase PARAM_VOID */
		case PARAM_ILUINT:
			{
				/* first assign and determine the type of the Callback */
				ILboolean (* function)(ILuint) = ilu_functions[function_index].Callback;
				/* then declare the parameter variables */
				ILuint param_value;
				/* fill them */
				int success = sscanf(solid_params, "%u", & param_value);
				if (success != 1)
				{/* see how it ended */
					fprintf(stderr, "Error interpreting '%s' as unsigned integer (when calling %s)\n", solid_params, operation);
					break;
				}
				/* execute the command and store the result */
				return_value = function(param_value);
				break;
			}/* endcase PARAM_ILUINT */
		case PARAM_ILFLOAT:
			{
				ILboolean (* function)(ILfloat) = ilu_functions[function_index].Callback;
				double param_value;
				int success = sscanf(solid_params, "%lf", & param_value);
				if (success != 1)
				{
					fprintf(stderr, "Error interpreting '%s' as float (when calling %s)\n", solid_params, operation);
					break;
				}
				return_value = function((ILfloat)param_value);
				break;
			}/* endcase PARAM_ILFLOAT */
		case PARAM_OTHERS:
			switch (function_index)
			{/* next, the manual processing according to names */
				case ILU_SHARPEN:
					{
						ILboolean (* function)(ILfloat, ILuint) = ilu_functions[function_index].Callback;
						double factor; 
						ILuint iter;
						int success = sscanf(solid_params, "%lf,%u", & factor, & iter);
						if (success != 2)
						{
							fprintf(stderr, "Error interpreting '%s' as floating-point number and unsigned integer separated by comma (when calling %s)\n", solid_params, operation);
							break;
						}
						return_value = function((ILfloat)factor, iter);
						break;
					}/* endcase ILU_SHARPEN */
				case ILU_CROP:
					{
						ILboolean (* function)(ILuint, ILuint, ILuint, ILuint, ILuint, ILuint ) = ilu_functions[function_index].Callback;
						ILuint xoff, yoff, zoff, width, height, depth;
						int success = sscanf(solid_params, "%u,%u,%u,%u,%u,%u", & xoff, & yoff, & zoff, & width, & height, & depth);
						if (success != 6)
						{
							fprintf(stderr, "Error interpreting '%s' as 6 unsigned integers separated by comma (when calling %s)\n", solid_params, operation);
							break;
						}
						return_value = function(xoff, yoff, zoff, width, height, depth);
						break;
					}/* endcase ILU_CROP */
				case ILU_ENLARGECANVAS:
				case ILU_SCALE:
					{
						ILboolean (* function)(ILuint, ILuint, ILuint) = ilu_functions[function_index].Callback;
						ILuint width, height, depth;
						int success = sscanf(solid_params, "%u,%u,%u", & width, & height, & depth);
						if (success != 3)
						{
							fprintf(stderr, "Error interpreting '%s' as 3 unsigned integers separated by comma (when calling %s)\n", solid_params, operation);
							break;
						}
						return_value = function(width, height, depth);
						break;
					}/* endcase ILU_ENLARGECANVAS + ILU_SCALE */
				case ILU_ENLARGEIMAGE:
				case ILU_SCALECOLOURS:
					{
						ILboolean (* function)(ILfloat, ILfloat, ILfloat) = ilu_functions[function_index].Callback;
						double first, second, third;
						int success = sscanf(solid_params, "%lf,%lf,%lf", & first, & second, & third);
						if (success != 3)
						{
							fprintf(stderr, "Error interpreting '%s' as 3 floating-point numbers separated by comma (when calling %s)\n", solid_params, operation);
							break;
						}
						return_value = function((ILfloat)first, (ILfloat)second, (ILfloat)third);
						break;
					}/* endcase ILU_ENLARGEIMAGE + ILU_SCALECOLOURS */
				case ILU_ROTATE3D:
				case ILU_SATURATE4F:
					{
						ILboolean (* function)(ILfloat, ILfloat, ILfloat, ILfloat) = ilu_functions[function_index].Callback;
						double first, second, third, fourth;
						int success = sscanf(solid_params, "%lf,%lf,%lf,%lf", & first, & second, & third, & fourth);
						if (success != 4)
						{
							fprintf(stderr, "Error interpreting '%s' as 4 floating-point numbers separated by comma (when calling %s)\n", solid_params, operation);
							break;
						}
						return_value = function((ILfloat)first, (ILfloat)second, (ILfloat)third, (ILfloat)fourth);
						break;
					}/* endcase ILU_ROTATE3D + ILU_SATURATE4F */
				case ILU_REPLACECOLOUR:
					{
						ILboolean (* function)(ILubyte, ILubyte, ILubyte, ILfloat ) = ilu_functions[function_index].Callback;
						ILuint red, green, blue;
						double tolerance;
						int success = sscanf(solid_params, "%u,%u,%u,%lf", & red, & green, & blue, & tolerance);
						if (success != 4)
						{
							fprintf(stderr, "Error interpreting '%s' as 3 8-bit unsigned integers and one floating-point number separated by comma (when calling %s)\n", solid_params, operation);
							break;
						}
						return_value = function((ILubyte)red, (ILubyte)green, (ILubyte)blue, (ILfloat)tolerance);
						break;
					}/* endcase ILU_ROTATE3D + ILU_SATURATE4F * */
				/* iluConvolution(ILint *matrix, ILint scale, ILint bias); Any idea about this? */ 
			}/* endswitch(function_index) */
			break;

	}/* endswitch (ilu_functions[function_index].Parameter_type) */
	/* It didn't end good for some reason... */
	if (return_value == IL_FALSE)
	{
		int error= ilGetError();
		fprintf(stderr, "Something got wrong when calling %s(%s): %s\n", function, solid_params, iluErrorString(error) );
		return error;
	}
	return 0;
}

int do_stuff(const Params * parameters)
{
	if (parameters->Flags & FLAG_HELP || ((parameters->Flags | FLAG_LOAD |  FLAG_SAVE) != parameters->Flags) )
	{/* We wanted HELP or we did not get SAVE or LOAD */
		print_help(); /* tell the loser what to do, then :-) */
		return 0;
	}
	int verbose = parameters->Flags & FLAG_VERBOSE;

	int image_handle;
	int w, h;
	ILboolean result;

	/* Quite obvious stuff, just load an image */
	ilGenImages(1, & image_handle);
	ilBindImage(image_handle);
	result = ilLoadImage(parameters->Load_filename);
	if (result == IL_FALSE)
	{
		int error = ilGetError();
		fprintf(stderr, "Error: Something went wrong when loading file '%s' (%s)\n", parameters->Load_filename, iluErrorString(error));
		return error;
	}
	/* If we get image's dimensions, people will believe that we have actually loaded something :-) */
	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	if (verbose)
		printf("Loaded '%s', size %dx%d\n", parameters->Load_filename, w, h);
	/* Now let's do our stuff!!! */
	int i;
	for (i = 0; i < parameters->Calls_count; i++)
		perform_operation(parameters->Calls_strings[i], verbose);
	/* our stuff has been done... */

	result = ilSaveImage(parameters->Save_filename);
	if (result == IL_FALSE)
	{
		int error = ilGetError();
		fprintf(stderr, "Error: Something went wrong when saving file '%s' (%s)\n", parameters->Save_filename, iluErrorString(error));
		ilDeleteImages(1, & image_handle);
		return error;
	}
	ilDeleteImages(1, & image_handle);
	return 0;
}

int main(int argc, const char * argv[])
{
	/* Name our little program */
	strncpy(program_name, argv[0], short_strlen);
	/* Prepare ilu functions "database" */ 
	init_strings();
	/* initialize DevIL in order to get defined behavior of the app */
	ilInit();
	iluInit();
	/* Prepare command line parsing */
	Params * parameters = create_params();
	/* Do the parsing */
	parse_arguments(argc, argv, parameters);
	/* Finally do what we wanted */
	do_stuff(parameters);
	/* Clean after the party */
	destroy_params(parameters);
	return 0;
}


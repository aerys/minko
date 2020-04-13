#include <IL/devil_cpp_wrapper.hpp>

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	if (argc < 2) {
		cout << "Please specify a filename." << endl;
		return 1;
	}

	ilImage Image(argv[1]);

	cout << Image.Width() << 'x' << Image.Height() << '@' << (ILuint)Image.Bpp() << endl;

	ilEnable(IL_FILE_OVERWRITE);

	Image.Save("test.tga");

	return 0;
}


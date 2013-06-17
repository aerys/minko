#include "FileLoader.hpp"

#include <fstream>

using namespace minko::file;

std::string
FileLoader::load(const std::string& filename)
{
	std::fstream file(filename, std::ios::in | std::ios::ate);

	if (file.is_open())
	{
		unsigned int size = (unsigned int)file.tellg();
		std::vector<char> content(size + 1);

		content[size] = 0;
		file.seekg(0, std::ios::beg);
		file.read(&content[0], size);
		file.close();

		return std::string(&content[0]);
	}
	else
		throw std::invalid_argument("filename");
}

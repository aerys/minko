#include "Loader.hpp"
#include "Options.hpp"
#include "minko/Signal.hpp"

#include <fstream>

using namespace minko::file;

Loader::Loader() :
	_complete(Signal<Loader::Ptr>::create()),
	_error(Signal<Loader::Ptr>::create())
{
}

void
Loader::load(const std::string& filename, std::shared_ptr<Options> options)
{
	auto flags = std::ios::in | std::ios::ate;

	_options = options;
	
	std::fstream file(filename, flags);

	if (!file.is_open())
		file.open(_options->includePath() + "/" + filename, flags);

	if (file.is_open())
	{
		unsigned int size = (unsigned int)file.tellg();

		_data.resize(size);

		file.seekg(0, std::ios::beg);
		file.read(&_data[0], size);
		file.close();
	
		_complete->execute(shared_from_this());
	}
	else
		_error->execute(shared_from_this());
}

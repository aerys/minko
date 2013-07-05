/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "MkParser.hpp"

using namespace minko::file;
using namespace minko::math;

MkParser::MkParser()
{
}

void
MkParser::parse(const std::string&					filename,
				std::shared_ptr<Options>			options,
				const std::vector<unsigned char>&	data)
{
	_node = scene::Node::create();

	_node->addController(
		controller::SurfaceController::create(
			options->assets()->geometry("cube"),
			data::Provider::create()
					->set("material/diffuse/rgba",	Vector4::create(0.f, 0.f, 1.f, .1f)),
			options->assets()->effect("texture")));
	_node->addController(controller::TransformController::create());
	_node->controller<controller::TransformController>()->transform()->appendTranslation(0.f, 2.f, -3.f);
	std::cout << "parse MK" << std::endl << std::flush;
}

#include "Scene.hpp"

using namespace minko::scene;

void
Scene::render()
{
	_enterFrame->execute(std::dynamic_pointer_cast<Scene>(shared_from_this()));
	_exitFrame->execute(std::dynamic_pointer_cast<Scene>(shared_from_this()));
}
#include "SDLStage.hpp"

bool SDLStage::_active = false;

minko::Signal<>::Ptr SDLStage::_enterFrame = nullptr;
minko::Signal<>::Ptr SDLStage::_keyDown = nullptr;
minko::render::AbstractContext::Ptr	SDLStage::_context = nullptr;

SDL_Window*	SDLStage::_window = 0;

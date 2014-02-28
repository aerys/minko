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

#pragma once

#include "minko/component/ParticleSystem.hpp"

#include "minko/particle/StartDirection.hpp"

#include "minko/particle/modifier/IParticleModifier.hpp"
#include "minko/particle/modifier/Modifier1.hpp"
#include "minko/particle/modifier/Modifier3.hpp"
#include "minko/particle/modifier/StartVelocity.hpp"
#include "minko/particle/modifier/StartForce.hpp"
#include "minko/particle/modifier/StartSize.hpp"
#include "minko/particle/modifier/StartColor.hpp"
#include "minko/particle/modifier/StartRotation.hpp"
#include "minko/particle/modifier/StartAngularVelocity.hpp"
#include "minko/particle/modifier/StartSprite.hpp"
#include "minko/particle/modifier/IParticleUpdater.hpp"
#include "minko/particle/modifier/VelocityOverTime.hpp"
#include "minko/particle/modifier/ForceOverTime.hpp"
#include "minko/particle/modifier/SizeOverTime.hpp"
#include "minko/particle/modifier/ColorOverTime.hpp"
#include "minko/particle/modifier/SizeBySpeed.hpp"
#include "minko/particle/modifier/ColorBySpeed.hpp"

#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/sampler/Constant.hpp"
#include "minko/particle/sampler/RandomValue.hpp"
#include "minko/particle/sampler/LinearlyInterpolatedValue.hpp"

#include "minko/particle/shape/EmitterShape.hpp"
#include "minko/particle/shape/Box.hpp"
#include "minko/particle/shape/Sphere.hpp"
#include "minko/particle/shape/Cone.hpp"
#include "minko/particle/shape/Cylinder.hpp"
#include "minko/particle/shape/Point.hpp"

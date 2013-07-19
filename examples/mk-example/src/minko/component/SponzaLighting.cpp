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

#include "SponzaLighting.hpp"

#include "minko/data/Provider.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"

using namespace minko::component;
using namespace minko::math;

SponzaLighting::SponzaLighting():
	AbstractRootDataComponent(),
	_time(0),
	_offsets(Vector4::create(0.0f, 0.0f, 0.0f, 0.0f)),
	_frequencies(Vector4::create(0.0f, 0.0f, 0.0f, 0.0f)),
	_minThr(3.5f),
	_maxThr(3.8f)
{
	_offsets->setTo(
		(rand()/(float)RAND_MAX) * 10.0f,
		(rand()/(float)RAND_MAX) * 10.0f,
		(rand()/(float)RAND_MAX) * 10.0f,
		(rand()/(float)RAND_MAX) * 10.0f
	);
	_frequencies->setTo(
		 0.01f + (rand()/(float)RAND_MAX) * 0.5f,
		 0.01f + (rand()/(float)RAND_MAX) * 0.5f,
		 0.01f + (rand()/(float)RAND_MAX) * 0.5f,
		 0.01f + (rand()/(float)RAND_MAX) * 0.5f
	);

    _data
		->set("sponza.lightWorldPosition1",	Vector3::create(2.45f, 0.65f, 0.7f))
        ->set("sponza.lightAmbient1",		Vector3::create(0.25f, 0.12f, 0.0f))
		->set("sponza.lightDiffuse1",		Vector3::create(1.0f, 0.7f, 0.5f))
		->set("sponza.lightSpecular1",		Vector3::create(1.0f, 0.95f, 0.8f))
		->set("sponza.lightWorldPosition2",	Vector3::create(2.45f, 0.65f, -1.1f))
        ->set("sponza.lightAmbient2",		Vector3::create(0.25f, 0.12f, 0.0f))
		->set("sponza.lightDiffuse2",		Vector3::create(1.0f, 0.7f, 0.5f))
		->set("sponza.lightSpecular2",		Vector3::create(1.0f, 0.95f, 0.8f))
		->set("sponza.lightWorldPosition3",	Vector3::create(-3.14f, 0.65f, -1.1f))
        ->set("sponza.lightAmbient3",		Vector3::create(0.25f, 0.12f, 0.0f))
		->set("sponza.lightDiffuse3",		Vector3::create(1.0f, 0.7f, 0.5f))
		->set("sponza.lightSpecular3",		Vector3::create(1.0f, 0.95f, 0.8f))
		->set("sponza.lightWorldPosition4",	Vector3::create(-3.14f, 0.65f, 0.7f))
        ->set("sponza.lightAmbient4",		Vector3::create(0.25f, 0.12f, 0.0f))
		->set("sponza.lightDiffuse4",		Vector3::create(1.0f, 0.7f, 0.5f))
		->set("sponza.lightSpecular4",		Vector3::create(1.0f, 0.95f, 0.8f));
	_data
		->set("sponza.lightDistThres",	getDistanceThresholds());
}

void
SponzaLighting::step()
{
	_time = (_time + 1) % INT_MAX;

	Vector4::Ptr thres = _data->get<Vector4::Ptr>("sponza.lightDistThres");
	_data->set("sponza.lightDistThres", getDistanceThresholds(thres));
}

Vector4::Ptr
SponzaLighting::getDistanceThresholds(Vector4::Ptr output) const
{
	const float range = _maxThr - _minThr;
	float thr1 = _minThr + range * (0.5f + 0.5f * cosf(_frequencies->x() * (_time + _offsets->x())));
	float thr2 = _minThr + range * (0.5f + 0.5f * cosf(_frequencies->y() * (_time + _offsets->y())));
	float thr3 = _minThr + range * (0.5f + 0.5f * cosf(_frequencies->z() * (_time + _offsets->z())));
	float thr4 = _minThr + range * (0.5f + 0.5f * cosf(_frequencies->w() * (_time + _offsets->w())));
	
	return output
		? output->setTo(thr1, thr2, thr3, thr4)
		: Vector4::create(thr1, thr2, thr3, thr4);
}
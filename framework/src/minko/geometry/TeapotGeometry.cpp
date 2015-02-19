/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"}, to deal in the Software without restriction,
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

#include "minko/geometry/TeapotGeometry.hpp"

#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"

using namespace minko;
using namespace minko::geometry;

const uint TeapotGeometry::_patches[32][16] = {
	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 },
	{ 4, 17, 18, 19, 8, 20, 21, 22, 12, 23, 24, 25, 16, 26, 27, 28 },
	{ 19, 29, 30, 31, 22, 32, 33, 34, 25, 35, 36, 37, 28, 38, 39, 40 },
	{ 31, 41, 42, 1, 34, 43, 44, 5, 37, 45, 46, 9, 40, 47, 48, 13 },
	{ 13, 14, 15, 16, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60 },
	{ 16, 26, 27, 28, 52, 61, 62, 63, 56, 64, 65, 66, 60, 67, 68, 69 },
	{ 28, 38, 39, 40, 63, 70, 71, 72, 66, 73, 74, 75, 69, 76, 77, 78 },
	{ 40, 47, 48, 13, 72, 79, 80, 49, 75, 81, 82, 53, 78, 83, 84, 57 },
	{ 57, 58, 59, 60, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96 },
	{ 60, 67, 68, 69, 88, 97, 98, 99, 92, 100, 101, 102, 96, 103, 104, 105 },
	{ 69, 76, 77, 78, 99, 106, 107, 108, 102, 109, 110, 111, 105, 112, 113, 114 },
	{ 78, 83, 84, 57, 108, 115, 116, 85, 111, 117, 118, 89, 114, 119, 120, 93 },
	{ 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136 },
	{ 124, 137, 138, 121, 128, 139, 140, 125, 132, 141, 142, 129, 136, 143, 144, 133 },
	{ 133, 134, 135, 136, 145, 146, 147, 148, 149, 150, 151, 152, 69, 153, 154, 155 },
	{ 136, 143, 144, 133, 148, 156, 157, 145, 152, 158, 159, 149, 155, 160, 161, 69 },
	{ 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177 },
	{ 165, 178, 179, 162, 169, 180, 181, 166, 173, 182, 183, 170, 177, 184, 185, 174 },
	{ 174, 175, 176, 177, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197 },
	{ 177, 184, 185, 174, 189, 198, 199, 186, 193, 200, 201, 190, 197, 202, 203, 194 },
	{ 204, 204, 204, 204, 207, 208, 209, 210, 211, 211, 211, 211, 212, 213, 214, 215 },
	{ 204, 204, 204, 204, 210, 217, 218, 219, 211, 211, 211, 211, 215, 220, 221, 222 },
	{ 204, 204, 204, 204, 219, 224, 225, 226, 211, 211, 211, 211, 222, 227, 228, 229 },
	{ 204, 204, 204, 204, 226, 230, 231, 207, 211, 211, 211, 211, 229, 232, 233, 212 },
	{ 212, 213, 214, 215, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245 },
	{ 215, 220, 221, 222, 237, 246, 247, 248, 241, 249, 250, 251, 245, 252, 253, 254 },
	{ 222, 227, 228, 229, 248, 255, 256, 257, 251, 258, 259, 260, 254, 261, 262, 263 },
	{ 229, 232, 233, 212, 257, 264, 265, 234, 260, 266, 267, 238, 263, 268, 269, 242 },
	{ 270, 270, 270, 270, 279, 280, 281, 282, 275, 276, 277, 278, 271, 272, 273, 274 },
	{ 270, 270, 270, 270, 282, 289, 290, 291, 278, 286, 287, 288, 274, 283, 284, 285 },
	{ 270, 270, 270, 270, 291, 298, 299, 300, 288, 295, 296, 297, 285, 292, 293, 294 },
	{ 270, 270, 270, 270, 300, 305, 306, 279, 297, 303, 304, 275, 294, 301, 302, 271 }
};

const float TeapotGeometry::_points[306][3] = 
{
	{ 1.4f, 0.0f, 2.4f }, { 1.4f, -0.784f, 2.4f }, { 0.784f, -1.4f, 2.4f },
	{ 0.0f, -1.4f, 2.4f }, { 1.3375f, 0.0f, 2.53125f }, { 1.3375f, -0.749f, 2.53125f },
	{ 0.749f, -1.3375f, 2.53125f }, { 0.0f, -1.3375f, 2.53125f }, { 1.4375f, 0.0f, 2.53125f },
	{ 1.4375f, -0.805f, 2.53125f }, { 0.805f, -1.4375f, 2.53125f }, { 0.0f, -1.4375f, 2.53125f },
	{ 1.5f, 0.0f, 2.4f }, { 1.5f, -0.84f, 2.4f }, { 0.84f, -1.5f, 2.4f },
	{ 0.0f, -1.5f, 2.4f }, { -0.784f, -1.4f, 2.4f }, { -1.4f, -0.784f, 2.4f },
	{ -1.4f, 0.0f, 2.4f }, { -0.749f, -1.3375f, 2.53125f }, { -1.3375f, -0.749f, 2.53125f },
	{ -1.3375f, 0.0f, 2.53125f }, { -0.805f, -1.4375f, 2.53125f }, { -1.4375f, -0.805f, 2.53125f },
	{ -1.4375f, 0.0f, 2.53125f }, { -0.84f, -1.5f, 2.4f }, { -1.5f, -0.84f, 2.4f },
	{ -1.5f, 0.0f, 2.4f }, { -1.4f, 0.784f, 2.4f }, { -0.784f, 1.4f, 2.4f },
	{ 0.0f, 1.4f, 2.4f }, { -1.3375f, 0.749f, 2.53125f }, { -0.749f, 1.3375f, 2.53125f },
	{ 0.0f, 1.3375f, 2.53125f }, { -1.4375f, 0.805f, 2.53125f }, { -0.805f, 1.4375f, 2.53125f },
	{ 0.0f, 1.4375f, 2.53125f }, { -1.5f, 0.84f, 2.4f }, { -0.84f, 1.5f, 2.4f },
	{ 0.0f, 1.5f, 2.4f }, { 0.784f, 1.4f, 2.4f }, { 1.4f, 0.784f, 2.4f },
	{ 0.749f, 1.3375f, 2.53125f }, { 1.3375f, 0.749f, 2.53125f }, { 0.805f, 1.4375f, 2.53125f },
	{ 1.4375f, 0.805f, 2.53125f }, { 0.84f, 1.5f, 2.4f }, { 1.5f, 0.84f, 2.4f },
	{ 1.75f, 0.0f, 1.875f }, { 1.75f, -0.98f, 1.875f }, { 0.98f, -1.75f, 1.875f },
	{ 0.0f, -1.75f, 1.875f }, { 2.0f, 0.0f, 1.35f }, { 2.0f, -1.12f, 1.35f },
	{ 1.12f, -2.0f, 1.35f }, { 0.0f, -2.0f, 1.35f }, { 2.0f, 0.0f, 0.9f },
	{ 2.0f, -1.12f, 0.9f }, { 1.12f, -2.0f, 0.9f }, { 0.0f, -2.0f, 0.9f },
	{ -0.98f, -1.75f, 1.875f }, { -1.75f, -0.98f, 1.875f }, { -1.75f, 0.0f, 1.875f },
	{ -1.12f, -2.0f, 1.35f }, { -2.0f, -1.12f, 1.35f }, { -2.0f, 0.0f, 1.35f },
	{ -1.12f, -2.0f, 0.9f }, { -2.0f, -1.12f, 0.9f }, { -2.0f, 0.0f, 0.9f },
	{ -1.75f, 0.98f, 1.875f }, { -0.98f, 1.75f, 1.875f }, { 0.0f, 1.75f, 1.875f },
	{ -2.0f, 1.12f, 1.35f }, { -1.12f, 2.0f, 1.35f }, { 0.0f, 2.0f, 1.35f },
	{ -2.0f, 1.12f, 0.9f }, { -1.12f, 2.0f, 0.9f }, { 0.0f, 2.0f, 0.9f },
	{ 0.98f, 1.75f, 1.875f }, { 1.75f, 0.98f, 1.875f }, { 1.12f, 2.0f, 1.35f },
	{ 2.0f, 1.12f, 1.35f }, { 1.12f, 2.0f, 0.9f }, { 2.0f, 1.12f, 0.9f },
	{ 2.0f, 0.0f, 0.45f }, { 2.0f, -1.12f, 0.45f }, { 1.12f, -2.0f, 0.45f },
	{ 0.0f, -2.0f, 0.45f }, { 1.5f, 0.0f, 0.225f }, { 1.5f, -0.84f, 0.225f },
	{ 0.84f, -1.5f, 0.225f }, { 0.0f, -1.5f, 0.225f }, { 1.5f, 0.0f, 0.15f },
	{ 1.5f, -0.84f, 0.15f }, { 0.84f, -1.5f, 0.15f }, { 0.0f, -1.5f, 0.15f },
	{ -1.12f, -2.0f, 0.45f }, { -2.0f, -1.12f, 0.45f }, { -2.0f, 0.0f, 0.45f },
	{ -0.84f, -1.5f, 0.225f }, { -1.5f, -0.84f, 0.225f }, { -1.5f, 0.0f, 0.225f },
	{ -0.84f, -1.5f, 0.15f }, { -1.5f, -0.84f, 0.15f }, { -1.5f, 0.0f, 0.15f },
	{ -2.0f, 1.12f, 0.45f }, { -1.12f, 2.0f, 0.45f }, { 0.0f, 2.0f, 0.45f },
	{ -1.5f, 0.84f, 0.225f }, { -0.84f, 1.5f, 0.225f }, { 0.0f, 1.5f, 0.225f },
	{ -1.5f, 0.84f, 0.15f }, { -0.84f, 1.5f, 0.15f }, { 0.0f, 1.5f, 0.15f },
	{ 1.12f, 2.0f, 0.45f }, { 2.0f, 1.12f, 0.45f }, { 0.84f, 1.5f, 0.225f },
	{ 1.5f, 0.84f, 0.225f }, { 0.84f, 1.5f, 0.15f }, { 1.5f, 0.84f, 0.15f },
	{ -1.6f, 0.0f, 2.025f }, { -1.6f, -0.3f, 2.025f }, { -1.5f, -0.3f, 2.25f },
	{ -1.5f, 0.0f, 2.25f }, { -2.3f, 0.0f, 2.025f }, { -2.3f, -0.3f, 2.025f },
	{ -2.5f, -0.3f, 2.25f }, { -2.5f, 0.0f, 2.25f }, { -2.7f, 0.0f, 2.025f },
	{ -2.7f, -0.3f, 2.025f }, { -3.0f, -0.3f, 2.25f }, { -3.0f, 0.0f, 2.25f },
	{ -2.7f, 0.0f, 1.8f }, { -2.7f, -0.3f, 1.8f }, { -3.0f, -0.3f, 1.8f },
	{ -3.0f, 0.0f, 1.8f }, { -1.5f, 0.3f, 2.25f }, { -1.6f, 0.3f, 2.025f },
	{ -2.5f, 0.3f, 2.25f }, { -2.3f, 0.3f, 2.025f }, { -3.0f, 0.3f, 2.25f },
	{ -2.7f, 0.3f, 2.025f }, { -3.0f, 0.3f, 1.8f }, { -2.7f, 0.3f, 1.8f },
	{ -2.7f, 0.0f, 1.575f }, { -2.7f, -0.3f, 1.575f }, { -3.0f, -0.3f, 1.35f },
	{ -3.0f, 0.0f, 1.35f }, { -2.5f, 0.0f, 1.125f }, { -2.5f, -0.3f, 1.125f },
	{ -2.65f, -0.3f, 0.9375f }, { -2.65f, 0.0f, 0.9375f }, { -2.0f, -0.3f, 0.9f },
	{ -1.9f, -0.3f, 0.6f }, { -1.9f, 0.0f, 0.6f }, { -3.0f, 0.3f, 1.35f },
	{ -2.7f, 0.3f, 1.575f }, { -2.65f, 0.3f, 0.9375f }, { -2.5f, 0.3f, 1.125f },
	{ -1.9f, 0.3f, 0.6f }, { -2.0f, 0.3f, 0.9f }, { 1.7f, 0.0f, 1.425f },
	{ 1.7f, -0.66f, 1.425f }, { 1.7f, -0.66f, 0.6f }, { 1.7f, 0.0f, 0.6f },
	{ 2.6f, 0.0f, 1.425f }, { 2.6f, -0.66f, 1.425f }, { 3.1f, -0.66f, 0.825f },
	{ 3.1f, 0.0f, 0.825f }, { 2.3f, 0.0f, 2.1f }, { 2.3f, -0.25f, 2.1f },
	{ 2.4f, -0.25f, 2.025f }, { 2.4f, 0.0f, 2.025f }, { 2.7f, 0.0f, 2.4f },
	{ 2.7f, -0.25f, 2.4f }, { 3.3f, -0.25f, 2.4f }, { 3.3f, 0.0f, 2.4f },
	{ 1.7f, 0.66f, 0.6f }, { 1.7f, 0.66f, 1.425f }, { 3.1f, 0.66f, 0.825f },
	{ 2.6f, 0.66f, 1.425f }, { 2.4f, 0.25f, 2.025f }, { 2.3f, 0.25f, 2.1f },
	{ 3.3f, 0.25f, 2.4f }, { 2.7f, 0.25f, 2.4f }, { 2.8f, 0.0f, 2.475f },
	{ 2.8f, -0.25f, 2.475f }, { 3.525f, -0.25f, 2.49375f }, { 3.525f, 0.0f, 2.49375f },
	{ 2.9f, 0.0f, 2.475f }, { 2.9f, -0.15f, 2.475f }, { 3.45f, -0.15f, 2.5125f },
	{ 3.45f, 0.0f, 2.5125f }, { 2.8f, 0.0f, 2.4f }, { 2.8f, -0.15f, 2.4f },
	{ 3.2f, -0.15f, 2.4f }, { 3.2f, 0.0f, 2.4f }, { 3.525f, 0.25f, 2.49375f },
	{ 2.8f, 0.25f, 2.475f }, { 3.45f, 0.15f, 2.5125f }, { 2.9f, 0.15f, 2.475f },
	{ 3.2f, 0.15f, 2.4f }, { 2.8f, 0.15f, 2.4f }, { 0.0f, 0.0f, 3.15f },
	{ 0.0f, -0.002f, 3.15f }, { 0.002f, 0.0f, 3.15f }, { 0.8f, 0.0f, 3.15f },
	{ 0.8f, -0.45f, 3.15f }, { 0.45f, -0.8f, 3.15f }, { 0.0f, -0.8f, 3.15f },
	{ 0.0f, 0.0f, 2.85f }, { 0.2f, 0.0f, 2.7f }, { 0.2f, -0.112f, 2.7f },
	{ 0.112f, -0.2f, 2.7f }, { 0.0f, -0.2f, 2.7f }, { -0.002f, 0.0f, 3.15f },
	{ -0.45f, -0.8f, 3.15f }, { -0.8f, -0.45f, 3.15f }, { -0.8f, 0.0f, 3.15f },
	{ -0.112f, -0.2f, 2.7f }, { -0.2f, -0.112f, 2.7f }, { -0.2f, 0.0f, 2.7f },
	{ 0.0f, 0.002f, 3.15f }, { -0.8f, 0.45f, 3.15f }, { -0.45f, 0.8f, 3.15f },
	{ 0.0f, 0.8f, 3.15f }, { -0.2f, 0.112f, 2.7f }, { -0.112f, 0.2f, 2.7f },
	{ 0.0f, 0.2f, 2.7f }, { 0.45f, 0.8f, 3.15f }, { 0.8f, 0.45f, 3.15f },
	{ 0.112f, 0.2f, 2.7f }, { 0.2f, 0.112f, 2.7f }, { 0.4f, 0.0f, 2.55f },
	{ 0.4f, -0.224f, 2.55f }, { 0.224f, -0.4f, 2.55f }, { 0.0f, -0.4f, 2.55f },
	{ 1.3f, 0.0f, 2.55f }, { 1.3f, -0.728f, 2.55f }, { 0.728f, -1.3f, 2.55f },
	{ 0.0f, -1.3f, 2.55f }, { 1.3f, 0.0f, 2.4f }, { 1.3f, -0.728f, 2.4f },
	{ 0.728f, -1.3f, 2.4f }, { 0.0f, -1.3f, 2.4f }, { -0.224f, -0.4f, 2.55f },
	{ -0.4f, -0.224f, 2.55f }, { -0.4f, 0.0f, 2.55f }, { -0.728f, -1.3f, 2.55f },
	{ -1.3f, -0.728f, 2.55f }, { -1.3f, 0.0f, 2.55f }, { -0.728f, -1.3f, 2.4f },
	{ -1.3f, -0.728f, 2.4f }, { -1.3f, 0.0f, 2.4f }, { -0.4f, 0.224f, 2.55f },
	{ -0.224f, 0.4f, 2.55f }, { 0.0f, 0.4f, 2.55f }, { -1.3f, 0.728f, 2.55f },
	{ -0.728f, 1.3f, 2.55f }, { 0.0f, 1.3f, 2.55f }, { -1.3f, 0.728f, 2.4f },
	{ -0.728f, 1.3f, 2.4f }, { 0.0f, 1.3f, 2.4f }, { 0.224f, 0.4f, 2.55f },
	{ 0.4f, 0.224f, 2.55f }, { 0.728f, 1.3f, 2.55f }, { 1.3f, 0.728f, 2.55f },
	{ 0.728f, 1.3f, 2.4f }, { 1.3f, 0.728f, 2.4f }, { 0.0f, 0.0f, 0.0f },
	{ 1.5f, 0.0f, 0.15f }, { 1.5f, 0.84f, 0.15f }, { 0.84f, 1.5f, 0.15f },
	{ 0.0f, 1.5f, 0.15f }, { 1.5f, 0.0f, 0.075f }, { 1.5f, 0.84f, 0.075f },
	{ 0.84f, 1.5f, 0.075f }, { 0.0f, 1.5f, 0.075f }, { 1.425f, 0.0f, 0.0f },
	{ 1.425f, 0.798f, 0.0f }, { 0.798f, 1.425f, 0.0f }, { 0.0f, 1.425f, 0.0f },
	{ -0.84f, 1.5f, 0.15f }, { -1.5f, 0.84f, 0.15f }, { -1.5f, 0.0f, 0.15f },
	{ -0.84f, 1.5f, 0.075f }, { -1.5f, 0.84f, 0.075f }, { -1.5f, 0.0f, 0.075f },
	{ -0.798f, 1.425f, 0.0f }, { -1.425f, 0.798f, 0.0f }, { -1.425f, 0.0f, 0.0f },
	{ -1.5f, -0.84f, 0.15f }, { -0.84f, -1.5f, 0.15f }, { 0.0f, -1.5f, 0.15f },
	{ -1.5f, -0.84f, 0.075f }, { -0.84f, -1.5f, 0.075f }, { 0.0f, -1.5f, 0.075f },
	{ -1.425f, -0.798f, 0.0f }, { -0.798f, -1.425f, 0.0f }, { 0.0f, -1.425f, 0.0f },
	{ 0.84f, -1.5f, 0.15f }, { 1.5f, -0.84f, 0.15f }, { 0.84f, -1.5f, 0.075f },
	{ 1.5f, -0.84f, 0.075f }, { 0.798f, -1.425f, 0.0f }, { 1.425f, -0.798f, 0.0f }
};

TeapotGeometry::TeapotGeometry() :
    Geometry("teapot")
{
}

void
TeapotGeometry::initialize(std::shared_ptr<render::AbstractContext> context, const uint divs)
{
	std::vector<unsigned short> indicesData;
	std::vector<std::vector<float>> vertices(1, std::vector<float>());
	std::vector<std::vector<float>> patch(16, std::vector<float>(16, 0.f));
	uint currentVertexId = 0;

	for (auto& bezierPatch : _patches)
	{
		for (uint i = 0; i < 16; ++i)
		{
			patch[i][0] = _points[bezierPatch[i] - 1][0];
			patch[i][1] = _points[bezierPatch[i] - 1][1];
			patch[i][2] = _points[bezierPatch[i] - 1][2];
		}

		genPatchVertices(patch, divs, vertices[0]);
		genPatchIndices(currentVertexId, divs, indicesData);

		currentVertexId += (divs + 1) * (divs + 1);
	}

	// this is slow and memory consuming and could be avoided by non duplicating all border
	// vertices on genPatchVertexData and genPatchIndexData...
	removeDuplicatedVertices(indicesData, vertices, vertices[0].size() / 3);

	auto vb = render::VertexBuffer::create(context, vertices[0]);

	vb->addAttribute("position", 3);
	addVertexBuffer(vb);

	indices(render::IndexBuffer::create(context, indicesData));

    computeCenterPosition();
}

void
TeapotGeometry::genPatchVertices(std::vector<std::vector<float>>& patch, const uint divs, std::vector<float>& vertices)
{
	std::vector<float> zero3(3, 0.f);
	std::vector<std::vector<float>> last(divs + 1, zero3);
	std::vector<std::vector<float>> tmp(4, zero3);

	for (uint u = 0; u <= divs; ++u)
	{
		auto py = (float)u / (float)divs;

		bernstein(py, patch[0], patch[1], patch[2], patch[3], tmp[0]);
		bernstein(py, patch[4], patch[5], patch[6], patch[7], tmp[1]);
		bernstein(py, patch[8], patch[9], patch[10], patch[11], tmp[2]);
		bernstein(py, patch[12], patch[13], patch[14], patch[15], tmp[3]);

		for (uint v = 0; v <= divs; ++v)
		{
			auto px = (float)v / (float)divs;

			bernstein(px, tmp[0], tmp[1], tmp[2], tmp[3], last[v]);
			vertices.push_back(last[v][0]);
			vertices.push_back(last[v][2]);
			vertices.push_back(last[v][1]);
		}
	}
}

void
TeapotGeometry::bernstein(float					u,
						  std::vector<float>&	p0,
						  std::vector<float>&	p1,
						  std::vector<float>&	p2,
						  std::vector<float>&	p3,
						  std::vector<float>&	out)
{
	auto um = 1.f - u;
	auto um2 = um * um;
	auto u2 = u * u;

	auto c0 = u * u2;
	auto c1 = 3.f * u2 * um;
	auto c2 = 3.f * u * um2;
	auto c3 = um * um2;

	out[0] = c0 * p0[0] + c1 * p1[0] + c2 * p2[0] + c3 * p3[0];
	out[1] = c0 * p0[1] + c1 * p1[1] + c2 * p2[1] + c3 * p3[1];
	out[2] = c0 * p0[2] + c1 * p1[2] + c2 * p2[2] + c3 * p3[2];
}

void
TeapotGeometry::genPatchIndices(const uint offset, const uint divs, std::vector<unsigned short>& indices)
{
	for (uint u = 0; u < divs; ++u)
	{
		for (uint v = 0; v < divs; ++v)
		{
			indices.push_back(offset + u * (divs + 1) + v);
			indices.push_back(offset + u * (divs + 1) + v + 1);
			indices.push_back(offset + (u + 1) * (divs + 1) + v);

			indices.push_back(offset + u * (divs + 1) + v + 1);
			indices.push_back(offset + (u + 1) * (divs + 1) + v + 1);
			indices.push_back(offset + (u + 1) * (divs + 1) + v);
		}
	}
}

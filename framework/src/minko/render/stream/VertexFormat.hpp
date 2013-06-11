#pragma once

namespace minko
{
	namespace render
	{
		namespace stream
		{
			class VertexFormat
			{
			public:
				struct XYZ
				{
					float x;
					float y;
					float z;
				};

				struct Normal
				{
					float nx;
					float ny;
					float nz;
				};

				struct Tangent
				{
					float tx;
					float ty;
					float tz;
				};

				struct UV
				{
					float u;
					float v;
				};
			};
		}
	}
}
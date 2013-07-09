#pragma once

namespace minko
{
	namespace particle
	{
		enum VertexComponentFlags
		{
			DEFAULT			= 0x0,
			COLOR			= 0x1,
			SIZE			= (0x1 << 1),
			TIME			= (0x1 << 2),
			OLD_POSITION	= (0x1 << 3),
			ROTATION		= (0x1 << 4),
			ANG_VELOCITY	= (0x1 << 5),
			SPRITEINDEX		= (0x1 << 6)
		};
	}
}
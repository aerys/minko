/*
Copyright (c) 2014 Aerys

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

#include "minko/Common.hpp"

#include "minko/animation/AbstractTimeline.hpp"

namespace minko
{
	namespace animation
	{
		class Matrix4x4Timeline: public AbstractTimeline
		{
		public:
			typedef std::shared_ptr<Matrix4x4Timeline>			Ptr;

		private:
			typedef std::vector<std::pair<uint, math::mat4>>	MatrixTimetable;

		private:
			MatrixTimetable	            _matrices;
			bool			            _interpolate;

		public:
			inline static
			Ptr
			create(const std::string&				propertyName,
				   uint								duration,
				   const std::vector<uint>&			timetable,
				   const std::vector<math::mat4>&	matrices,
				   bool								interpolate = false)
			{
				Ptr ptr	= std::shared_ptr<Matrix4x4Timeline>(
					new Matrix4x4Timeline(
						propertyName,
						duration,
						timetable, 
						matrices, 
						interpolate
					)
				);

				return ptr;
			}

			inline static
			Ptr
			create(const Matrix4x4Timeline&	timeline)
			{
				Ptr ptr = std::shared_ptr<Matrix4x4Timeline>(new Matrix4x4Timeline(timeline));

				return ptr;
			}

			AbstractTimeline::Ptr
			clone();

            inline
            const std::vector<std::pair<uint, math::mat4>>&
            matrices() const
            {
                return _matrices;
            }

            inline
            bool
            interpolate() const
            {
                return _interpolate;
            }

			void
			update(uint time, data::Store& data, bool skipPropertyNameFormatting = true);

            math::mat4
            interpolate(uint time) const;

		private:
			Matrix4x4Timeline(const std::string&,
							  uint,
							  const std::vector<uint>&,
							  const std::vector<math::mat4>&,
							  bool);

			Matrix4x4Timeline(const Matrix4x4Timeline& matrix);

			void
			initializeMatrixTimetable(const std::vector<uint>&,
									  const std::vector<math::mat4>&);
		};
	}
}
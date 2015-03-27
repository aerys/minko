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

namespace minko
{
    namespace data
    {
        class HalfEdge
        {
        public :
            typedef std::shared_ptr<HalfEdge> Ptr;

        public:
            inline static
            Ptr
            create(unsigned int startNodeId,
                   unsigned int endNodeId,
                   unsigned int edgeId)
            {
                auto instance = Ptr(new HalfEdge(startNodeId, endNodeId, edgeId));

                return instance;
            }

        private:
            unsigned int                _startNodeId;
            unsigned int                _endNodeId;
            unsigned int                _edgeId;
            Ptr                         _next;
            Ptr                         _prec;
            Ptr                         _adjacent;
            std::vector<Ptr>            _face;
            std::vector<Ptr>            _firstReverseFace;
            std::vector<Ptr>            _secondReverseFace;
            bool                        _marked;

        private :
            HalfEdge(unsigned int startNodeId,
                     unsigned int endNodeId,
                     unsigned int edgeId);

        public:
            bool
            indiceInEdge(unsigned int indice);

            bool
            indiceInFace(unsigned int indice);

            unsigned int
            getThirdVertex();

            inline
            bool
            marked() const
            {
                return _marked;
            }

            inline
            unsigned int
            startNodeId() const
            {
                return _startNodeId;
            }

            inline
            void
            marked(bool value)
            {
                _marked = value;
            }

            inline
            unsigned int
            endNodeId() const
            {
                return _endNodeId;
            }

            inline
            unsigned int
            edgeId() const
            {
                return _edgeId;
            }

            inline
            Ptr
            next() const
            {
                return _next;
            }

            inline
            void
            next(Ptr value)
            {
                _next = value;
            }

            inline
            Ptr
            prec() const
            {
                return _prec;
            }

            inline
            void
            prec(Ptr value)
            {
                _prec = value;
            }

            inline
            Ptr
            adjacent() const
            {
                return _adjacent;
            }

            inline
            void
            adjacent(Ptr value)
            {
                _adjacent = value;
            }

            inline
            const std::vector<Ptr>&
            face() const
            {
                return _face;
            }

            inline
            std::vector<Ptr>&
            face()
            {
                return _face;
            }

            void
            setFace(Ptr he1,
                    Ptr he2,
                    Ptr he3);

            inline
            std::vector<Ptr>&
            secondReverseFace()
            {
                if (_secondReverseFace.size() == 0)
                {
                    _secondReverseFace.push_back(_face[2]);
                    _secondReverseFace.push_back(_face[0]);
                    _secondReverseFace.push_back(_face[1]);
                }

                return _secondReverseFace;
            }

            inline
            std::vector<Ptr>&
            firstReverseFace()
            {
                if (_firstReverseFace.size() == 0)
                {
                    _firstReverseFace.push_back(_face[1]);
                    _firstReverseFace.push_back(_face[2]);
                    _firstReverseFace.push_back(_face[0]);
                }

                return _firstReverseFace;
            }
        };

        // debug
        std::ostream &operator<<(std::ostream &flux, const HalfEdge& halfEdge);
        std::ostream &operator<<(std::ostream &flux, std::shared_ptr<HalfEdge> halfEdge);
    }
}

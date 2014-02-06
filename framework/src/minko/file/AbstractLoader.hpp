
#pragma once


#include "minko/Common.hpp"
#include "minko/Signal.hpp"

namespace minko
{
    namespace file
    {
        class AbstractLoader :
                public std::enable_shared_from_this<AbstractLoader>
        {
        public:
            typedef std::shared_ptr<AbstractLoader> Ptr;

        protected:
            std::vector<unsigned char>      _data;
            std::shared_ptr<Options>        _options;
            std::string                     _filename;
            std::string                     _resolvedFilename;

            std::shared_ptr<Signal<Ptr>>        _complete;
            std::shared_ptr<Signal<Ptr, float>> _progress;
            std::shared_ptr<Signal<Ptr>>        _error;

        public:
            static Ptr create();

            inline
            const std::vector<unsigned char>&
            data()
            {
                return _data;
            }

            inline
            std::shared_ptr<Options>
            options()
            {
                return _options;
            }

            inline
            const std::string&
            filename()
            {
                return _filename;
            }

            inline
            const std::string&
            resolvedFilename()
            {
                return _resolvedFilename;
            }

            inline
            std::shared_ptr<Signal<Ptr>>
            complete()
            {
                return _complete;
            }

            inline
            std::shared_ptr<Signal<Ptr, float>>
            progress()
            {
                return _progress;
            }

            inline
            std::shared_ptr<Signal<Ptr>>
            error()
            {
                return _error;
            }

            virtual void
            load(const std::string& filename, std::shared_ptr<Options> options) = 0;

        protected:
            AbstractLoader():
                _complete(Signal<Ptr>::create()),
                _progress(Signal<Ptr, float>::create()),
                _error(Signal<Ptr>::create())
            {
            }
        };

    }
}


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

            std::shared_ptr<Signal<Ptr>>    _complete;
            std::shared_ptr<Signal<Ptr>>    _progress;
            std::shared_ptr<Signal<Ptr>>    _error;

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
            std::shared_ptr<Signal<Ptr>>
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

            virtual
            char
            separator()
            {
#ifdef _WIN32
                return '\\';
#else
                return '/';
#endif
            }

            virtual void
            load(const std::string& filename, std::shared_ptr<Options> options) = 0;

        protected:
            AbstractLoader():
                _complete(Signal<Ptr>::create()),
                _progress(Signal<Ptr>::create()),
                _error(Signal<Ptr>::create())
            {
            }

            std::string
            sanitizeFilename(const std::string& filename)
            {
                auto f = filename;
                auto sep = separator();
                auto a = sep == '/' ? '\\' : '/';

                for (auto pos = f.find_first_of(a);
                     pos != std::string::npos;
                     pos = f.find_first_of(a))
                {
                    f = f.replace(pos, 1, 1, sep);
                }

                return f;
            }
        };

    }
}

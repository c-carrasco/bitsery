//MIT License
//
//Copyright (c) 2017 Mindaugas Vinkelis
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#ifndef BITSERY_EXT_ENTROPY_H
#define BITSERY_EXT_ENTROPY_H

#include "value_range.h"

namespace bitsery {

    namespace details {
        template<typename TValue, typename TContainer>
        size_t findEntropyIndex(const TValue &v, const TContainer &defValues) {
            size_t index{1u};
            for (auto &d:defValues) {
                if (d == v)
                    return index;
                ++index;
            }
            return 0u;
        };
    }

    namespace ext {

        template<typename TContainer>
        class Entropy {
        public:

            constexpr explicit Entropy(TContainer& values) : _values{values} {

            };

            template<typename Ser, typename Writer, typename T, typename Fnc>
            void serialize(Ser &s, Writer &writer, const T &obj, Fnc &&fnc) const {
                assert(details::ContainerTraits<TContainer>::size(_values) > 0);
                auto index = details::findEntropyIndex(obj, _values);
                s.ext(index, ext::ValueRange<size_t>{0u, details::ContainerTraits<TContainer>::size(_values)});
                if (!index)
                    fnc(const_cast<T &>(obj));
            }

            template<typename Des, typename Reader, typename T, typename Fnc>
            void deserialize(Des &d, Reader &reader, T &obj, Fnc &&fnc) const {
                assert(details::ContainerTraits<TContainer>::size(_values) > 0);
                size_t index{};
                d.ext(index, ext::ValueRange<size_t>{0u, details::ContainerTraits<TContainer>::size(_values)});
                if (index)
                    obj = *std::next(std::begin(_values), index-1);
                else
                    fnc(obj);
            }

        private:
            TContainer& _values;
        };
    }

    namespace details {
        template<typename TContainer, typename T>
        struct ExtensionTraits<ext::Entropy<TContainer>, T> {
            using TValue = T;
            static constexpr bool SupportValueOverload = true;
            static constexpr bool SupportObjectOverload = true;
            static constexpr bool SupportLambdaOverload = true;
        };
    }

}


#endif //BITSERY_EXT_ENTROPY_H

/******
st_tree: An easy to use and highly configurable C++ template tree class, 
using STL container style interfaces.

Copyright (c) 2010-2011 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
******/

#if !defined(__x_common_h__)
#define __x_common_h__ 1

inline const char* indent_padding(unsigned n) {
    static char const spaces[] = "                                                                   ";
    static const unsigned ns = sizeof(spaces)/sizeof(*spaces);
    if (n >= ns) n = ns-1;
    return spaces + (ns-1-n);
}


template <typename Iterator, typename Stream>
void serialize_indented(const Iterator& F, const Iterator& L, Stream& s, unsigned indent=2) {
    for (Iterator j(F);  j != L;  ++j) {
        s << indent_padding(j->ply() * indent);
        s << j->data();
        s << "\n";
    }
}


template <typename Tree, typename Stream>
void serialize_indented(const Tree& t, Stream& s, unsigned indent=2) {
    serialize_indented(t.df_pre_begin(), t.df_pre_end(), s, indent);
}

#endif

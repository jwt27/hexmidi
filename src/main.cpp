#include <iostream>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <jw/io/key.h>
#include <jw/vector.h>


namespace jw
{
    using namespace io;
    std::unordered_map<key, vector2i> key_grid
    {
        { key::backtick,    {  -1, 0 } },
        { key::n1,          {   0, 0 } },
        { key::n2,          {   1, 0 } },
        { key::n3,          {   2, 0 } },
        { key::n4,          {   3, 0 } },
        { key::n5,          {   4, 0 } },
        { key::n6,          {   5, 0 } },
        { key::n7,          {   6, 0 } },
        { key::n8,          {   7, 0 } },
        { key::n9,          {   8, 0 } },
        { key::n0,          {   9, 0 } },
        { key::minus,       {  10, 0 } },
        { key::equals,      {  11, 0 } },
        { key::q,           {   0, 1 } },
        { key::w,           {   1, 1 } },
        { key::e,           {   2, 1 } },
        { key::r,           {   3, 1 } },
        { key::t,           {   4, 1 } },
        { key::y,           {   5, 1 } },
        { key::u,           {   6, 1 } },
        { key::i,           {   7, 1 } },
        { key::o,           {   8, 1 } },
        { key::p,           {   9, 1 } },
        { key::brace_left,  {  10, 1 } },
        { key::brace_right, {  11, 1 } },
        { key::backslash,   {  12, 1 } },
        { key::a,           {   0, 2 } },
        { key::s,           {   1, 2 } },
        { key::d,           {   2, 2 } },
        { key::f,           {   3, 2 } },
        { key::g,           {   4, 2 } },
        { key::h,           {   5, 2 } },
        { key::j,           {   6, 2 } },
        { key::k,           {   7, 2 } },
        { key::l,           {   8, 2 } },
        { key::semicolon,   {   9, 2 } },
        { key::quote,       {  10, 2 } },
        { key::z,           {   0, 3 } },
        { key::x,           {   1, 3 } },
        { key::c,           {   2, 3 } },
        { key::v,           {   3, 3 } },
        { key::b,           {   4, 3 } },
        { key::n,           {   5, 3 } },
        { key::m,           {   6, 3 } },
        { key::comma,       {   7, 3 } },
        { key::dot,         {   8, 3 } },
        { key::slash,       {   9, 3 } }
    };
}



int jwdpmi_main(std::deque<std::string_view>)
{
    
    return 0;
}

#include <iostream>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <jw/io/key.h>
#include <jw/vector.h>
#include <jw/io/keyboard.h>
#include <jw/io/ps2_interface.h>
#include <jw/io/mpu401.h>

namespace jw
{
    using namespace io;
    std::unordered_map<key, vector2i> key_grid
    {
        { key::backtick,    {  -1, 3 } },
        { key::n1,          {   0, 3 } },
        { key::n2,          {   1, 3 } },
        { key::n3,          {   2, 3 } },
        { key::n4,          {   3, 3 } },
        { key::n5,          {   4, 3 } },
        { key::n6,          {   5, 3 } },
        { key::n7,          {   6, 3 } },
        { key::n8,          {   7, 3 } },
        { key::n9,          {   8, 3 } },
        { key::n0,          {   9, 3 } },
        { key::minus,       {  10, 3 } },
        { key::equals,      {  11, 3 } },
        { key::q,           {   0, 2 } },
        { key::w,           {   1, 2 } },
        { key::e,           {   2, 2 } },
        { key::r,           {   3, 2 } },
        { key::t,           {   4, 2 } },
        { key::y,           {   5, 2 } },
        { key::u,           {   6, 2 } },
        { key::i,           {   7, 2 } },
        { key::o,           {   8, 2 } },
        { key::p,           {   9, 2 } },
        { key::brace_left,  {  10, 2 } },
        { key::brace_right, {  11, 2 } },
        { key::backslash,   {  12, 2 } },
        { key::a,           {   0, 1 } },
        { key::s,           {   1, 1 } },
        { key::d,           {   2, 1 } },
        { key::f,           {   3, 1 } },
        { key::g,           {   4, 1 } },
        { key::h,           {   5, 1 } },
        { key::j,           {   6, 1 } },
        { key::k,           {   7, 1 } },
        { key::l,           {   8, 1 } },
        { key::semicolon,   {   9, 1 } },
        { key::quote,       {  10, 1 } },
        { key::z,           {   0, 0 } },
        { key::x,           {   1, 0 } },
        { key::c,           {   2, 0 } },
        { key::v,           {   3, 0 } },
        { key::b,           {   4, 0 } },
        { key::n,           {   5, 0 } },
        { key::m,           {   6, 0 } },
        { key::comma,       {   7, 0 } },
        { key::dot,         {   8, 0 } },
        { key::slash,       {   9, 0 } }
    };

    void hexmidi()
    {
        bool running { true };
        mpu401_stream mpu { mpu401_config { } };
        byte base_note = 45;
        vector2i step { 2, 3 };

        callback key_event { [&] (key_state_pair k)
        {
            if (k.second == key_state::repeat) return;
            switch (k.first)
            {
            default:
                try
                {
                    auto g = key_grid.at(k.first);
                    byte event = k.second == key_state::down ? 0x90 : 0x80;
                    byte note = base_note + step.x * g.x + step.y * g.y;
                    byte vel = 100;
                    mpu << event << note << vel << std::flush;
                }
                catch (const std::out_of_range&) { }
                break;

            case key::esc:
                running = false;
                break;
            }
        } };

        keyboard keyb { std::make_shared<ps2_interface>() };
        keyb.key_changed += key_event;
        keyb.auto_update(true);

        while (running)
        {
            thread::yield();
        }
    }
}

int jwdpmi_main(std::deque<std::string_view>)
{
    jw::hexmidi();
    return 0;
}

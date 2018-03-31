/* * * * * * * * * * * * * *  hexmidi  * * * * * * * * * * * * * */
/* Copyright (C) 2018 J.W. Jagersma, see COPYING.txt for details */

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

    std::array<bool, 12> scale;

    struct midi_note
    {
        byte value;

        friend std::ostream& operator<<(std::ostream& out, midi_note n)
        {
            constexpr std::array<const char*, 12> names { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };
            return out << names[n.value % 12] << n.value / 12;
        }

        constexpr operator byte() const noexcept { return value; }
    };

    bool running { true };
    byte base { 45 };
    vector2i step { 2, 3 };

    void print_grid()
    {
        std::cout << "Z=" << std::dec << midi_note { base } << ", step=" << step << std::endl;
    }

    void print_scale()
    {
        constexpr std::array<const char*, 12> names { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        std::cout << "Scale: "
        for (int i = 0; i < 12; ++i)
            if (scale[i]) std::cout << names[i] << ' ';
        std::cout << std::endl;
    }

    void hexmidi()
    {
        scale.fill(true);

        mpu401_stream mpu { mpu401_config { } };
        keyboard keyb { std::make_shared<ps2_interface>() };

        callback key_event { [&] (key_state_pair k)
        {
            auto ctrl = keyb[key::any_ctrl];
            auto alt = keyb[key::any_alt];
            if (k.second != key_state::repeat)
            {
                switch (k.first)
                {
                default:
                    try
                    {
                        auto g = key_grid.at(k.first);
                        byte event = k.second == key_state::down ? 0x90 : 0x80;
                        byte note = base + step.x * g.x + step.y * g.y;
                        if (alt and k.second.is_down())
                        {
                            auto& s = scale[note % 12];
                            s = not s;
                            print_scale();
                        }
                        byte vel = 100;
                        if (scale[note % 12] or k.second.is_up()) mpu << event << note << vel << std::flush;
                    }
                    catch (const std::out_of_range&) { }
                    break;

                case key::space:
                {
                    byte event = 0xb0;
                    byte cc = 64;
                    byte value = k.second == key_state::down ? 0x7f : 0x00;
                    mpu << event << cc << value << std::flush;
                    break;
                }
                }
            }
            if (not k.second.is_up())
            {
                switch (k.first)
                {
                case key::esc: running = false; break;

                case key::down:     if (ctrl) --step.y; else base -= step.y; print_grid(); break;
                case key::up:       if (ctrl) ++step.y; else base += step.y; print_grid(); break;
                case key::left:     if (ctrl) --step.x; else base -= step.x; print_grid(); break;
                case key::right:    if (ctrl) ++step.x; else base += step.x; print_grid(); break;

                case key::num_add:  if (ctrl) ++base; else base += 12; print_grid(); break;
                case key::num_sub:  if (ctrl) --base; else base -= 12; print_grid(); break;

                case key::num_mul:
                    scale.fill([] { for (auto&& i : scale) { if (not i) return true; } return false; });
                    print_scale();
                    break;
                }

                static std::unordered_map<key, byte> scale_keys
                {
                    { key::f1,   0 },
                    { key::f2,   1 },
                    { key::f3,   2 },
                    { key::f4,   3 },
                    { key::f5,   4 },
                    { key::f6,   5 },
                    { key::f7,   6 },
                    { key::f8,   7 },
                    { key::f9,   8 },
                    { key::f10,  9 },
                    { key::f11, 10 },
                    { key::f12, 11 }
                };
                if (scale_keys.count(k.first))
                {
                    auto& s = scale[scale_keys[k.first]];
                    s = not s;
                    print_scale();
                }
            }
        } };

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

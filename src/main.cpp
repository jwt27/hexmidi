/* * * * * * * * * * * * * *  hexmidi  * * * * * * * * * * * * * */
/* Copyright (C) 2018 J.W. Jagersma, see COPYING.txt for details */

#include <iostream>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <jw/io/key.h>
#include <jw/vector.h>
#include <jw/io/keyboard.h>
#include <jw/io/ps2_interface.h>
#include <jw/io/mpu401.h>
#include <jw/io/gameport.h>
#include <jw/split_stdint.h>

namespace jw
{
    using namespace io;
    using split_int14_t = split_int<signed, 14>;
    using split_uint14_t = split_int<unsigned, 14>;

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
        std::cout << "Scale: ";
        for (int i = 0; i < 12; ++i)
            if (scale[i]) std::cout << names[i] << ' ';
        std::cout << std::endl;
    }

    void enable_joystick(std::optional<gameport<>>& joy)
    {
        gameport<>::config gameport_cfg { };
        gameport_cfg.strategy = gameport<>::poll_strategy::busy_loop;
        gameport_cfg.enable.x1 = false;
        gameport_cfg.enable.y1 = false;
        gameport_cfg.output_range.min.x = -8192;
        gameport_cfg.output_range.min.x = 8191;
        gameport_cfg.output_range.min.y = 0;
        gameport_cfg.output_range.min.y = 127;

        std::cout << "calibrate joystick, press fire when done.\n";
        {
            io::gameport<> joystick { gameport_cfg };
            std::swap(gameport_cfg.calibration.x0_max, gameport_cfg.calibration.x0_min);
            std::swap(gameport_cfg.calibration.y0_max, gameport_cfg.calibration.y0_min);
            while (true)
            {
                auto raw = joystick.get_raw();
                gameport_cfg.calibration.x0_min = std::min(gameport_cfg.calibration.x0_min, raw.x0);
                gameport_cfg.calibration.y0_min = std::min(gameport_cfg.calibration.y0_min, raw.y0);
                gameport_cfg.calibration.x0_max = std::max(gameport_cfg.calibration.x0_max, raw.x0);
                gameport_cfg.calibration.y0_max = std::max(gameport_cfg.calibration.y0_max, raw.y0);

                auto[a0, b0, a1, b1] = joystick.buttons();
                if (a0 or b0 or a1 or b1) break;
            }
        }
        gameport_cfg.strategy = gameport<>::poll_strategy::thread;

        joy.emplace(gameport_cfg);
    }

    void hexmidi()
    {
        std::cout << "Initializing... " << std::flush;
        scale.fill(true);

        using namespace std::chrono_literals;
        chrono::setup::setup_pit(true, 0x1000);
        chrono::setup::setup_tsc(10000);

        thread::yield_for(2s);

        mpu401_stream mpu { mpu401_config { } };
        keyboard keyb { std::make_shared<ps2_interface>() };
        std::optional<gameport<>> joy { };

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
                        if (ctrl) break;
                        auto g = key_grid.at(k.first);
                        byte event = k.second == key_state::down ? 0x90 : 0x80;
                        byte note = base + step.x * g.x + step.y * g.y;
                        if (alt and k.second.is_down())
                        {
                            auto& s = scale[note % 12];
                            s = not s;
                            print_scale();
                        }
                        byte vel = joy ? joy->get().y : 100;
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
                    scale.fill([] { for (auto&& i : scale) { if (not i) return true; } return false; }());
                    print_scale();
                    break;

                case key::j:
                    if (not ctrl) break;
                    if (joy)
                    {
                        joy.reset();
                        std::cout << "Joystick disabled." << std::endl;
                    }
                    else
                    {
                        enable_joystick(joy);
                        std::cout << "Joystick enabled." << std::endl;
                    }
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
        vector4f joy_value { }, last_joy_value;
        std::cout << "ready.\n" << std::endl;

        while (running)
        {
            if (joy)
            {
                last_joy_value = joy_value;
                joy_value = joy->get();
                if (joy->cfg.enable.x0 and joy_value.x != last_joy_value.x)
                {
                    byte event = 0xb0;
                    split_uint14_t value = joy_value.x;
                    byte cc = 1;    // modulation
                    byte v = value.hi;
                    mpu << event << cc << v;
                    cc = 1 + 32;
                    v = value.lo;
                    mpu << event << cc << v << std::flush;
                }
                if (joy->cfg.enable.y0 and joy_value.y != last_joy_value.y)
                {
                    byte event = 0xd0;  // aftertouch
                    byte value = joy_value.y;
                    mpu << event << value << std::flush;
                }
                if (joy->cfg.enable.x1 and joy_value.z != last_joy_value.z)
                {

                }
                if (joy->cfg.enable.y1 and joy_value.w != last_joy_value.w)
                {

                }
            }
            thread::yield_for(std::chrono::milliseconds { 2 });
        }
    }
}

int jwdpmi_main(std::deque<std::string_view>)
{
    jw::hexmidi();
    return 0;
}

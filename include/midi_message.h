/* * * * * * * * * * * * * *  hexmidi  * * * * * * * * * * * * * */
/* Copyright (C) 2018 J.W. Jagersma, see COPYING.txt for details */

#pragma once
#include <variant>
#include <vector>
#include <iostream>
#include <jw/common.h>
#include <jw/split_stdint.h>
#include <jw/thread/thread.h>

namespace jw
{
    using split_uint14_t = split_int<unsigned, 14>;

    struct midi_message
    {
        using clock = jw::chrono::tsc;

        // tag types
        struct channel_message { byte channel; };
        struct system_message { };
        struct system_common_message : public system_message { };
        struct system_realtime_message : public system_message { };

        // channel message types
        struct note_off : public channel_message { byte note, velocity; };
        struct note_on : public channel_message { byte note, velocity; };
        struct key_pressure : public channel_message { byte note, value; };
        struct channel_pressure : public channel_message { byte value; };
        struct control_change : public channel_message { byte controller, value; };
        struct program_change : public channel_message { byte value; };
        struct pitch_change : public channel_message { split_uint14_t value; };

        // system message types
        struct sysex : public system_common_message { std::vector<byte> data; };
        struct mtc_quarter_frame : public system_common_message { byte data; };    // TODO
        struct song_position : public system_common_message { split_uint14_t value; };
        struct song_select : public system_common_message { byte value; };
        struct tune_request : public system_common_message { };
        struct clock_tick : public system_realtime_message { };
        struct clock_start : public system_realtime_message { };
        struct clock_continue : public system_realtime_message { };
        struct clock_stop : public system_realtime_message { };
        struct active_sense : public system_realtime_message { };
        struct reset : public system_realtime_message { };

        std::variant<
            note_off,
            note_on,
            key_pressure,
            channel_pressure,
            control_change,
            program_change,
            pitch_change,
            sysex,
            mtc_quarter_frame,
            song_position,
            song_select,
            tune_request,
            clock_tick,
            clock_start,
            clock_continue,
            clock_stop,
            active_sense,
            reset> msg;
        typename clock::time_point time;

        midi_message(std::istream& in) { in >> *this; }
        template<typename T> midi_message(T&& m, auto t = clock::now()) : msg(std::forward<T>(m)), time(t) { }

    protected:
        struct stream_writer
        {
            std::ostream& out;

            void operator()(const note_off& msg)            { out.put(0x80 | (msg.channel & 0x0f)); out.put(msg.note);          out.put(msg.velocity); }
            void operator()(const note_on& msg)             { out.put(0x90 | (msg.channel & 0x0f)); out.put(msg.note);          out.put(msg.velocity); }
            void operator()(const key_pressure& msg)        { out.put(0xa0 | (msg.channel & 0x0f)); out.put(msg.note);          out.put(msg.value); }
            void operator()(const control_change& msg)      { out.put(0xb0 | (msg.channel & 0x0f)); out.put(msg.controller);    out.put(msg.value); }
            void operator()(const program_change& msg)      { out.put(0xc0 | (msg.channel & 0x0f)); out.put(msg.value); }
            void operator()(const channel_pressure& msg)    { out.put(0xd0 | (msg.channel & 0x0f)); out.put(msg.value); }
            void operator()(const pitch_change& msg)        { out.put(0xe0 | (msg.channel & 0x0f)); out.put(msg.value.lo);      out.put(msg.value.hi); }

            void operator()(const sysex& msg)               { out.put(0xf0); out.write(reinterpret_cast<const char*>(msg.data.data()), msg.data.size()); out.put(0xf7); }
            void operator()(const mtc_quarter_frame& msg)   { out.put(0xf1); out.put(msg.data); }
            void operator()(const song_position& msg)       { out.put(0xf2); out.put(msg.value.lo); out.put(msg.value.hi); }
            void operator()(const song_select& msg)         { out.put(0xf3); out.put(msg.value); }

            void operator()(const tune_request&)    { out.put(0xf6); }
            void operator()(const clock_tick&)      { out.put(0xf8); }
            void operator()(const clock_start&)     { out.put(0xfa); }
            void operator()(const clock_continue&)  { out.put(0xfb); }
            void operator()(const clock_stop&)      { out.put(0xfc); }
            void operator()(const active_sense&)    { out.put(0xfe); }
            void operator()(const reset&)           { out.put(0xff); }
        };

    public:
        friend std::ostream& operator<<(std::ostream& out, const midi_message& in)
        {
            std::visit(stream_writer { out }, in.msg);
            return out;
        }

        friend std::istream& operator>>(std::istream& in, midi_message& out)
        {
            byte a;
            auto get = [&in] { return static_cast<byte>(in.get()); };
            do { a = get(); } while ((a & 0x80) == 0);
            out.time = clock::now();
            if ((a & 0xf0) != 0xf0)   // channel message
            {
                byte ch = get();
                switch (a & 0xf0)
                {
                case 0x80: out.msg = note_off { { ch }, get(), get() }; break;
                case 0x90: out.msg = note_on { { ch }, get(), get() }; break;
                case 0xa0: out.msg = key_pressure { { ch }, get(), get() }; break;
                case 0xb0: out.msg = control_change { { ch }, get(), get() }; break;
                case 0xc0: out.msg = program_change { { ch }, get() }; break;
                case 0xd0: out.msg = channel_pressure { { ch }, get() }; break;
                case 0xe0: out.msg = pitch_change { { ch }, { get(), get() } }; break;
                }
            }
            else                    // system message
            {
                switch (a)
                {
                case 0xf0:
                {
                    out.msg = sysex { };
                    auto& data = std::get<sysex>(out.msg).data;
                    std::vector<char> extra { };
                    while (true)
                    {
                        a = get();
                        if (a == 0xf7) break;       // end of sysex
                        if ((a & 0x80) == 0) data.push_back(a);
                        else extra.push_back(a);    // single-byte realtime messages may be mixed in
                    }
                    for (auto&& i : extra) in.putback(i);
                    break;
                }
                case 0xf1: out.msg = mtc_quarter_frame { { }, get() }; break;
                case 0xf2: out.msg = song_position { { }, { get(), get() } }; break;
                case 0xf3: out.msg = song_select { { }, get() }; break;
                case 0xf6: out.msg = tune_request { }; break;
                case 0xf8: out.msg = clock_tick { }; break;
                case 0xfa: out.msg = clock_start { }; break;
                case 0xfb: out.msg = clock_continue { }; break;
                case 0xfc: out.msg = clock_stop { }; break;
                case 0xfe: out.msg = active_sense { }; break;
                case 0xff: out.msg = reset { }; break;
                }
            }
            return in;
        }
    };
}

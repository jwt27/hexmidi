# hexmidi
Use your PC keyboard as a hexagonal isomorphic MIDI keyboard.

### System requirements
- FreeDOS / MS-DOS or compatible
- Pentium processor
- ~4MB memory
- MPU-401 compatible MIDI card
- optional: joystick

### Keys

Default key layout on startup:
```
    `     1     2     3     4     3     6     7     8     9     0     -     =
   E-3   F#3   G#3   A#3   C-4   D-4   E-4   F#4   G#4   A#4   C-5   D-5   E-5
         
             Q     W     E     R     T     Y     U     I     O     P     [     ]     \
            D#3   F-3   G-3   A-3   B-3   C#4   D#4   F-4   G-4   A-4   B-4   C#5   D#5
         
                A     S     D     F     G     H     J     K     L     ;     '
               C-3   D-3   E-3   F#3   G#3   A#3   C-4   D-4   E-4   F#4   G#4
         
                   Z     X     C     V     B     N     M     ,     .     /
                  A-2   B-2   C#3   D#3   F-3   G-3   A-3   B-3   C#4   D#4
```

Control keys:
```
       arrow keys: shift key grid up/down/left/right
ctrl + arrow keys: adjust interval between keys
       numpad +/-: shift grid one octave up/down
ctrl + numpad +/-: shift grid one semitone up/down
            space: sustain
           f1-f12: disable/enable notes to form a scale
       alt + note: same as f1-f12
         numpad *: enable all notes
	 ctrl + j: enable joystick
```

Joystick:
```
    x axis: modulation
    y axis: velocity + aftertouch
```

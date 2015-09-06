# Cellular Automaton Engine 2D in OpenGL

![Cellular Automaton Engine in action](https://i.imgur.com/Zdeuliw.png)

## Simulation Modes

- Conway's Game of Life
- modified version of Conway's Game of Life (they gain/lose size and colour instead of on/off)

See it in action: 
- version 1.5 screencast: https://youtu.be/oU1YTF9n-4E
- version 1.5 realtime: https://youtu.be/04eWncAXFMs
- version 1.0: https://youtu.be/vocNmgc3i_c

## Update 1.5

Changes:

- 3D cubes
- Lighting  (2 sources)
- Lots of nice colours
- hidden HUD (shift+i to toggle)

## Keys

- [F1] fullscreen/windowed
- [ESC] exit app
- [ENTER] reset simulation
- [SHIFT]+[i] or [I] toggle HUD
- [SPACEBAR] change modes 

## Free code

- C++
- OpenGL
- FreeGLUT

## Download binary

- Linux 32bit: [http://pub.p1x.in/ogl-linux-32bit.app](http://pub.p1x.in/ogl-linux-32bit.app)

## Compile
Make shure to have OpenGL, FreeGLUT installed.
```
cd cellular-automaton
gcc -Os ca2d.cpp -o ca2d.app -lglut -lGL -lGLU -lm
./ca2d.app
```

# Cellular Automaton Engine 3D in OpenGL

New engine. TBD


## The MIT License (MIT)

Copyright (c) 2015 P1X

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

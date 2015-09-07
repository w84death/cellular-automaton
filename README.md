![Cellular Automaton 3D](http://i.imgur.com/Ld4KPsA.png)
# Cellular Automaton Engine 3D in OpenGL

## Simulation Modes

- modified version of Conway's Game of Life (they gain/lose size and colour instead of on/off)
- added third dimension

## Download

- Linux 32bit: [ca2d-osx](https://github.com/w84death/cellular-automaton/raw/master/bin/ca3d-linux.app)
- OSX 64bit: [ca3d-osx](https://github.com/w84death/cellular-automaton/raw/master/bin/ca3d-osx)

## Usage
- [F1] fullscreen/windowed
- [F2] reset array (random)
- [F3, F4, F5] move camera to point 1, 2, 3
- [WSAD] move camera forward/bacward/left/right
- [QE] up/down
- [ARROWS] move target of the camera

## Media
Video: [https://youtu.be/YsIRFvRxoEs](https://youtu.be/YsIRFvRxoEs)

![Cellular Automaton 2D](http://i.imgur.com/UpUECZ1.png)
# Cellular Automaton Engine 2D in OpenGL

## Simulation Modes

- Conway's Game of Life
- modified version of Conway's Game of Life (they gain/lose size and colour instead of on/off)

## Download

- Linux 32bit: [http://pub.p1x.in/ogl-linux-32bit.app](http://pub.p1x.in/ogl-linux-32bit.app)

## Usage
- [F1] fullscreen/windowed
- [ESC] exit app
- [ENTER] reset simulation
- [SHIFT]+[i] or [I] toggle HUD
- [SPACEBAR] change modes

## Media
Videos: [https://youtu.be/04eWncAXFMs](https://youtu.be/04eWncAXFMs) [https://youtu.be/oU1YTF9n-4E](https://youtu.be/oU1YTF9n-4E)


# Compile
## Linux
Make shure to have OpenGL, FreeGLUT installed.

```
gcc -Os ca3d.cpp -o ca3d.app -lglut -lGL -lGLU -lm
./ca2d.app
```

## OSX
Only OpenGL needed.

```
gcc -o ca3d ca3d.cpp -framework GLUT -framework OpenGL
./ca3d
```




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

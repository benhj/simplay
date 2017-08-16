# simplay

This forms the beginnings of a re-write of an Artificial Life simulator I wrote during the latter parts of my PhD and later, during a following post-doc. The original aim of that work was a study of nervous-system/body-plan co-evolution in models of soft-bodied worm-like creatures. I decided to rewrite the software as the original was so buggy and crufty it was starting to become a headache to maintain. This version will be far cleaner and from a software architectural perspective, hopefully a bit more elegant.

A video of an early version:

[![demo](https://github.com/benhj/simplay/blob/master/animatTest.mov)]

## The plan

The plan is to implement the following stuff. I will cross items out as I make progress.

* ~~Basic animat model.~~
* ~~Simple soft-bodied physics engine composed of springs and point masses.~~
* ~~A water force generator to simlate water drag and viscosity.~~
* ~~CTRNN implementation for eventual control of an animat.~~
* ~~Antennae model.~~
* Collision detection and resolution.
* ~~Main simulation engine~~
* ~~Optimization process~~
* GUI enhancements (T.B.D.)
* ~~Environment Zoom.~~
* ~~Environment rotate.~~
* ~~Central axis.~~
* ~~Animat highlight.~~
* ~~Compass when rotate.~~

## To build

From the source root:

```
mkdir build
cd build
cmake ..
make
```


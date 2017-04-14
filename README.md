# simplay

This forms the beginnings of a re-write of a simulator I wrote during my PhD and later, during a following post-doc. 
The original aim of that work was a study of nervous-system/body plan co-evolution in models of soft-bodied worm-like creatures. I decided to rewrite the software as the original was so buggy and crufty it was starting to become a headache to maintain. This version will be far cleaner and from a software architectural perspective, hopefully a bit more elegant.

## The plan

The plan is to implement the following stuff. I will cross items out as I make progress.

* ~~Simple soft-bodied physics engine composed of springs and point masses.~~
* ~~A water force generator to simlate water drag and viscosity.~~
* ~~CTRNN implementation for eventual control of an animat.~~
* Collision detection and resolution.
* Main simulation engine
* Optimization process
* GUI enhancements (T.B.D.)

## To build

From the source root:

```
mkdir build
cd build
cmake ..
make
```


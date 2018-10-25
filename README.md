# Shovel Engine

A very work-in-progress voxel engine.

My first and only major C/C++ project so far.

Written with a very limited C-like subset of C++. No OOP, just ECS.

The one notable thing about this is the greedy mesher that I got to mesh chunks of 64^3 voxels in 10-15ms on a Pentium G4560, or 512x256x512 in 600ms on a Ryzen 1600 (FYI that is really damn fast) 

Built using old school Makefiles, compiled with GCC (MinGW-w64), written in Sublimetext 3. I can't stand MSVC.


# Images
Greedy meshing and MC-style baked AO
![image](https://kosshi.net/u/qVUI.png)
380FPS with a RX480
![image](https://kosshi.net/u/NfMb.png)
Debugging player collisions and text rendering
![image](https://kosshi.net/u/Q3CQ.png)

# Other
Due to my foolish use of proprietary Consolas font and the questionable inclusion of the libraries used, this shouldn't be public here at all (and wasn't until my github educational license ran out). I cannot give this a free license for now.

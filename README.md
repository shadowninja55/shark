# shark
ever wanted an x86-64 kernel to play snake on bare metal? no? well too bad!

## run
`make run` to try out the game in qemu

## build
`make` to build `shark.iso`, which boots on real hardware! (at least on my hardware that is)

## gameplay
`wasd` to move, `r` to restart after death

## details
- clock, keyboard, and video "drivers"
- smart game rendering; no full redraws!
- bare-bones prng

all interesting code is in `kernel/src`

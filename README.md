## FDC Programming
`Step Rate`: How fast the head moves between adjacent tracks

`Head Load Time`: The delay while the head is being lowered (engaged) onto the disk surface and the drive lets it stabilize before data transfer starts.
- Think of it like: the arm moves down, the spring/solenoid presses the head onto the spinning media, then you wait a few milliseconds so it’s stable.

`Head unload time`: The delay while the head is being kept down after a command ends, before lifting it off.
- Purpose: if you’re about to do another read/write soon, you don’t want to lift and immediately drop again.
- After this timeout expires (if no new command arrives), the FDC disengages the head (lifts it back up).

`Load`   = time to drop down and settle.
`Unload` = time to hold down before lifting.

Example: at C=4, H=0, S=5 : want to move to C=9, H=1, S=30

1. Seek:
    - Steps from C4 -> C9 = 6steps
    - Delay of step-rate between each step so the head settles
2. Select head H = 1
3. Head load time wait once before I/O
4. Rotational latenc: the controller watches the IDs the track spins until S = 30
5. Read/Write sector via DMA/PIO
6. Command ends → head unload time counts once, then the head may lift
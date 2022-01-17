# Ohj2-GUI-Hano

Admittedly a few questionable design choices here, which I would change if I rewrote this. Live and learn. Still showcases a bit of actual work done with the graphical side of QT.

Probably single biggest "no-no" is clearing the whole scene and re-drawing everything each time a disk is moved from
one pin to another. The disks are presented as rectItems. In my defense deleting the rectItem or using removeGraphicItem(rectItem) crashed the program. Could not get the setpos to work either. Maybe should have another look now...

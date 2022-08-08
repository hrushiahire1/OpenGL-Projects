cls

del OGL.exe

del OGL.res

del OGL.obj

cl.exe /c /EHsc OGL.cpp

rc.exe OGL.rc

link.exe OGL.obj OGL.res user32.lib gdi32.lib kernel32.lib winmm.lib /SUBSYSTEM:WINDOWS /MACHINE:x64

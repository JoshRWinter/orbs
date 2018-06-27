rc resource.rc
cl /EHsc /IC:\users\josh\desktop\SDL2-2.0.8\include /std:c++17 *.cpp SDL2.lib opengl32.lib resource.res /link /out:Orbs.scr

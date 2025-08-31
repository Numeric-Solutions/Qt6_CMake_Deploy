I ran the command
```
cmake --install build-release --prefix ~/Qt6_CMake_Deploy_Package
```
And it seemed to work.  When I ran the deployed application:
```
cd ~/Qt6_CMake_Deploy_Package/bin
./Qt6_CMake_Deploy
```
The application ran as it should.  However, when I copied it to my target computer (a desktop PC with a clean Linux Mint install) and run the application, I get the following error:

```
➜  bin ./Qt6_CMake_Deploy
qt.qpa.plugin: From 6.5.0, xcb-cursor0 or libxcb-cursor0 is needed to load the Qt xcb platform plugin.
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: xcb.

[1]    3861 IOT instruction (core dumped)  ./Qt6_CMake_Deploy
```

# 2025.08.29 Using linuxdeployqt
After running 
```bash
./linuxdeployqt-continuous-x86_64.AppImage ~/Qt6_Deploy_Complete/usr/bin/Qt6_CMake_Deploy -extra-plugins=iconengines,platformthemes -no-translations -verbose=2 -unsupported-allow-new-glibc
```
I get the following error:
```bash
➜  Qt6_CMake_Deploy_Portable ./AppRun
qt.qpa.plugin: From 6.5.0, xcb-cursor0 or libxcb-cursor0 is needed to load the Qt xcb platform plugin.
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: xcb.

[1]    3109 IOT instruction (core dumped)  ./AppRun
```
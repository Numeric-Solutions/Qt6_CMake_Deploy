This is an example of how to build a project in `Qt6/C++`, using all the 
modern featuers, `QGraphics`, `QWidgets` and `QCharts` (but not `QML`).  This 
example also uses the recommended build system:  `CMake`/`Ninja` and deploys the 
project using the modern `Qt6`/`CMake` method, which is then used to create an `AppImage` 
using `LinuxDeploy`.

# How to Build Project
### Qt Creator:
   Open `CMakeLists.txt` in `Qt Creator` and select `Build/Build Project` or `Ctr + B`
   To run 


### Clean Build and Test

```zsh
rm -rf build
cmake -G Ninja -S ./ -B ./build
cmake --build build --parallel
./build/Qt6_CMake_Deploy
```
this is a basic clean build and test of the example project.

### Build for Deployment usign CMake/Ninja:

1. **Delete build, Configure with Ninja, Build with Ninja, and Install/Deploy, and Test:**
```zsh
rm -rf build Qt6_CMake_Deploy
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cmake --install build --prefix "$PWD/Qt6_CMake_Deploy" &&
./build/Qt6_CMake_Deploy
```
This creates an install/deploy folder `Qt6_CMake_Deploy`, which in theory 
should be ready to deploy.  However, this does not copy all the dependencies for 
this example.  Which means you will have to track down the missing dependencies 
(see `https://github.com/Numeric-Solutions/Julia_Dependency_Tools` for additional help).
Or use `LinuxDeploy` to make an AppImage

# (Optional) Find all Dependencies
```zsh
cd Qt6_CMake_Deploy/bin
ldd ./Qt6_CMake_Deploy > dependencies.txt
```
This does not include run-time dependencies.
For a more complete analysis, including run-time deps, see `Julia_Dependency_Tools`
```zsh
https://github.com/Numeric-Solutions/Julia_Dependency_Tools
```

# Using LinuxDeploy to make an AppImage
so far, this is the only method that has worked completely for this example on my target PC


### Set UP Environment:

1. **Ensure Qt is in PATH and Export QMAKE:**
   ```zsh
   export PATH=/home/nathan/Qt/6.9.2/gcc_64/bin:$PATH
   export QMAKE=/home/nathan/Qt/6.9.2/gcc_64/bin/qmake
   ```
   Add the above exports to your `.zshrc` file to make the above exports permanent

2. **Download LinuxDeploy and Plugin**:
   ```zsh
   prev="$PWD"
   mkdir -p ~/DeploymentTools/linuxdeploy/
   cd ~/DeploymentTools/linuxdeploy/
   wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
   chmod +x linuxdeploy-x86_64.AppImage
   wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
   chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
   cd "$prev"
   ```
   Change download directory (`~/DeploymentTools/linuxdeploy/`) to whatever you like

### Build Project and create CMake/Install/Deploy Folder

Follow the steps in 'Build for Deployment usign CMake/Ninja'
<!-- ```zsh
rm -rf build Qt6_CMake_Deploy
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cmake --install build --prefix "$PWD/Qt6_CMake_Deploy"
``` -->

### Create AppDir structure (AppDir)
```zsh
rm -rf AppDir
mkdir -p AppDir/usr/bin
mkdir -p AppDir/usr/lib
mkdir -p AppDir/usr/share/applications
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps/
```

### Copy your executable, qss file, /lib/, /plugins/, and icon
```zsh
cp Qt6_CMake_Deploy/bin/Qt6_CMake_Deploy AppDir/usr/bin/
cp Qt6_CMake_Deploy/bin/QTdark_PlotWin.qss AppDir/usr/bin/
cp -r Qt6_CMake_Deploy/lib AppDir/usr/lib/
cp -r Qt6_CMake_Deploy/plugins AppDir/usr/lib/
cp ./ns256.png AppDir/usr/share/icons/hicolor/256x256/apps/Qt6_CMake_Deploy.png
```

### Create a desktop file
```zsh
cat > AppDir/usr/share/applications/Qt6_CMake_Deploy.desktop << EOF
[Desktop Entry]
Type=Application
Name=Qt6 CMake Deploy
Exec=Qt6_CMake_Deploy
Icon=Qt6_CMake_Deploy
Categories=Utility;
Comment=Qt6 CMake Deploy Application
EOF
```

### create a symlink of the .desktop and icon files in the AppDir's top level
```zsh
cd AppDir
ln -s usr/share/applications/Qt6_CMake_Deploy.desktop Qt6_CMake_Deploy.desktop
ln -s usr/bin/Qt6_CMake_Deploy AppRun
ln -s usr/share/icons/hicolor/256x256/apps/Qt6_CMake_Deploy.png Qt6_CMake_Deploy.png
cd ..
```

### Run LinuxDeploy
cd into `path/to/linuxdeploy/` and run `linuxdeploy`
```zsh
prev="$PWD"
dep="$prev/AppDir"
icon="$prev/AppDir/usr/share/icons/hicolor/256x256/apps/Qt6_CMake_Deploy.png"
cd ~/DeploymentTools/linuxdeploy/
./linuxdeploy-x86_64.AppImage --appdir "$dep" --plugin qt --output appimage --icon-file "$icon"
mv Qt6_CMake_Deploy-x86_64.AppImage ~/
cd "$prev"
```

### Test AppImage
```zsh
~/Qt6_CMake_Deploy-x86_64.AppImage
```

### (Optional) Copy AppImage to Target Computer
```zsh
scp -r ~/Qt6_CMake_Deploy-x86_64.AppImage name@192.168.1.xxx:/home/name/
```

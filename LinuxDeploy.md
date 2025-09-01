# Using LinuxDeploy to make an AppImage

## Set UP Environment:

1. **Ensure Qt is in PATH:**
`export PATH=/home/nathan/Qt/6.9.2/gcc_64/bin:$PATH`

2. **Export QMAKE:** 
`export QMAKE=/home/nathan/Qt/6.9.2/gcc_64/bin/qmake`

1. **Download LinuxDeploy**:
   ```
   prev="$PWD"
   mkdir -p ~/DeploymentTools/linuxdeploy/
   cd ~/DeploymentTools/linuxdeploy/
   wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
   chmod +x linuxdeploy-x86_64.AppImage
   wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
   chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
   cd "$prev"
   ```

## Build Project
```zsh
rm -rf build-release
cmake -G Ninja -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
cmake --install build-release --prefix ~/Qt6_CMake_Deploy_Package
```

## Create AppDir structure
```zsh
rm -rf ~/Qt6_Deploy_Complete
mkdir -p ~/Qt6_Deploy_Complete/usr/bin
mkdir -p ~/Qt6_Deploy_Complete/usr/lib
mkdir -p ~/Qt6_Deploy_Complete/usr/share/applications
mkdir -p ~/Qt6_Deploy_Complete/usr/share/icons/hicolor/256x256/apps/
```

## Copy your executable, qss file, /lib/, /plugins/, and icon
```zsh
cp ~/Qt6_CMake_Deploy_Package/bin/Qt6_CMake_Deploy ~/Qt6_Deploy_Complete/usr/bin/
cp ~/Qt6_CMake_Deploy_Package/bin/QTdark_PlotWin.qss ~/Qt6_Deploy_Complete/usr/bin/
cp -r ~/Qt6_CMake_Deploy_Package/lib ~/Qt6_Deploy_Complete/usr/lib/
cp -r ~/Qt6_CMake_Deploy_Package/plugins ~/Qt6_Deploy_Complete/usr/lib/
cp ./ns256.png ~/Qt6_Deploy_Complete/usr/share/icons/hicolor/256x256/apps/Qt6_CMake_Deploy.png
```

## Create a desktop file
```zsh
cat > ~/Qt6_Deploy_Complete/usr/share/applications/Qt6_CMake_Deploy.desktop << EOF
[Desktop Entry]
Type=Application
Name=Qt6 CMake Deploy
Exec=Qt6_CMake_Deploy
Icon=Qt6_CMake_Deploy
Categories=Utility;
Comment=Qt6 CMake Deploy Application
EOF
```

## create a symlink of the .desktop and icon files in the AppDir's top level
```zsh
cd ~/Qt6_Deploy_Complete/
ln -s usr/share/applications/Qt6_CMake_Deploy.desktop ~/Qt6_Deploy_Complete/Qt6_CMake_Deploy.desktop
ln -s usr/bin/Qt6_CMake_Deploy ~/Qt6_Deploy_Complete/AppRun
ln -s usr/share/icons/hicolor/256x256/apps/Qt6_CMake_Deploy.png ~/Qt6_Deploy_Complete/Qt6_CMake_Deploy.png
cd ~/Documents/Qt/Qt6_CMake_Deploy/
```

## Run LinuxDeploy
cd into `path/to/linuxdeploy/` and run `linuxdeploy`
```zsh
cd ~/DeploymentTools/linuxdeploy/
./linuxdeploy-x86_64.AppImage --appdir ~/Qt6_Deploy_Complete --plugin qt --output appimage --icon-file ~/Qt6_Deploy_Complete/usr/share/icons/hicolor/256x256/apps/Qt6_CMake_Deploy.png
mv Qt6_CMake_Deploy-x86_64.AppImage ~/
cd ~/Documents/Qt/Qt6_CMake_Deploy
```

## Copy AppImage to target computer
```zsh
scp -r ~/Qt6_CMake_Deploy-x86_64.AppImage reflexion@192.168.1.200:/home/reflexion/
```


# Clean Build in /bin
`rm -rf build && cmake -G Ninja -S ./ -B ./build && cmake --build build --parallel`

# FOR DEPLOYMENT:

## Delete /build-release, if it exists
`rm -rf build-release`

## 1. Configure with Ninja
`cmake -G Ninja -S . -B build-release -DCMAKE_BUILD_TYPE=Release`

## 2. Build with Ninja
`cmake --build build-release`

## 3. Install/Deploy
`cmake --install build-release --prefix ~/Qt6_CMake_Deploy_Package`

# Find all Dependencies
```bash
cd ~/Qt6_CMake_Deploy_Package/bin
ldd ./Qt6_CMake_Deploy > dependencies.txt
```

# Using LinuxDeployQt

## Download linuxdeployqt
```bash
# Download linuxdeployqt
wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod +x linuxdeployqt-continuous-x86_64.AppImage
```

## Create a proper deployment structure
```bash
# Create AppDir structure
mkdir -p ~/Qt6_Deploy_Complete/usr/bin
mkdir -p ~/Qt6_Deploy_Complete/usr/lib
mkdir -p ~/Qt6_Deploy_Complete/usr/share/applications

# Copy your executable
cp ~/Qt6_CMake_Deploy_Package/bin/Qt6_CMake_Deploy ~/Qt6_Deploy_Complete/usr/bin/
cp ~/Qt6_CMake_Deploy_Package/bin/QTdark_PlotWin.qss ~/Qt6_Deploy_Complete/usr/bin/

# Create a desktop file
cat > ~/Qt6_Deploy_Complete/usr/share/applications/Qt6_CMake_Deploy.desktop << EOF
[Desktop Entry]
Type=Application
Name=Qt6 CMake Deploy
Exec=Qt6_CMake_Deploy
Icon=Qt6_CMake_Deploy
Categories=Utility;
EOF
```

## Make sure Qt is in your path
```bash
export PATH=/home/nathan/Qt/6.9.2/gcc_64/bin:$PATH
```

## Deploy with all dependencies
```bash
./linuxdeployqt-continuous-x86_64.AppImage ~/Qt6_Deploy_Complete/usr/bin/Qt6_CMake_Deploy -extra-plugins=iconengines,platformthemes -no-translations -verbose=2 -unsupported-allow-new-glibc
```
`-unsupported-allow-new-glibc` was added to support newer Linux host systems

## Create tarball
```bash
cd ~/Qt6_Deploy_Complete
tar -czf ~/Qt6_CMake_Deploy_Portable.tar.gz .
```
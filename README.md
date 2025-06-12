# README #

See https://tkasozi.github.io/ITools/

### Deps management using vcpkg.

./vcpkg install curl:x64-windows
./vcpkg install minizip:x64-windows
./vcpkg install boost-property-tree:x64-windows
 ./vcpkg install qtbase:x64-windows

 windeployqt --basedir "C:\vcpkg\installed\x64-windows\tools\qt6" --debug --no-translations --no-opengl-sw ITools.exe
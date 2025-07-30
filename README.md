# README #

See https://tkasozi.github.io/Buraq/

### Deps management using vcpkg.

./vcpkg install curl:x64-windows
./vcpkg install minizip:x64-windows
./vcpkg install boost-property-tree:x64-windows
 ./vcpkg install qtbase:x64-windows

### Navigate to your project's root directory (where vcpkg.json is) and run:

vcpkg install --triplet=x64-mingw-dynamic --host-triplet=x64-mingw-dynamic

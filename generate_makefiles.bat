del cmake_install.cmake Makefile .cproject .project

rd /s /q CMakeFiles

del /s /q include/moc_*

cmake -DCMAKE_BUILD_TYPE=Debug -G "Eclipse CDT4 - MinGW Makefiles"

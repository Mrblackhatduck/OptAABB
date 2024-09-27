::C:/msys64/mingw64/bin/
g++.exe -g -std=c++17 -I./Includes/ -I./Dependencies/glm/ -I./Dependencies/GL4.3/include/ -I./Dependencies/ -L./Libs/ ./Dependencies/GL4.3/src/glad.c ./**.cpp -lglfw3dll -o build/output && cd build && output.exe && PAUSE && cd ..

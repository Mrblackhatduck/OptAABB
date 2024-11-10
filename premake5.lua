workspace "AABBALgo"
    architecture "x86_64"
    configurations {"Debug","Release"}
    
    project"OptAABB"
        language "C++"
        cppdialect "C++17"
        kind "ConsoleApp"
        includedirs{
            "./Includes/",
            "./Dependencies/glm/",
            "./Dependencies/GL4.3/include/", 
            "./Dependencies/stb_image/", 
            "./Dependencies/tinyGltf/", 
            "./Dependencies/"
        }
        files
        {
            "./Includes/**.h",
            "./Includes/**.hpp",
            "./Dependencies/GL4.3/src/glad.c",
            "./*.cpp",
            "./**.glsl",
            "./**.frag",
            "./**.vert",
        }
        libdirs
        {
            "./Libs/lib-vc2019"
        }
        links{
            "glfw3dll.lib"
        }
        targetdir "./build"
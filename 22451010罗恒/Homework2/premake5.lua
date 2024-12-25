workspace "Homework2"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    startproject "Homework2"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    IncludeDir = {}
    IncludeDir["GLFW"] = "../vendor/openglEnv/GLFW/glfw/include"
    IncludeDir["GLAD"] = "../vendor/openglEnv/GLAD/include"
    IncludeDir["GLM"] = "../vendor/openglEnv/glm-1.0.1"
    IncludeDir["stbi"] = "../vendor/openglEnv/stbimage"

    include "../vendor/openglEnv/GLAD"
    project "Homework2"
        location "Homework2"
        kind "ConsoleAPP"
        language "C++"
        cppdialect "C++17"
        staticruntime "off"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/shader/**.vert",
            "%{prj.name}/src/shader/**.frag",
            "../vendor/openglEnv/stbimage/**.cpp",
            "../vendor/openglEnv/stbimage/**.h"

        }
        includedirs
        {
            "%{prj.name}/src",
            "%{IncludeDir.GLFW}",
            "%{IncludeDir.GLAD}",
            "%{IncludeDir.GLM}",
            "%{IncludeDir.stbi}"
        }
        links
        {
            "../vendor/openglEnv/GLFW/glfw/build/src/Debug/glfw3.lib",
            "Glad",
            "../vendor/openglEnv/assimp-5.4.3/build/lib/Debug/assimp-vc142-mtd.lib"
        }
        defines{
            "GLFW_INCLUDE_NONE"
        }

        filter "system:window"
            cppdialect "C++17"
            staticruntime "off"
            systemversion "latest"

        filter "configurations:Debug"
            symbols "on"

        filter "configurations:Release"
            optimize "on"


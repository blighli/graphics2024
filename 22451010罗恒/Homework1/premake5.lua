workspace "Homework1"
    configurations {"Debug", "Release"}
    architecture "x86_64"
    startproject "Homework1"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    IncludeDir = {}
    IncludeDir["GLFW"] = "../vendor/openglEnv/GLFW/glfw/include"
    IncludeDir["GLAD"] = "../vendor/openglEnv/GLAD/include"
    IncludeDir["GLM"] = "../vendor/openglEnv/glm-1.0.1"
    IncludeDir["Assimp"] = "../vendor/openglEnv/assimp-5.4.3/include"
    include "../vendor/openglEnv/GLAD"
    project "Homework1"
        location "Homework1"
        kind "ConsoleAPP"
        language "C++"
        cppdialect "C++17"
        staticruntime "off"
        targetdir ("bin/" ..outputdir .. "/%{prj.name}")
        objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/shader/**.vs",
            "%{prj.name}/src/shader/**.fs"
        }

        includedirs
        {
            "%{IncludeDir.GLFW}",
            "%{IncludeDir.GLAD}",
            "%{prj.name}/src",
            "%{IncludeDir.GLM}",
            "%{IncludeDir.Assimp}"
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
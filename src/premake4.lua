#!lua

-- Additional Linux libs: "X11", "Xxf86vm", "Xi", "Xrandr", "stdc++"

includeDirList = {
    "../shared",
    "../shared/include",
    "../shared/gl3w",
    "../shared/imgui",
    "../shared/lodepng",
    "../shared/irrKlang-64bit-1.6.0/include",
    "include"
}

libDirectories = { 
    "../lib",
    "../shared/irrKlang-64bit-1.6.0/bin/macosx-gcc"
}


if os.get() == "macosx" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "lodepng",
        "glfw3",
        "lua",
        "irrklang"
    }
end

if os.get() == "linux" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua",
        "IrrKlang",
        "GL",
        "Xinerama",
	"lodepng",
        "Xcursor",
        "Xxf86vm",
        "Xi",
        "Xrandr",
        "X11",
        "stdc++",
        "dl",
        "pthread"
    }
end

-- Build Options:
if os.get() == "macosx" then
    linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL", "-lirrklang" }
end
if os.get() == "linux" then
    linkOptionList = { "-Wl,-R../lib" }
end


buildOptions = {"-std=c++17", "-O2"}

solution "CS488-Projects"
    configurations { "Debug", "Release" }

    project "game"
        kind "ConsoleApp"
        language "C++"
        location "build"
        objdir "build"
        targetdir "."
        buildoptions (buildOptions)
        libdirs (libDirectories)
        links (linkLibs)
        linkoptions (linkOptionList)
        includedirs (includeDirList)
        files { "src/*.cpp" }
        flags {"staticruntime"}

    configuration "Debug"
        defines { "DEBUG", "DLL_EXPORT" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" ,"DLL_EXPORT"}
        flags { "Optimize" }

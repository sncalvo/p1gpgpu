workspace "Practico1"
  configurations { "Debug", "Release" }

  location "build"

project "Practico1"
  kind "ConsoleApp"
  language "C"

  targetdir "bin/%{cfg.buildcfg}"

  files { "%{prj.name}/**.h", "%{prj.name}/**.c" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
    buildoptions { "-O3 -mtune=native -funroll-loops" }

solution "wonton"
	configurations { "Debug", "Release" }
	
	project "wonton"
		kind "ConsoleApp"
		language "C"
		files { "src/*.c" }
		includedirs { "include",
					   "../oauth" }
        links{ "pthread" }
        targetdir ("bin/")
		
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }  

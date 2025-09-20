add_rules("mode.debug")

target("main")
	set_kind("binary")
	add_files("*.cpp")
	add_includedirs("include/")
	add_linkdirs("./")
	add_links("raylib", "rlImGui")
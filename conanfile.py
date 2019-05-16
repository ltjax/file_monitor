from conans import ConanFile, CMake, tools


class FilemonitorConan(ConanFile):
    name = "file_monitor"
    version = "0.1"
    license = "MIT"
    author = "Marius Elvert marius.elvert@googlemail.com"
    url = "https://github.com/ltjax/file_monitor"
    description = "Lean library to observe file changes in a specific directory path."
    topics = ("hot-loading", "file-monitoring")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    exports_sources = "*"
    requires = "boost_filesystem/1.69.0@bincrafters/stable", "boost_iostreams/1.69.0@bincrafters/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".", defs={'file_monitor_USE_CONAN': 'ON'})
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="hello")
        self.copy("*file_monitor.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["file_monitor"]


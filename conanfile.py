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
    exports_sources = "source/*", "tests/*", "CMakeLists.txt"
    requires = "boost_filesystem/1.69.0@bincrafters/stable", "boost_range/1.69.0@bincrafters/stable", "boost_algorithm/1.69.0@bincrafters/stable"
    
    def _configured_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".", defs={'file_monitor_USE_CONAN': 'ON'})
        return cmake

    def requirements(self):
        if self.settings.os == "Macos":
            self.requires("boost_iostreams/1.69.0@bincrafters/stable")

    def build(self):
        self._configured_cmake().build()

    def package(self):
        self._configured_cmake().install()

    def package_info(self):
        self.cpp_info.libs = ["file_monitor"]


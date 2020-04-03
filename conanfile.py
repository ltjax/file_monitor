from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration


class FilemonitorConan(ConanFile):
    name = "file_monitor"
    version = "1.0"
    license = "MIT"
    author = "Marius Elvert marius.elvert@googlemail.com"
    url = "https://github.com/ltjax/file_monitor"
    description = "Lean library to observe file changes in a specific directory path."
    topics = ("hot-loading", "file-monitoring")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False],
               "filesystem": ["boost", "c++17"]}
    default_options = {"shared": False,
                       "filesystem": "c++17"}
    generators = "cmake"
    exports_sources = "source/*", "tests/*", "CMakeLists.txt"
    
    def _configured_cmake(self):
        cmake = CMake(self)
        defs={'file_monitor_USE_CONAN': 'ON',
              'file_monitor_BUILD_TESTS': 'OFF',
              'file_monitor_USE_BOOST': self.options.filesystem == "boost",
              'file_monitor_USE_CXX17': self.options.filesystem == "c++17"}
        cmake.configure(source_folder=".", defs=defs)
        return cmake

    def configure(self):
        if self.options.filesystem == "c++17":
            if self.settings.compiler == "gcc" and tools.Version(self.settings.compiler.version) < "8":
                raise ConanInvalidConfiguration("Using <filesystem> requires at least g++ 8")
            if self.settings.compiler == "apple-clang" and tools.Version(self.settings.compiler.version) < "11.0":
                raise ConanInvalidConfiguration("Using <filesystem> requires at least apple-clang 11.0")


    def requirements(self):
        if self.options.filesystem == "boost":
            boost = ["boost_filesystem/1.69.0@bincrafters/stable",
                     "boost_range/1.69.0@bincrafters/stable",
                     "boost_algorithm/1.69.0@bincrafters/stable"]
            for each in boost:
                self.requires(each)
            if self.settings.os == "Macos":
                self.requires("boost_iostreams/1.69.0@bincrafters/stable")

    def build(self):
        self._configured_cmake().build()

    def package(self):
        self._configured_cmake().install()

    def package_info(self):
        self.cpp_info.libs = ["file_monitor"]
        if self.options.filesystem == "boost":
            self.cpp_info.defines = ["file_monitor_USE_BOOST"]
        # Need to link to stdc++fs for g++8, or using <filesystem> will crash
        if self.options.filesystem == "c++17" and self.settings.compiler == "gcc" and\
                tools.Version(self.settings.compiler.version) < "9":
            self.cpp_info.libs.append("stdc++fs")
        if self.settings.os == "Macos":
            self.cpp_info.frameworks = ["CoreFoundation", "CoreServices"]



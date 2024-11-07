from conan import ConanFile, tools
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout


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
    exports_sources = "source/*", "tests/*", "CMakeLists.txt"
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.variables['file_monitor_USE_CONAN'] = True
        toolchain.variables['file_monitor_BUILD_TESTS'] = False
        toolchain.variables['file_monitor_USE_BOOST'] = self.options.filesystem == "boost"
        toolchain.variables['file_monitor_USE_CXX17'] = self.options.filesystem == "c++17"
        toolchain.generate()
    
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
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

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



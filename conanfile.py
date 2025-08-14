from conan import ConanFile, tools
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.scm import Version

class FilemonitorConan(ConanFile):
    name = "file_monitor"
    version = "1.0"
    license = "MIT"
    author = "Marius Elvert marius.elvert@googlemail.com"
    url = "https://github.com/ltjax/file_monitor"
    description = "Lean library to observe file changes in a specific directory path."
    topics = ("hot-loading", "file-monitoring")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = "source/*", "tests/*", "CMakeLists.txt"
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.variables['file_monitor_USE_CONAN'] = True
        toolchain.variables['file_monitor_BUILD_TESTS'] = False
        toolchain.variables['file_monitor_USE_BOOST'] = False
        toolchain.variables['file_monitor_USE_CXX17'] = True
        toolchain.generate()
    
    def configure(self):
        if self.settings.compiler == "gcc" and Version(self.settings.compiler.version) < "8":
            raise ConanInvalidConfiguration("Using <filesystem> requires at least g++ 8")
        if self.settings.compiler == "apple-clang" and Version(self.settings.compiler.version) < "11.0":
            raise ConanInvalidConfiguration("Using <filesystem> requires at least apple-clang 11.0")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["file_monitor"]
        # Need to link to stdc++fs for g++8, or using <filesystem> will crash
        if self.settings.compiler == "gcc" and Version(self.settings.compiler.version) < "9":
            self.cpp_info.libs.append("stdc++fs")
        if self.settings.os == "Macos":
            self.cpp_info.frameworks = ["CoreFoundation", "CoreServices"]



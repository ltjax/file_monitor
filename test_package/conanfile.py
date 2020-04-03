import os

from conans import ConanFile, CMake, tools


class FilemonitorTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        # This is a bit of a hack to set the language standard, but I don't see a better option right now
        using_boost = "file_monitor_USE_BOOST" in self.deps_cpp_info["file_monitor"].defines
        cmake.configure(defs={"CMAKE_CXX_STANDARD": 14 if using_boost else 17})
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def test(self):
        # Does not run standalone on macOS
        if not tools.cross_building(self.settings) and self.settings.os != "Macos":
            os.chdir("bin")
            self.run(".%sexample" % os.sep)

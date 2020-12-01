from conans import ConanFile, CMake, tools


class HttpMockServerConan(ConanFile):
    name = "httpmockserver"
    version = "0.1"
    description = "C++ HTTP mock server"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/seznam/httpmockserver"
    license = "MIT"
    topics = ("conan", "mock")
    exports_sources = ["CMakeLists.txt"]
    generators = "cmake", "cmake_find_package"
    settings = "os", "arch", "compiler", "build_type"

    def source(self):
        self.run("git clone " + self.homepage + ".git")
        tools.replace_in_file(
            "httpmockserver/CMakeLists.txt",
            "project(httpmockserver LANGUAGES CXX)",
            '\n'.join([
                'project(httpmockserver LANGUAGES CXX)',
                'include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)',
                'conan_basic_setup()']))
        tools.replace_in_file(
            "httpmockserver/src/CMakeLists.txt",
            "install(TARGETS httpmockserver ARCHIVE DESTINATION lib)",
            '\n'.join([
                "install(TARGETS httpmockserver ARCHIVE DESTINATION lib)",
                "conan_target_link_libraries(httpmockserver)"]))

    def requirements(self):
        self.requires("cpr/1.5.0")
        self.requires("gtest/1.10.0")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["USE_ALL_SYSTEM_LIBS"] = True
        cmake.definitions["USE_SYSTEM_CURL"] = True
        cmake.definitions["BUILD_CPR_TESTS"] = False
        cmake.definitions["GENERATE_COVERAGE"] = False
        cmake.definitions["USE_SYSTEM_GTEST"] = False
        cmake.definitions["CMAKE_USE_OPENSSL"] = False
        cmake.configure(source_folder="httpmockserver")
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include/httpmockserver", src="httpmockserver", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["httpmockserver", "microhttpd"]

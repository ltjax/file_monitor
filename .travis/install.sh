#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Linux' ]]; then
    sudo apt-get install g++-${GCC_VERSION}
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} 60 --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION}
    sudo update-alternatives --config gcc
    python3 --version
    sudo pip3 install cmake==3.13.3
    pip3 install --upgrade pip --user
    pip --version
    pip install conan --upgrade --user
    pip install conan_package_tools --user
    cmake --version
    conan --version
    conan config install conan_config/
    conan profile new default --detect --force
    conan profile update settings.compiler.libcxx=libstdc++11 default
fi

if [[ "$(uname -s)" == 'Darwin' ]]; then  
    python3 --version

    pip3 install conan --upgrade
    pip3 install conan_package_tools
    conan --version
    conan user
    conan config install conan_config/
fi

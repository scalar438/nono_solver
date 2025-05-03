FROM teeks99/clang-ubuntu:21

RUN apt update && apt install curl -y

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.30.3"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

RUN apt install unzip 

COPY ./install_ninja.sh /tmp/

RUN chmod +x /tmp/install_ninja.sh && /tmp/install_ninja.sh

RUN apt update && apt install libboost-dev -y

#COPY ./reinstall-cmake.sh /tmp/

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

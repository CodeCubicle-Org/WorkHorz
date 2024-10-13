#FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04
# Another option would be use the offical image from ubuntu, # Use the official Ubuntu base image
FROM ubuntu:latest

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="ubuntu-24.04"

# Set environment variables (optional but good practice)
ENV DEBIAN_FRONTEND=noninteractive

# Update the Ubuntu package list and install necessary packages
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    build-essential \
    ca-certificates \
    ccache \
    cmake \
    coreutils \
    cppcheck \
    curl \
    debianutils \
    dialog \
    diffutils \
    dirmngr \
    dpkg \
    e2fsprogs \
    gcc-12 \
    g++-12 \
    git \
    git-man \
    gnupg \
    gnupg-l10n \
    gnupg-utils \
    gnupg2 \
    gpg \
    gpg-agent \
    gpg-wks-client \
    gpg-wks-server \
    gpgconf \
    gpgsm \
    gpgv \
    groff-base \
    gzip \
    htop \
    iproute2 \
    jq \
    locales \
    login \
    lsof \
    media-types \
    mount \
    mold \
    nano \
    ninja-build \
    ncdu \
    netbase \
    openssh-client \
    openssl \
    passwd \
    psmisc \
    python3 \
    python3-minimal \
    readline-common \
    rsync \
    sed \
    strace \
    sudo \
    sqlite3 \
    sysvinit-utils \
    tar \
    tzdata \
    unzip \
    util-linux \
    vim-common \
    vim-tiny \
    wget \
    xxd \
    zip \
    zlib1g  \
    && apt-get clean && \
    rm -rf /var/lib/apt/lists/*


# # 1. Install GCC 14.x (build from source as it's not available through apt)
# RUN cd /tmp && \
#     wget https://ftp.gnu.org/gnu/gcc/gcc-14.1.0/gcc-14.1.0.tar.gz && \
#     tar -xzf gcc-14.1.0.tar.gz && cd gcc-14.1.0 && \
#     ./contrib/download_prerequisites && \
#     mkdir build && cd build && \
#     ../configure --enable-languages=c,c++ --disable-multilib && \
#     make -j$(nproc) && \
#     make install

# # 3. Install CMake >= 3.28
# RUN apt-get remove cmake && \
#     cd /tmp && \
#     wget https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0.tar.gz && \
#     tar -xzvf cmake-3.28.0.tar.gz && cd cmake-3.28.0 && \
#     ./bootstrap && \
#     make -j$(nproc) && \
#     make install

# # 4. Install MoLD >= 2.30
# RUN apt-get remove -y mold && \
#     cd /tmp && \
#     wget https://github.com/rui314/mold/releases/download/v2.30/mold-2.30.tar.gz && \
#     tar -xzvf mold-2.30.tar.gz && cd mold-2.30 && \
#     make -j$(nproc) && \
#     make install

# # 5. Install CCache >= 4.9.1
# RUN apt-get remove -y ccache && \
#     cd /tmp && \
#     wget https://github.com/ccache/ccache/releases/download/v4.9.1/ccache-4.9.1.tar.gz && \
#     tar -xzvf ccache-4.9.1.tar.gz && cd ccache-4.9.1 && \
#     ./configure && \
#     make -j$(nproc) && \
#     make install

# # 6. Install Doxygen >= 1.9.8
# RUN apt-get remove -y doxygen && \
#     cd /tmp && \
#     wget https://github.com/doxygen/doxygen/archive/refs/tags/Release_1_9_8.tar.gz && \
#     tar -xzf Release_1_9_8.tar.gz && cd doxygen-Release_1_9_8 && \
#     cmake -Bbuild -H. && \
#     cmake --build build && \
#     cmake --install build

# # 7. Install Vcpkg (newest)
# RUN git clone https://github.com/microsoft/vcpkg.git /usr/local/vcpkg && \
#     /usr/local/vcpkg/bootstrap-vcpkg.sh

# # 8. Install cppCheck >= 2.13
# RUN apt-get remove -y cppcheck && \
#     cd /tmp && \
#     wget https://github.com/danmar/cppcheck/archive/refs/tags/2.13.tar.gz && \
#     tar -xzvf 2.13.tar.gz && cd cppcheck-2.13 && \
#     cmake -Bbuild -H. && \
#     cmake --build build && \
#     cmake --install build

# # 9. Install SQLite >= 3.44
# RUN apt-get remove sqlite3 && \
#     cd /tmp && \
#     wget https://www.sqlite.org/2023/sqlite-autoconf-3440000.tar.gz && \
#     tar -xvzf sqlite-autoconf-3440000.tar.gz && \
#     cd sqlite-autoconf-3440000 && \
#     ./configure && \
#     make -j$(nproc) && \
#     make install

# # 10. Set GCC 14 as the default GCC compiler
# RUN update-alternatives --install /usr/bin/gcc gcc /usr/local/bin/gcc-14.1.0 60 && \
#     update-alternatives --install /usr/bin/g++ g++ /usr/local/bin/g++-14.1.0 60

# # Clean up
# RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Set timezone (optional)
RUN ln -fs /usr/share/zoneinfo/Europe/Zurich /etc/localtime && dpkg-reconfigure -f noninteractive tzdata

# Create a user with sudo privileges (optional)
RUN useradd -ms /bin/bash dockeruser && echo "dockeruser:password" | chpasswd && adduser dockeruser sudo

# Set the default user (optional)
USER dockeruser

# Set the default working directory
WORKDIR /home/dockeruser

# Expose port 22 for SSH (if needed)
EXPOSE 22

# Entry point (optional, depends on use case)
CMD [ "bash" ]

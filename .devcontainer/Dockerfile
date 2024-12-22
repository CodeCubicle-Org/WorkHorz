FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04
# Another option would be use the offical image from ubuntu, # Use the official Ubuntu base image
#FROM ubuntu:latest

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="ubuntu-24.04"

# Set environment variables (optional but good practice)
ENV DEBIAN_FRONTEND=noninteractive
# This env is for it to work on mac
ENV VCPKG_FORCE_SYSTEM_BINARIES=1

# Update the Ubuntu package list and install necessary packages
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    autoconf \
    automake \
    autoconf-archive \
    build-essential \
    ca-certificates \
    coreutils \
    bash \
    cmake \
    ccache \
    cppcheck \
    curl \
    debianutils \
    dialog \
    diffutils \
    dirmngr \
    doxygen \
    dpkg \
    e2fsprogs \
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
    libtool \
    icu-devtools \
    libpng-dev \
    libzip-dev \
    locales \
    login \
    lua5.4\
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
    sysvinit-utils \
    sqlite3 \
    sqlite3-tools \
    tar \
    tzdata \
    unzip \
    util-linux \
    vim-common \
    vim-tiny \
    wget \
    xxd \
    zip \
    && apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# # Clean up
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

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

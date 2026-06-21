
# Optimized Ubuntu 25.10 for C++ development
# Target: ~5.5GB (reduced from 11GB) while keeping all necessary packages
# 
# Alternative: Consider using debian:bookworm-slim (~100MB smaller base)
# but Ubuntu 25.10 provides newer packages and better compatibility
FROM ubuntu:25.10

ENV DEBIAN_FRONTEND=noninteractive

# First, remove unnecessary packages that come with base Ubuntu image
# These are typically not needed for containerized development environments
RUN apt-get update && apt-get remove -y --purge \
        snapd \
        ubuntu-advantage-tools \
        ubuntu-release-upgrader-core \
        update-notifier-common \
        apport \
        apport-symptoms \
        || true

# Remove unnecessary recommended packages and clean up
# This removes packages that were auto-installed as dependencies but aren't needed
RUN apt-get autoremove -y \
        && apt-get purge -y $(dpkg -l | grep '^rc' | awk '{print $2}') 2>/dev/null || true \
        && apt-get clean \
        && rm -rf /var/lib/apt/lists/*

# Single-layer installation of all packages (reduces image size)
# Using --no-install-recommends to avoid unnecessary dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
        # Core system essentials
        ca-certificates \
        # Build toolchain (gcc/g++ included in build-essential, but explicit for clarity)
        build-essential \
        gdb \
        # Build tools
        cmake \
        ninja-build \
        ccache \
        mold \
        # Development utilities
        git \
        curl \
        wget \
        sudo \
        openssh-server \
        # vcpkg dependencies (IMPORTANT!)
        zip \
        unzip \
        tar \
        pkg-config \
        # Documentation & analysis
        doxygen \
        cppcheck \
        # Automation tools
        autoconf \
        automake \
        autoconf-archive \
        libtool \
        # Scripting
        lua5.4 \
        # Development libraries
        libssl-dev \
        libsqlite3-dev \
        libpng-dev \
        libzip-dev \
        liblzma-dev \
        libicu-dev \
        icu-devtools \
        # Clean up in same layer to reduce size
        && apt-get autoremove -y \
        && apt-get clean \
        && rm -rf /var/lib/apt/lists/* \
        && rm -rf /tmp/* \
        && rm -rf /var/tmp/*

# Install vcpkg - optimized installation to minimize size
# Use shallow clone and remove unnecessary parts after bootstrap
RUN git clone --depth 1 https://github.com/microsoft/vcpkg.git /opt/vcpkg \
        && cd /opt/vcpkg \
        && ./bootstrap-vcpkg.sh

# Clean up vcpkg to reduce size (separate step for better error handling)
RUN cd /opt/vcpkg \
        && rm -rf .git \
        && rm -rf downloads \
        && rm -rf buildtrees \
        && rm -rf packages \
        && find . -name "*.md" -type f -delete 2>/dev/null || true \
        && find . -name "*.txt" -type f ! -name "*.cmake" ! -name "vcpkg*" -delete 2>/dev/null || true \
        && find . -name ".git*" -delete 2>/dev/null || true

# Create non-root user (before cleanup to avoid permission issues)
RUN adduser --disabled-password --gecos '' coder \
        && echo "coder ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/coder \
        && chmod 0440 /etc/sudoers.d/coder \
        && chown -R coder:coder /opt/vcpkg

USER coder
WORKDIR /home/coder

# Add vcpkg to PATH
RUN echo 'export VCPKG_ROOT="/opt/vcpkg"' >> /home/coder/.bashrc \
        && echo 'export PATH="$PATH:$VCPKG_ROOT"' >> /home/coder/.bashrc

# Switch back to root for final aggressive cleanup
USER root

# Final cleanup: Remove any remaining unnecessary packages and files
# This step removes packages that might have been installed as dependencies but aren't needed
RUN apt-get update && apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*

# Aggressive cleanup to minimize final image size
# This cleanup removes files that are not needed for runtime but keep all packages
RUN set -e \
        # Remove documentation and man pages
        && rm -rf /usr/share/doc/* \
        && rm -rf /usr/share/man/* \
        && rm -rf /usr/share/info/* \
        && rm -rf /usr/share/lintian/* \
        && rm -rf /usr/share/groff/* \
        && rm -rf /usr/share/gnome/help/* \
        && rm -rf /usr/share/gtk-doc/* \
        # Remove caches
        && rm -rf /var/cache/apt/* \
        && rm -rf /var/lib/apt/lists/* \
        && rm -rf /root/.cache/* \
        && rm -rf /tmp/* \
        && rm -rf /var/tmp/* \
        # Remove logs
        && find /var/log -type f -delete 2>/dev/null || true \
        # Remove static libraries (.a files) - can save 100-200MB
        && find /usr/lib* -name "*.a" -type f -delete 2>/dev/null || true \
        && find /usr/lib* -name "*.la" -type f -delete 2>/dev/null || true \
        # Remove locale data aggressively (keep only C locale)
        && find /usr/share/locale -mindepth 1 -maxdepth 1 -type d ! -name 'C' ! -name 'C.UTF-8' -exec rm -rf {} + 2>/dev/null || true \
        && rm -rf /usr/share/locale/*/LC_MESSAGES 2>/dev/null || true \
        # Remove unnecessary documentation and text files
        && find /usr/share -name "*.md" -type f -delete 2>/dev/null || true \
        && find /usr/share -name "*.txt" -type f ! -path "*/licenses/*" ! -path "*/copyright" ! -name "*.cmake" -delete 2>/dev/null || true \
        && find /usr/share -name "README*" -type f -delete 2>/dev/null || true \
        && find /usr/share -name "CHANGELOG*" -type f -delete 2>/dev/null || true \
        # Remove Python cache if any
        && find /usr -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true \
        && find /usr -name "*.pyc" -type f -delete 2>/dev/null || true \
        && find /usr -name "*.pyo" -type f -delete 2>/dev/null || true \
        # Remove unnecessary /usr/share subdirectories
        && rm -rf /usr/share/backgrounds 2>/dev/null || true \
        && rm -rf /usr/share/themes 2>/dev/null || true \
        && rm -rf /usr/share/icons 2>/dev/null || true \
        && rm -rf /usr/share/pixmaps 2>/dev/null || true \
        && rm -rf /usr/share/applications 2>/dev/null || true \
        && rm -rf /usr/share/mime 2>/dev/null || true \
        # Remove unnecessary binaries and tools we don't need
        && rm -rf /usr/games 2>/dev/null || true \
        && rm -rf /usr/local/man 2>/dev/null || true \
        # Clear ccache
        && ccache --clear 2>/dev/null || true \
        # Remove empty directories
        && find /usr -type d -empty -delete 2>/dev/null || true

USER coder
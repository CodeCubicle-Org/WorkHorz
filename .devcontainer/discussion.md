{
  "name": "WorkHorz Dev Container",
  "image": "mcr.microsoft.com/devcontainers/base:ubuntu",
  "context": ".",
  "dockerFile": "Dockerfile_whz.txt",
  "settings": {
    "C_Cpp.default.compilerPath": "/usr/bin/gcc-14",
    "C_Cpp.intelliSenseEngine": "Default",
    "terminal.integrated.shell.linux": "/bin/bash",
    "terminal.integrated.profiles.windows": {
      "WSL": {
        "path": "C:\\Windows\\System32\\wsl.exe",
        "args": []
      }
    },
    "remote.WSL2.connectionMethod": "wslExeProxy"
  },
  "extensions": [
    "ms-vscode.cpptools",
    "ms-vscode.cmake-tools",
    "ms-vscode.vscode-ssh",
    "twxs.cmake"
  ],
  "forwardPorts": [22],
  "postCreateCommand": "cmake -B build -S . -G Ninja",
  "remoteUser": "dockeruser",
  "features": {
    "ghcr.io/devcontainers/features/ccache:1": {},
    "ghcr.io/devcontainers/features/gcc:14": {},
    "ghcr.io/devcontainers/features/mold:2": {},
    "ghcr.io/devcontainers/features/ninja:1": {},
    "ghcr.io/devcontainers/features/cmake:3.28": {}
  }
}

# TODO:[

  1. Get all packages for Linux directly from APT with specified version.
  2. Find Mac specific settings for VScode in container and add it

]

# Nov. 7th 2024

## TODO
1. A shell script that takes care of:
  - fetch the vcpkag diretory
  - go there and run the boot-strap.sh
  - do a sudo git pull in that directory
Done :)

~2. A github secret to store the dockeruser password. Make sure it doesn't need the password everytime.~
I skipped this because I figured out it's not needed and I didn't encounter permission error anymore.

3. Set GCC as default complier - check the article sent by Patrick
terraform {
  required_providers {
    coder = {
      source  = "coder/coder"
      version = ">= 2.0"
    }
    docker = {
      source  = "kreuzwerker/docker"
      # FIX: Explicitly require a modern Docker provider version (>3.0)
      version = "~> 3.0"
    }
    local = {
      source = "hashicorp/local"
    }
  }
}

locals {
  username = data.coder_workspace_owner.me.name
}

data "coder_provisioner" "me" {}
data "coder_workspace" "me" {}
data "coder_workspace_owner" "me" {}

# Git repository parameter
data "coder_parameter" "git_repo" {
  name         = "git_repo"
  display_name = "Git Repository"
  description  = "The git repository URL to clone (optional)"
  default      = "https://github.com/CodeCubicle-Org/WorkHorz.git"
  mutable      = true
  type         = "string"
}

# The Dockerfile contents are now in a separate file.
resource "local_file" "dockerfile" {
  filename = "${path.module}/Dockerfile"
  content  = file("${path.module}/Dockerfile")
}

# 2. BUILD THE IMAGE
resource "docker_image" "workspace_image" {
  # Renamed for 25.10 base
  name = "coder-ubuntu25-cpp-workhorz-${data.coder_workspace.me.id}"
  build {
    context    = path.module
    dockerfile = "Dockerfile"
    tag        = ["coder-ubuntu25-cpp-workhorz:latest"]
  }
  keep_locally = true
  depends_on = [local_file.dockerfile]
}

resource "coder_agent" "main" {
  arch           = data.coder_provisioner.me.arch
  os             = "linux"
  startup_script = "#!/bin/bash\nset -e\nsudo /usr/sbin/sshd -D > /dev/null 2>&1 &\necho \"--- Tool Versions ---\"\ngcc --version | head -n 1\ngdb --version | head -n 1\nmold -v | head -n 1\ncmake --version | head -n 1\nvcpkg version | head -n 1\necho \"---------------------\"\nif [ -n \"${data.coder_parameter.git_repo.value}\" ]; then\n  echo \"--- Setting up Repository ---\"\n  cd /home/coder\n  if [ -d \".git\" ]; then\n    echo \"Git repository already exists, pulling latest changes...\"\n    git pull || echo \"Pull failed, continuing with existing code\"\n  elif [ \"$(ls -A /home/coder 2>/dev/null)\" ]; then\n    echo \"Directory not empty and not a git repo, cloning into subdirectory...\"\n    REPO_NAME=$(basename ${data.coder_parameter.git_repo.value} .git)\n    if [ ! -d \"$REPO_NAME\" ]; then\n      git clone ${data.coder_parameter.git_repo.value} \"$REPO_NAME\"\n      echo \"Repository cloned into $REPO_NAME/\"\n    else\n      echo \"Repository directory $REPO_NAME/ already exists\"\n    fi\n  else\n    echo \"Empty directory, cloning repository...\"\n    git clone ${data.coder_parameter.git_repo.value} .\n    echo \"Repository cloned successfully\"\n  fi\n  echo \"-------------------------\"\nfi\necho \"--- Disk Usage Report (Root FS) ---\"\ndf -h / | tail -n 1\necho \"--- Top 10 Largest Directories ---\"\ndu -h --max-depth=1 / 2>/dev/null | sort -rh | head -n 10\necho \"--- Largest /usr subdirectories ---\"\ndu -h --max-depth=1 /usr 2>/dev/null | sort -rh | head -n 10\necho \"--- Container Filesystem Analysis ---\"\necho \"Total filesystem size: $(du -sh / 2>/dev/null | cut -f1)\"\necho \"--- Docker Image/Container Info (if available) ---\"\nif command -v docker >/dev/null 2>&1; then\n  echo \"Docker images:\"\n  docker images --format \"table {{.Repository}}\\t{{.Tag}}\\t{{.Size}}\" 2>/dev/null | head -n 5 || echo \"Cannot list images\"\n  echo \"Current container:\"\n  docker ps --format \"table {{.ID}}\\t{{.Image}}\\t{{.Size}}\" 2>/dev/null | head -n 3 || echo \"Cannot list containers\"\nelse\n  echo \"Docker CLI not available in container\"\nfi\necho \"--- Overlay Filesystem Info ---\"\nif [ -d /sys/fs/cgroup ]; then\n  echo \"Container type: $(cat /proc/self/cgroup | head -1 | cut -d: -f3 | cut -d/ -f1 2>/dev/null || echo 'unknown')\"\nfi\necho \"--- Host-level check instructions ---\"\necho \"To check Docker image size on HOST, run:\"\necho \"  docker images | grep coder-ubuntu25-cpp-workhorz\"\necho \"  docker system df\"\necho \"  docker inspect <container-id> | grep -A 10 Mounts\"\necho \"-----------------------------------\""

  env = {
    # Set the default C/C++ standards
    CPPFLAGS = "-std=c++23"
    CFLAGS   = "-std=c23"
    # Git identity
    GIT_AUTHOR_NAME     = coalesce(data.coder_workspace_owner.me.full_name, data.coder_workspace_owner.me.name)
    GIT_AUTHOR_EMAIL    = "${data.coder_workspace_owner.me.email}"
    GIT_COMMITTER_NAME  = coalesce(data.coder_workspace_owner.me.full_name, data.coder_workspace_owner.me.name)
    GIT_COMMITTER_EMAIL = "${data.coder_workspace_owner.me.email}"
  }

  metadata {
    display_name = "CPU Usage"
    key          = "0_cpu_usage"
    script       = "coder stat cpu"
    interval     = 10
    timeout      = 1
  }

  metadata {
    display_name = "Disk Usage"
    key          = "1_disk_usage"
    script       = "coder stat disk --path /home/coder"
    interval     = 60
    timeout      = 1
  }

  metadata {
    display_name = "Memory Usage"
    key          = "2_memory_usage"
    script       = "coder stat mem"
    interval     = 10
    timeout      = 1
  }
}


module "code-server" {
  source   = "registry.coder.com/coder/code-server/coder"
  version  = "1.0.18"
  agent_id = coder_agent.main.id
  folder   = "/home/coder"
  order    = 1
}

module "jetbrains_gateway" {
  source   = "registry.coder.com/modules/jetbrains-gateway/coder"
  version  = "1.0.12"
  
  agent_id   = coder_agent.main.id
  agent_name = "main"
  folder     = "/home/coder"
  
  # Default IDE - CLion for C++ development
  jetbrains_ides = ["CL"]
  default        = "CL"
  
  # Use direct connection instead of subdomain (if subdomain fails)
  # This may require Coder to be configured with wildcard access URL
  # Or use SSH connection method
  order = 2
}

resource "docker_volume" "home_volume" {
  name = "coder-${data.coder_workspace.me.id}-home"
  lifecycle {
    ignore_changes = all
  }
  # FINAL LABEL FIX: Using the labels { key = value } block structure (with underscores)
  labels {
    label = "coder.owner"
    value = data.coder_workspace_owner.me.name
  }
  
  labels {
    label = "coder.workspace_id"
    value = data.coder_workspace.me.id
  }
}

resource "docker_container" "workspace" {
  count    = data.coder_workspace.me.start_count
  image    = docker_image.workspace_image.name
  name     = "coder-${data.coder_workspace_owner.me.name}-${lower(data.coder_workspace.me.name)}"
  hostname = data.coder_workspace.me.name

  entrypoint = ["sh", "-c", replace(coder_agent.main.init_script, "/localhost|127\\.0\\.0\\.1/", "host.docker.internal")]
  env        = ["CODER_AGENT_TOKEN=${coder_agent.main.token}"]

  host {
    host = "host.docker.internal"
    ip   = "host-gateway"
  }

  volumes {
    container_path = "/home/coder"
    volume_name    = docker_volume.home_volume.name
    read_only      = false
  }

  labels {
    label = "coder.owner"
    value = data.coder_workspace_owner.me.name
  }
  
  labels {
    label = "coder.workspace_id"
    value = data.coder_workspace.me.id
  }
}
terraform {
  required_providers {
    coder = {
      source  = "coder/coder"
      version = "~> 2.0"
    }
    docker = {
      source  = "kreuzwerker/docker"
      version = "~> 3.0"
    }
    envbuilder = {
      source  = "coder/envbuilder"
      version = "~> 1.0"
    }
  }
}

# Docker socket configuration
variable "docker_socket" {
  default     = ""
  description = "(Optional) Docker socket URI"
  type        = string
}

# Cache repository for faster builds
variable "cache_repo" {
  default     = ""
  description = "(Optional) Container registry for caching builds (e.g., docker.io/username/workhorz-cache)"
  type        = string
}

variable "insecure_cache_repo" {
  default     = false
  description = "Enable if cache registry doesn't serve HTTPS"
  type        = bool
}

variable "cache_repo_docker_config_path" {
  default     = ""
  description = "(Optional) Path to docker config.json with cache repo credentials"
  sensitive   = true
  type        = string
}

provider "coder" {}
provider "docker" {
  host = var.docker_socket != "" ? var.docker_socket : null
}
provider "envbuilder" {}

data "coder_provisioner" "me" {}
data "coder_workspace" "me" {}
data "coder_workspace_owner" "me" {}

# Repository selection
data "coder_parameter" "git_repo" {
  name         = "git_repo"
  display_name = "Git Repository"
  description  = "The git repository URL for WorkHorz"
  default      = "https://github.com/patlecat/WorkHorz.git"
  mutable      = true
  order        = 1
}

# Branch selection
data "coder_parameter" "git_branch" {
  name         = "git_branch"
  display_name = "Git Branch"
  description  = "The git branch to checkout"
  default      = "main"
  mutable      = true
  order        = 2
}

# CPU allocation
data "coder_parameter" "cpu" {
  name         = "cpu"
  display_name = "CPU Cores"
  description  = "Number of CPU cores"
  default      = "4"
  type         = "number"
  mutable      = true
  order        = 3
  validation {
    min = 2
    max = 16
  }
}

# Memory allocation
data "coder_parameter" "memory" {
  name         = "memory"
  display_name = "Memory (GB)"
  description  = "Amount of memory in GB"
  default      = "8"
  type         = "number"
  mutable      = true
  order        = 4
}

# Devcontainer builder image
data "coder_parameter" "devcontainer_builder" {
  name         = "devcontainer_builder"
  display_name = "Devcontainer Builder"
  description  = "Envbuilder image version. See: https://github.com/coder/envbuilder/pkgs/container/envbuilder"
  default      = "ghcr.io/coder/envbuilder:latest"
  mutable      = true
  order        = 5
}

# Fallback image if devcontainer build fails
data "coder_parameter" "fallback_image" {
  name         = "fallback_image"
  display_name = "Fallback Image"
  description  = "Image to use if devcontainer build fails"
  default      = "mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04"
  mutable      = true
  order        = 6
}

locals {
  container_name             = "coder-${data.coder_workspace_owner.me.name}-${lower(data.coder_workspace.me.name)}"
  devcontainer_builder_image = data.coder_parameter.devcontainer_builder.value
  git_author_name            = coalesce(data.coder_workspace_owner.me.full_name, data.coder_workspace_owner.me.name)
  git_author_email           = data.coder_workspace_owner.me.email
  repo_url                   = data.coder_parameter.git_repo.value

  # Envbuilder environment variables
  envbuilder_env = {
    "ENVBUILDER_GIT_URL" : local.repo_url,
    "ENVBUILDER_GIT_BRANCH" : data.coder_parameter.git_branch.value,
    "ENVBUILDER_CACHE_REPO" : var.cache_repo,
    "CODER_AGENT_TOKEN" : coder_agent.main.token,
    "CODER_AGENT_URL" : replace(data.coder_workspace.me.access_url, "/localhost|127\\.0\\.0\\.1/", "host.docker.internal"),
    "ENVBUILDER_INIT_SCRIPT" : replace(coder_agent.main.init_script, "/localhost|127\\.0\\.0\\.1/", "host.docker.internal"),
    "ENVBUILDER_FALLBACK_IMAGE" : data.coder_parameter.fallback_image.value,
    "ENVBUILDER_DOCKER_CONFIG_BASE64" : try(data.local_sensitive_file.cache_repo_dockerconfigjson[0].content_base64, ""),
    "ENVBUILDER_PUSH_IMAGE" : var.cache_repo == "" ? "" : "true",
    "ENVBUILDER_INSECURE" : "${var.insecure_cache_repo}",
    "ENVBUILDER_DEVCONTAINER_DIR" : ".devcontainer",
    "VCPKG_FORCE_SYSTEM_BINARIES" : "1",
  }

  docker_env = [for k, v in local.envbuilder_env : "${k}=${v}"]
}

# Cache repo credentials (if needed)
data "local_sensitive_file" "cache_repo_dockerconfigjson" {
  count    = var.cache_repo_docker_config_path == "" ? 0 : 1
  filename = var.cache_repo_docker_config_path
}

# Pull devcontainer builder image
resource "docker_image" "devcontainer_builder_image" {
  name         = local.devcontainer_builder_image
  keep_locally = true
}

# Persistent workspace volume
resource "docker_volume" "workspaces" {
  name = "coder-${data.coder_workspace.me.id}"
  lifecycle {
    ignore_changes = all
  }
  labels {
    label = "coder.owner"
    value = data.coder_workspace_owner.me.name
  }
  labels {
    label = "coder.owner_id"
    value = data.coder_workspace_owner.me.id
  }
  labels {
    label = "coder.workspace_id"
    value = data.coder_workspace.me.id
  }
  labels {
    label = "coder.workspace_name_at_creation"
    value = data.coder_workspace.me.name
  }
}

# vcpkg cache volume
resource "docker_volume" "vcpkg_cache" {
  name = "coder-${data.coder_workspace.me.id}-vcpkg"
  lifecycle {
    ignore_changes = all
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

# Build cache volume
resource "docker_volume" "build_cache" {
  name = "coder-${data.coder_workspace.me.id}-build"
  lifecycle {
    ignore_changes = all
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

# Check for cached image
resource "envbuilder_cached_image" "cached" {
  count         = var.cache_repo == "" ? 0 : data.coder_workspace.me.start_count
  builder_image = local.devcontainer_builder_image
  git_url       = local.repo_url
  cache_repo    = var.cache_repo
  extra_env     = local.envbuilder_env
  insecure      = var.insecure_cache_repo
}

# Main development container
resource "docker_container" "workspace" {
  count    = data.coder_workspace.me.start_count
  image    = var.cache_repo == "" ? local.devcontainer_builder_image : envbuilder_cached_image.cached[0].image
  name     = local.container_name
  hostname = data.coder_workspace.me.name
  env      = var.cache_repo == "" ? local.docker_env : envbuilder_cached_image.cached[0].env

  # Resource limits
  cpu_shares = data.coder_parameter.cpu.value * 1024
  memory     = data.coder_parameter.memory.value * 1024

  # Network configuration
  host {
    host = "host.docker.internal"
    ip   = "host-gateway"
  }

  # Workspace volume
  volumes {
    container_path = "/workspaces"
    volume_name    = docker_volume.workspaces.name
    read_only      = false
  }

  # vcpkg cache
  volumes {
    container_path = "/home/dockeruser/.vcpkg"
    volume_name    = docker_volume.vcpkg_cache.name
    read_only      = false
  }

  # Build cache
  volumes {
    container_path = "/home/dockeruser/build"
    volume_name    = docker_volume.build_cache.name
    read_only      = false
  }

  # Labels for resource tracking
  labels {
    label = "coder.owner"
    value = data.coder_workspace_owner.me.name
  }
  labels {
    label = "coder.owner_id"
    value = data.coder_workspace_owner.me.id
  }
  labels {
    label = "coder.workspace_id"
    value = data.coder_workspace.me.id
  }
  labels {
    label = "coder.workspace_name"
    value = data.coder_workspace.me.name
  }
}

# Coder agent
resource "coder_agent" "main" {
  arch = data.coder_provisioner.me.arch
  os   = "linux"
  dir  = "/workspaces/${basename(local.repo_url)}"

  startup_script = <<-EOT
    set -e

    # Install vcpkg dependencies if vcpkg.json exists
    if [ -f "vcpkg.json" ]; then
      echo "Installing vcpkg dependencies..."
      if [ ! -d "vcpkg" ]; then
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        ./bootstrap-vcpkg.sh
        cd ..
      fi
      ./vcpkg/vcpkg install
    fi

    # Create build directory
    mkdir -p build

    echo "WorkHorz workspace ready!"
  EOT

  env = {
    GIT_AUTHOR_NAME     = local.git_author_name
    GIT_AUTHOR_EMAIL    = local.git_author_email
    GIT_COMMITTER_NAME  = local.git_author_name
    GIT_COMMITTER_EMAIL = local.git_author_email
  }

  # Metadata
  metadata {
    display_name = "CPU Usage"
    key          = "0_cpu_usage"
    script       = "coder stat cpu"
    interval     = 10
    timeout      = 1
  }

  metadata {
    display_name = "RAM Usage"
    key          = "1_ram_usage"
    script       = "coder stat mem"
    interval     = 10
    timeout      = 1
  }

  metadata {
    display_name = "Workspace Disk"
    key          = "3_workspace_disk"
    script       = "coder stat disk --path /workspaces"
    interval     = 60
    timeout      = 1
  }

  metadata {
    display_name = "CPU Usage (Host)"
    key          = "4_cpu_usage_host"
    script       = "coder stat cpu --host"
    interval     = 10
    timeout      = 1
  }

  metadata {
    display_name = "Memory Usage (Host)"
    key          = "5_mem_usage_host"
    script       = "coder stat mem --host"
    interval     = 10
    timeout      = 1
  }

  metadata {
    display_name = "Load Average (Host)"
    key          = "6_load_host"
    script       = <<EOT
      echo "`cat /proc/loadavg | awk '{ print $1 }'` `nproc`" | awk '{ printf "%0.2f", $1/$2 }'
    EOT
    interval     = 60
    timeout      = 1
  }
}

# VS Code Web IDE module
module "code-server" {
  count   = data.coder_workspace.me.start_count
  source  = "registry.coder.com/modules/code-server/coder"
  version = "~> 1.0"

  agent_id = coder_agent.main.id
  order    = 1
  folder   = "/workspaces/${basename(local.repo_url)}"
}

# JetBrains IDE support (CLion)
module "jetbrains" {
  count      = data.coder_workspace.me.start_count
  source     = "registry.coder.com/modules/jetbrains/coder"
  version    = "~> 1.0"
  agent_id   = coder_agent.main.id
  agent_name = "main"
  folder     = "/workspaces/${basename(local.repo_url)}"

  # Suggest CLion for C++ development
  default = ["CL"]
}

# WorkHorz web application
resource "coder_app" "workhorz_web" {
  agent_id     = coder_agent.main.id
  slug         = "workhorz-web"
  display_name = "WorkHorz Web"
  url          = "http://localhost:8080"
  icon         = "/icon/code.svg"
  subdomain    = true
  share        = "owner"

  healthcheck {
    url       = "http://localhost:8080/health"
    interval  = 5
    threshold = 6
  }
}

# Workspace metadata
resource "coder_metadata" "container_info" {
  count       = data.coder_workspace.me.start_count
  resource_id = coder_agent.main.id

  item {
    key   = "builder image"
    value = var.cache_repo == "" ? local.devcontainer_builder_image : envbuilder_cached_image.cached[0].image
  }

  item {
    key   = "git url"
    value = local.repo_url
  }

  item {
    key   = "git branch"
    value = data.coder_parameter.git_branch.value
  }

  item {
    key   = "cache repo"
    value = var.cache_repo == "" ? "not enabled" : var.cache_repo
  }

  item {
    key   = "cpu cores"
    value = "${data.coder_parameter.cpu.value}"
  }

  item {
    key   = "memory (GB)"
    value = "${data.coder_parameter.memory.value}"
  }
}

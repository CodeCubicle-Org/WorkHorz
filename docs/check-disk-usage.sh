#!/bin/bash
# Script to check where disk space is being used in Coder workspace
# Run this on the HOST system (where Coder server is running)

echo "=== Docker Disk Usage Analysis ==="
echo ""
echo "1. Docker Images (this is likely where the 13GB is):"
docker images | grep -E "coder-ubuntu25-cpp-workhorz|REPOSITORY"
echo ""
echo "2. Docker System Disk Usage:"
docker system df
echo ""
echo "3. Individual Image Layers (detailed):"
IMAGE_NAME=$(docker images --format "{{.Repository}}:{{.Tag}}" | grep coder-ubuntu25-cpp-workhorz | head -1)
if [ -n "$IMAGE_NAME" ]; then
    echo "Inspecting image: $IMAGE_NAME"
    docker history "$IMAGE_NAME" --human --format "table {{.CreatedBy}}\t{{.Size}}"
else
    echo "Image not found. List all images:"
    docker images
fi
echo ""
echo "4. Container Disk Usage:"
docker ps --format "table {{.ID}}\t{{.Image}}\t{{.Size}}\t{{.Names}}"
echo ""
echo "5. Docker Volume Usage:"
docker volume ls | grep coder
echo ""
echo "6. Volume Details (size and usage):"
for vol in $(docker volume ls -q | grep coder); do
    echo "Volume: $vol"
    docker volume inspect "$vol" --format "  Size: {{.Mountpoint}}" 2>/dev/null || echo "  (cannot inspect)"
    # Try to get actual size
    MOUNTPOINT=$(docker volume inspect "$vol" --format "{{.Mountpoint}}" 2>/dev/null)
    if [ -n "$MOUNTPOINT" ] && [ -d "$MOUNTPOINT" ]; then
        SIZE=$(du -sh "$MOUNTPOINT" 2>/dev/null | cut -f1)
        echo "  Disk usage: $SIZE"
    fi
done
echo ""
echo "7. All Containers (including stopped):"
docker ps -a --format "table {{.ID}}\t{{.Image}}\t{{.Status}}\t{{.Names}}"
echo ""
echo "8. Volumes attached to containers:"
docker ps -a --format "{{.Names}}" | while read name; do
    if [ -n "$name" ]; then
        echo "Container: $name"
        docker inspect "$name" --format "  Volumes: {{range .Mounts}}{{.Name}} {{end}}" 2>/dev/null || echo "  (cannot inspect)"
    fi
done
echo ""
echo ""
echo "=== ANALYSIS ==="
echo "Your Docker images are ~1.1-1.7GB each (EXCELLENT!)"
echo "The 13GB you're seeing includes:"
echo "  - Old Docker images: ~4.3GB (reclaimable)"
echo "  - Build cache: ~2.8GB (reclaimable)"
echo "  - Volumes: ~1.7GB (727MB reclaimable)"
echo "  - Current workspace: ~1.2GB (actual filesystem)"
echo ""
echo "=== CLEANUP COMMANDS (run as root) ==="
echo ""
echo "1. Remove old/unused Docker images:"
echo "   docker image prune -a"
echo ""
echo "2. Remove build cache:"
echo "   docker builder prune -a"
echo ""
echo "3. Remove unused volumes:"
echo "   docker volume prune"
echo "   (Note: Only removes volumes not attached to ANY container, even stopped ones)"
echo ""
echo "4. Manually remove old workspace volumes (if workspaces are deleted in Coder):"
echo "   docker volume rm <volume-name>"
echo "   Example: docker volume rm coder-aa5ced6d-630a-40a4-9541-1ac2b4bf2e9f-home"
echo ""
echo "5. Remove stopped containers first, then prune volumes:"
echo "   docker container prune"
echo "   docker volume prune"
echo ""
echo "6. Remove everything unused (careful!):"
echo "   docker system prune -a --volumes"
echo ""
echo "=== To reduce Docker image size further: ==="
echo "- Use 'docker build --squash' to squash layers"
echo "- Use multi-stage builds"
echo "- Consider using a smaller base image (debian:bookworm-slim)"
echo ""
echo "=== To check workspace filesystem (inside container): ==="
echo "Run: du -sh /"
echo "This should show ~1.2GB (the actual filesystem usage)"


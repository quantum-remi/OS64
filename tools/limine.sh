#!/usr/bin/env bash

set -eo pipefail

LIMINE_VERSION="v9.x-binary"
LIMINE_DIR="${PWD}/tools/limine"
LIMINE_REPO="https://github.com/limine-bootloader/limine.git"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
    exit 1
}

info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

check_deps() {
    local deps=("git" "make" "nasm" "clang")
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" >/dev/null 2>&1; then
            error "Missing dependency: $dep"
        fi
    done
}

clone_limine() {
    if [[ ! -d "$LIMINE_DIR" ]]; then
        info "Cloning Limine binary branch..."
        git clone --depth 1 --branch "$LIMINE_VERSION" "$LIMINE_REPO" "$LIMINE_DIR"
    else
        info "Limine repository already exists"
        warning "To force fresh clone: rm -rf $LIMINE_DIR"
    fi
}

setup_symlinks() {
    local target_dir="${PWD}/tools/bin"
    
    mkdir -p "$target_dir"
    ln -sfv "$LIMINE_DIR/limine" "$target_dir/"
}

main() {
    check_deps
    clone_limine
    setup_symlinks 
    make -C "$LIMINE_DIR" CC=clang
    cp -v "$LIMINE_DIR"/limine.h src/include/
    echo
    info "Limine v9.x-binary setup complete!"
    echo -e "Add to PATH:\n  ${YELLOW}export PATH=\"${PWD}/tools/bin:\$PATH\"${NC}"
}

main "$@"


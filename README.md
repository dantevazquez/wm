# monowm

A lightweight, minimal monocle window manager for X11 written in C. It stretches windows to fullscreen (optionally leaving space for a status bar like `lemonbar`) and manages window focus sequentially.

---

## Installation

### Method 1: Nix Flake (Recommended)

If you are using Nix, you can run or install `monowm` instantly without manual compilation.

#### Try it without installing
To run `monowm` directly:
```bash
nix run github:dantevazquez/monowm --extra-experimental-features "nix-command flakes"
```

#### Install into your Nix profile
```bash
nix profile install github:dantevazquez/monowm --extra-experimental-features "nix-command flakes"
```

#### Use in NixOS configuration
Add the input to your `flake.nix`:
```nix
inputs = {
  monowm.url = "github:dantevazquez/monowm";
};
```
Then add it to your system packages:
```nix
environment.systemPackages = [
  inputs.monowm.packages.${pkgs.system}.default
];
```

#### Dev Shell
To start a development shell with all dependencies needed to compile `monowm`:
```bash
nix develop --extra-experimental-features "nix-command flakes"
```

---

### Method 2: Manual Installation (Makefile)

#### Dependencies
Ensure you have the following packages installed:
- `libX11` development headers
- `pkg-config`
- `gcc`
- `make`

#### Building and Installing
1. Compile the binaries:
   ```bash
   make
   ```
2. Install the binaries and startup scripts:
   ```bash
   sudo make install PREFIX=/usr/local
   ```

---

## Configuration

Customizing keybindings and window options can be done by editing [config.h](file:///home/dante/monowm/config.h) before compiling.

- **Status Bar (`lemonbar`)**: Enabled/disabled in `config.h`.
- **Keyboard Shortcuts**: Configuration is read from `sxhkdrc` (installed to `~/.config/sxhkd/sxhkdrc`).

## License

This project is licensed under the MIT License.

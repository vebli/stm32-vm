{
  description = "stm32-vm";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = {
    self,
    nixpkgs,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {
      inherit system;
      config = {
        allowUnfree = true;
      };
    };
  in {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        gcc-arm-embedded
        newlib
        cmake
        gnumake
        stlink
        openocd
        usbutils
        minicom
        stm32cubemx
      ];
    };
  };
}

{
  outputs = { self, nixpkgs }: {
    devShells.x86_64-linux.default = let
      pkgs = import nixpkgs { system = "x86_64-linux"; };
    in pkgs.mkShell {
      buildInputs = with pkgs; [
        platformio
        (python3.withPackages (ps: with ps; [
          pyserial
          matplotlib3
        ]))
      ];
    };
  };
}

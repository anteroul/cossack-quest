{
  description = "Cossack Quest - a raylib game in C++";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        cossack-quest = pkgs.stdenv.mkDerivation {
          pname = "cossack-quest";
          version = "0.1.0";
          src = ./.;

          cmakeDir = "../Cossack-Quest/cmake";

          nativeBuildInputs = [ pkgs.cmake pkgs.pkg-config pkgs.makeWrapper ];
          buildInputs = [ pkgs.raylib ];

          # The game loads assets/ and shaders/ relative to the working directory,
          # so install them next to each other and cd there before launching.
          installPhase = ''
            runHook preInstall
            mkdir -p $out/bin $out/share/cossack-quest
            install -Dm755 CossackQuest $out/share/cossack-quest/CossackQuest
            cp -r $src/Cossack-Quest/assets $src/Cossack-Quest/shaders $out/share/cossack-quest/
            makeWrapper $out/share/cossack-quest/CossackQuest $out/bin/cossack-quest \
              --chdir $out/share/cossack-quest
            runHook postInstall
          '';

          meta.mainProgram = "cossack-quest";
        };
      in
      {
        packages.default = cossack-quest;

        apps.default = flake-utils.lib.mkApp { drv = cossack-quest; };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ cossack-quest ];
          packages = with pkgs; [
            clang-tools # clangd + clang-format for Helix
            gdb
          ];

          shellHook = ''
            echo "Cossack Quest dev shell"
            echo "  configure: cmake -S Cossack-Quest/cmake -B build -DCMAKE_BUILD_TYPE=Debug"
            echo "  build:     cmake --build build"
            echo "  run:       (cd Cossack-Quest && ../build/CossackQuest)"
          '';
        };
      });
}

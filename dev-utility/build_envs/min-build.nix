with import <nixpkgs> {};

stdenv.mkDerivation rec {
  name = "kttd-buid-env";
  env = buildEnv { name = name; paths = buildInputs; };

  buildInputs = [
    gcc
    cmake
    elfutils
  ];

  hardeningDisable = [ "pic" ];
}


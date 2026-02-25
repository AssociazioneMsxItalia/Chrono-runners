set -eux

python3 afb_pack.py -f ../assets/chronorunner.fxlist -o ../content/ayfx/fx_chronorunner.afb
../../../tools/MSXtk/bin/MSXbin ../content/ayfx/fx_chronorunner.afb
rm ../content/ayfx/fx_chronorunner.afb

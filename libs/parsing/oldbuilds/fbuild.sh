WDIR=$(pwd)
lin="$WDIR/build $WDIR/dist/assemblelib-linux/libassemblelib.a"
linDebug="$WDIR/dbuild $WDIR/dist/assemblelib-linux/libdassemblelib.a"

eval $lin
eval $linDebug
$win = "$pwd\build.bat $pwd\dist\assemblelib-win\libassemblelib.a"
$winDebug = "$pwd\dbuild.bat $pwd\dist\assemblelib-win\libdassemblelib.a"

Remove-Item "$pwd/dist/assemblelib-win/*"
Remove-Item "$pwd/dist/assemblelib-linux/*"

iex $win
iex $winDebug

wsl -- "./fbuild.sh"
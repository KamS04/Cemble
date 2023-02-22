$libsFolder = "$PSScriptRoot\libs"
$comm = "gcc $args commandline.c oarrlist.c korolib.c setup.c assembler.c writer.c ``
    reader.c sort.c map.c encoder.c encode.c ``
    wtiming.c wfilereader.c formats.c instruction.c ``
    -L$libsFolder -lwdparselib -lwdassemblelib -ggdb -DDEBUG"

Invoke-Expression $comm
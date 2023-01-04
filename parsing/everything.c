#include "parselib.h"
#include<stdbool.h>
#include "syntax.h"
#include "data_p.h"
#include "constant_p.h"
#include "structure_p.h"
#include "instruction_p.h"
#include "comment.h"
#include "common.h"

parser* _aPArr[7];
parser* _cP;
parser* _assemblyParser;
bool _aPinited = false;

void _init_assembly_stuff() {
    int i = 0;
    _aPArr[i++] = create_data_parser();
    _aPArr[i++] = create_constant_parser();
    _aPArr[i++] = create_structure_parser();
    _aPArr[i++] = create_argument_structure_parser();
    _aPArr[i++] = instructionParser();
    _aPArr[i++] = create_comment_parser();
    _aPArr[i++] = label;
    _cP = choice(_aPArr, 7);
    _assemblyParser = manyAS1(_cP, SYNTAX_TYPE);
    _aPinited = true;
}

parser* single_assembly_parser() {
    if (!_aPinited) {
        _init_assembly_stuff();
    }
    return _cP;
}

parser* create_assembly_parser() {
    if (!_aPinited) {
        _init_assembly_stuff();
    }

    return _assemblyParser;
}
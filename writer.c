#include<stdio.h>
#include "parselib.h"
#include "assembler.h"
#include "encoder.h"
#include "sort.h"
#include<string.h>

parser* _eL[2];
parser* _encodedLines;
bool _in_enclin = false;

parser* _uL[5];
parser* _unencodedLines;
bool _in_unenclin = false;

char _hold_stuff[500];
char hexC[] = "0123456789abcdef";

int find_n_before(char* ascM, int st) {
    for (int i = st; i >= 0; i--) {
        if (ascM[i] == '\n') {
            return i+1;
        }
    }
    return 0;
}

int find_n_after(char* ascM, int st) {
    int i = st+1;
    while (true) {
        char c = ascM[i];
        if (c == '\n') {
            return i;
        } else if (c == '\0') {
            return i;
        }
        i++;
    }
    return -1;
}

// Assumes error
void pretty_print_asm_error(char* asmC, state* pA) {
    int lastN = find_n_before(asmC, pA->index);
    int nextN = find_n_after(asmC, pA->index);
    int eLen = nextN - lastN;
    
    printf("%.*s\n", eLen, asmC + lastN);
    int lBefore = pA->index - lastN - 1;
    for (int i = 0; i < lBefore; i++) {
        putchar(' ');
    }
    putchar('^');


    if (pA->error != NULL) {
        printf(" %s\n", pA->error);
    }
}

parser* encodedLines() {
    if (!_in_enclin) {
        _eL[0] = instructionParser();
        _eL[1] = create_data_parser();
        _encodedLines = choice(_eL, 2);
        _in_enclin = true;
    }
    return _encodedLines;
}

parser* unEncodedLines() {
    if (!_in_unenclin) {
        _uL[0] =  label;
        _uL[1] = create_constant_parser();
        _uL[2] = create_structure_parser();
        _uL[3] = create_argument_structure_parser();
        _uL[4] = create_comment_parser();
        _unencodedLines = choice(_uL, 5);
        _in_unenclin = true;
    }
    return _unencodedLines;
}

long long next_encoded_lines(int start, char* asmC) {
    state* i_state; state* n_state;
    i_state = default_state();
    i_state->index = start;

    parser* p = encodedLines();
    parser* pi = unEncodedLines();
    
    while (1) {
        // try encoded
        n_state = evaluate(p, asmC, i_state);
        // printf("encoded starting at %d:\n%s\n", i_state->index, state_to_string(n_state));

        if (n_state->is_error) {
            // try unEncoded
            deallocate_state(n_state);
            n_state = evaluate(pi, asmC, i_state);
            // printf("unencoded starting at %d:\n%s\n", i_state->index, state_to_string(n_state));
            i_state->index = n_state->index;
            deallocate_state(n_state);
            // getchar();
        } else {
            long long ret = ((long long)i_state->index) << 32 | n_state->index;
            deallocate_state(n_state);
            free(i_state);
            // printf("%lld %d %d\n", ret, i_state->index, n_state->index);
            return ret;
        }
    }

    return -1;
}

void remove_new_lines(char* txt) {
    int i = 0;
    while (1) {
        if (txt[i] == '\0') {
            return;
        }
        if (txt[i] == '\n') {
            txt[i] = ' ';
        }
        i++;
    }
    return;
}

void pretty_print_machine_code(uint8_t* mCode, uint8_t cOffset, int mLen, char* asmC, AssemblyContext* asmCon, int decAddrs) {
    int insWritten = 5;
    bool fst = true;
    int st = 0; int le = 0; int en = 0;
    int cAddr = cOffset;

    if (decAddrs) {
        printf("%05d: ", cAddr);
    } else {
        putchar( hexC[cAddr >> 12] );
        putchar( hexC[(cAddr >> 8) & 0x000f] );
        putchar( hexC[(cAddr >> 4) & 0x000f] );
        putchar( hexC[cAddr & 0x000f] );
        putchar(':');
        putchar(' ');
    }

    for (int i = 0; i < mLen+1; i++) {
        int idxNL = bin_search_prim(asmCon->readOutput->nextLineIndexes, asmCon->readOutput->num_next_lines, i);
        if (i != 0 && idxNL != -1) {
            if (insWritten < 0) {
                insWritten = 0;
            }

            // padding + |
            printf("%-*s| ", insWritten * 3, "");
            long long out = next_encoded_lines(st, asmC);
            st = (int) (out >> 32);
            en = (int) out;
            le = en - st;
            memcpy(_hold_stuff, asmC + st, le);
            _hold_stuff[le] = '\0';
            remove_new_lines(_hold_stuff);
            printf("%s\n", _hold_stuff);
            st = en;

            if (i == mLen) {
                break;
            }

            cAddr = i + cOffset;
            if (decAddrs) {
                printf("%05d: ", cAddr);
            } else {
                putchar( hexC[cAddr >> 12] );
                putchar( hexC[(cAddr >> 8) & 0x000f] );
                putchar( hexC[(cAddr >> 4) & 0x000f] );
                putchar( hexC[cAddr & 0x000f] );
                putchar(':');
                putchar(' ');
            }
            
            insWritten = 5;
        }
        
        uint8_t byte = mCode[i];
        putchar( hexC[byte >> 4] );
        putchar( hexC[byte & 0x0f] );
        putchar(' ');
        insWritten--;
    }
}

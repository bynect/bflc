#include "brainfuck.h"

error_t
scan_brainfuck(const char *src, size_t len, ir_t *ir)
{
    error_t err;
    error_init(&err, NULL, NULL);

    uint32_t line = 1;
    uint32_t column = 1;
    size_t instr = 0;

    for (size_t i = 0; i < len; ++i)
    {
        ++column;
        switch (src[i])
        {
            case '>':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_PTRINC;
                ir->instrs[instr].arg = 1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case '<':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_PTRDEC;
                ir->instrs[instr].arg = 1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case '+':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_CELINC;
                ir->instrs[instr].arg = 1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case '-':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_CELDEC;
                ir->instrs[instr].arg = 1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case '.':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_OUTPUT;
                ir->instrs[instr].arg = -1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case ',':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_INPUT;
                ir->instrs[instr].arg = -1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case '[':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_JMPBEG;
                ir->instrs[instr].arg = -1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case ']':
                ir_ensure(ir, instr);
                ir->instrs[instr].instr = INSTR_JMPEND;
                ir->instrs[instr].arg = -1;
                ir->instrs[instr].line = line;
                ir->instrs[instr].column = column;
                ir->instrs[instr].offset = i;
                ++instr;
                break;

            case '\n':
                column = 1;
                ++line;
                break;
        }
    }

    ir->end = instr;
    return err;
}

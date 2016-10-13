#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "rpn_utilities.h"
#include "infix_to_rpn.h"

bool should_pop_stack(uint8_t command, uint8_t prev_command){
    bool a = is_right_associative(command);
    bool b = operator_precedence(prev_command) < operator_precedence(command);
    bool c = !is_right_associative(command);
    bool d = operator_precedence(prev_command) <= operator_precedence(command);
    return (a && b) || (c && d);
}

void append(struct stack *s, uint8_t c){
    memcpy(&s->content[s->length], &c, 1);
    s->length++;
}

void pop_stack(struct stack *s){
    s->content[s->length] = '0';
    s->length--;
}

void update_infix_stacks(struct stack *variable, struct stack *symbol,\
                            uint8_t command){
    if (is_variable(command)){
        append(variable, command);
    }
    else if (is_symbol(command)){
        if (command == ')'){
            while (true){
                uint8_t c = symbol->content[symbol->length - 1];
                if(is_valid_operator(c)){
                    append(variable, c);
                    pop_stack(symbol);
                }
                else if (is_parenthesis(c)){
                    pop_stack(symbol);
                    if (c == '(') break;
                }
            }
        } else if (is_valid_operator(command)) {
            uint8_t prev_operator = symbol->content[symbol->length - 1];
            while (symbol->length > 0 && !is_parenthesis(prev_operator)){
                prev_operator = symbol->content[symbol->length - 1];
                if (should_pop_stack(command, prev_operator)){
                    append(variable, prev_operator);
                    pop_stack(symbol);
                } else break;
            }
            append(symbol, command);
            
        } else append(symbol, command);
    }
}

const char * infix_to_rpn(const char * expr){
    struct stack variable_stack = { .content = {0}, .length = 0};
    struct stack symbol_stack = { .content = {0}, .length = 0};
    for (uint16_t i = 0; i < strlen(expr); i++){
        update_infix_stacks(&variable_stack, &symbol_stack, expr[i]);
    }
    
    for (int16_t i = symbol_stack.length-1; i >= 0; i--){
        variable_stack.content[variable_stack.length] = symbol_stack.content[i];
        variable_stack.length++;
    }
    char *result = calloc(1000, sizeof(char));
    memcpy(result, variable_stack.content, 1000);
    return result;
}

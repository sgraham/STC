
#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cstack.h>

using_cvec(i, int);
using_cvec(c, char);
using_cstack(i, cvec_i);
using_cstack(c, cvec_c);

int main() {
    cstack_i stack = cstack_i_init();
    cstack_c chars = cstack_c_init();

    for (int i=0; i<100; ++i)
        cstack_i_push(&stack, i*i);
    
    for (int i=0; i<90; ++i)
        cstack_i_pop(&stack);
    
    c_foreach (i, cstack_i, stack)
        printf("%d\n", *cstack_i_itval(i));
    printf("top: %d\n", *cstack_i_top(&stack));
}
# hook.s
.global changeDurabilityDamage
.extern durabilityDamageAdj
.extern originalFunc

changeDurabilityDamage:
    pushfq
    push %rax
    sub $16, %rsp
    movaps %xmm0, (%rsp)
    movss %xmm8, %xmm0
    call durabilityDamageAdj
    movss %xmm0, %xmm8
    subss %xmm8, %xmm9
    movaps (%rsp), %xmm0
    add $16, %rsp
    pop %rax
    popfq
    jmp *originalFunc(%rip)

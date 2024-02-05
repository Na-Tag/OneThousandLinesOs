
// version 1.0 "Hello World & printf"
// https://operating-system-in-1000-lines.vercel.app/ja/hello-world
#include "kernel.h"
#include "common.h"
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

// sbiの仕様に沿ってOpenSBIを呼び出すための関数
// 呼び出し規約における呼び出し先→callee(openSBI側), 呼び出し元→caller
struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid)
{
    // register + __asm__("レジスタ名") は
    // 指定したレジスタに値を入れるようにコンパイラに指示するもの
    register long a0 __asm__("a0") = arg0; // returing an error code.
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid; // SBI function ID(FID)
    register long a7 __asm__("a7") = eid; // SBI extension ID(EID)

    __asm__ __volatile__("ecall" // Control transfer instruction between the supervisor and the SEE.
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");
    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch)
{
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /*Console Putchar extension*/);
}
void *
memoryset(void *buf, char c, size_t n)
{
    uint8_t *p = (uint8_t *)buf;
    while (n--)
        *p++ = c;
    return buf;
}

void kernel_main(void)
{
    printf("\n\nHello %s\n", "World!");
    printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);
    for (;;)
    {
        __asm__ __volatile__("wfi"); // WFI: Wait For Interrupt 割り込み待ち
    }
}

__attribute__((section(".text.boot")))
__attribute__((naked))

void
boot(void)
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r"(__stack_top));
}
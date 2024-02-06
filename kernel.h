#pragma once

// kernelパニック
// エラー内容を表示した後に無限ループに入って処理を停止する。
#define PANIC(fmt, ...)                                                       \
    do                                                                        \
    {                                                                         \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        while (1)                                                             \
        {                                                                     \
        }                                                                     \
    } while (0)

struct sbiret
{
    long error;
    long value;
};

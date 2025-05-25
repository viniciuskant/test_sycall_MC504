#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define __NR_read_mc504 468

int main() {
    char buffer[128];
    int ret;

    ret = syscall(__NR_read_mc504, 0, buffer, sizeof(buffer));
    if (ret < 0) {
        perror("syscall read_mc504 failed");
        return 1;
    }

    printf("%s\n", buffer);
    return 0;
}
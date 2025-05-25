#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define SYS_set_logging_level 469
#define SYS_get_logging_level 470  

int set_logging_level(int subsystem_id, int level) {
    return syscall(SYS_set_logging_level, subsystem_id, level);
}

int get_logging_level(int subsystem_id) {
    return syscall(SYS_get_logging_level, subsystem_id);
}

int main() {
    int subsystem_id = 5;
    int old_level, new_level, current_level;

    // Verifica se as syscalls existem
    errno = 0;
    if (syscall(SYS_set_logging_level, subsystem_id, 0) == -1 && errno == ENOSYS) {
        fprintf(stderr, "SYS_set_logging_level não existe neste sistema.\n");
        return 1;
    }
    errno = 0;
    if (syscall(SYS_get_logging_level, subsystem_id) == -1 && errno == ENOSYS) {
        fprintf(stderr, "SYS_get_logging_level não existe neste sistema.\n");
        return 1;
    }

    // Mostra o nível atual
    old_level = get_logging_level(subsystem_id);
    if (old_level < 0) {
        perror("get_logging_level failed");
        return 1;
    }
    printf("Nível atual do subsistema %d: %d\n", subsystem_id, old_level);

    // Define um novo nível de logging
    new_level = (old_level == 0) ? 2 : 0;
    printf("Alterando o nível de logging do subsistema %d para %d...\n", subsystem_id, new_level);
    if (set_logging_level(subsystem_id, new_level) < 0) {
        perror("set_logging_level failed");
        return 1;
    }

    // Mostra o novo nível
    current_level = get_logging_level(subsystem_id);
    if (current_level < 0) {
        perror("get_logging_level failed");
        return 1;
    }
    printf("Novo nível de logging do subsistema %d: %d\n", subsystem_id, current_level);

    // Restaura o nível antigo para fins demonstrativos
    printf("Restaurando o nível antigo (%d)...\n", old_level);
    if (set_logging_level(subsystem_id, old_level) < 0) {
        perror("set_logging_level failed");
        return 1;
    }
    printf("Nível restaurado com sucesso.\n");

    return 0;
}
# SYSCALL MC504

Este repositório é dedicado a demonstrar a funcionalidade da implementação de 3 syscalls para a disciplina MC504, que estão nesse [fork](https://github.com/viniciuskant/syscall_MC504/) que fizemos do kernel. 

Para a implementação dessas syscalls, seguimos os seguintes materiais de referência:

- [Documentação oficial do kernel.org sobre syscalls](https://www.kernel.org/doc/html/latest/process/adding-syscalls.html#compatibility-system-calls-generic)
- [Material do LKCamp sobre system calls](https://docs.lkcamp.dev/unicamp_group/systemcalls/)

Em resumo, mudamos esses arquivos:

- **Kconfig**: [Documentação](https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html). Colocar uma descrição da nova syscall (feito na linha 2104).

- **arch/x86/entry/syscalls/syscall_64.tbl**: Define o nome e o ponteiro para a syscall (essa é a tabela de syscall do x86).  
  Caso queira tratar a compatibilidade de 32 e 64 bits devo usar (mas não para x86):
  ```c
  #define __NR_xyzzy 292
  __SC_COMP(__NR_xyzzy, sys_xyzzy, compat_sys_xyzzy)
  ```
  Exemplo adicionado:
  ```text
  468 64      read_mc504          sys_read_mc504
  ```
  (linha 394)

- **kernel/Makefile**: Responsável por adicionar de fato o arquivo da syscall na compilação. Por uma questão de organização, o código da syscall em si fica na mesma pasta que esse Makefile.
  ```makefile
  obj-y += read_mc504.o
  ```
  (linha 19)

- **include/uapi/asm-generic/unistd.h**: Para adicionar a nova syscall à lista genérica de chamadas de sistema. Define uma espécie de contador das syscalls e deve atualizar o `__NR_syscalls` para refletir a chamada adicional do sistema (linha 856 do arquivo).  
  Existe um macro `__SYSCALL`, que tem dois parâmetros: o número da syscall e o rótulo dela, que fica definido em `include/linux/syscall.h`.

- **include/linux/syscall.h**: Define o cabeçalho da função, o ponto de entrada da syscall para o sistema, e são marcadas com `asmlinkage`. Isso é para um sistema genérico, mas há algumas arquiteturas, como o x86, que têm uma maneira específica de declarar a syscall.
  ```c
  asmlinkage long sys_read_mc504(int fd, char __user *buf, size_t count);
  ```
  (linha 1002)

- **sys_ni.c**: Já me perdi, mas é um binário, usar `xxd` para abrir.

- **Implementação da syscall**:  
  Crie um arquivo chamado `read_mc504.c` dentro do diretório `kernel` para implementar a nova syscall.

  Utilize a macro `SYSCALL_DEFINE3` para definir a função da syscall, seguindo o exemplo:
  ```c
  SYSCALL_DEFINE3(membarrier, int, cmd, unsigned int, flags, int, cpu_id)
  ```
  A macro `SYSCALL_DEFINEn` (onde n é o número de argumentos) define corretamente o ponto de entrada da syscall, evitando problemas de nomeação e ligação. Por exemplo, para uma syscall com três argumentos:
  ```c
  SYSCALL_DEFINE4(read_mc504, int, fd, char __user *, buf, size_t, count)
  ```
  O uso dessa macro permite que o kernel gere os metadados necessários para a nova chamada de sistema, facilitando a integração e manutenção.
  
Isso inclui apenas as mudanças para o read_mc504, que foi um teste que fizemos, mas basicamente replicamos a mesma ideia e arquivos para a implementação das outras duas syscalls. 

As diferenças que fizemos no projeto podem ser vista com mais facilidade por meio de um diff que foi executado entre o repositorio original e o que modificamos:

```diff
$ diff linux linux_original -r --exclude=.git

diff --color -r '--exclude=.git' linux/arch/x86/entry/syscalls/syscall_64.tbl linux_original/arch/x86/entry/syscalls/syscall_64.tbl
394,396d393
< 468 common  read_mc504          sys_read_mc504
< 469 common  set_logging_level   sys_set_logging_level
< 470 common  get_logging_level   sys_get_logging_level

diff --color -r '--exclude=.git' linux/include/linux/syscalls.h linux_original/include/linux/syscalls.h
1001,1006d1000
< //mc504
< asmlinkage long sys_read_mc504(int fd, char __user *buf, size_t count);
< asmlinkage long sys_set_logging_level(int subsystem_id, int level);
< asmlinkage long sys_get_logging_level(int subsystem_id);

diff --color -r '--exclude=.git' linux/include/uapi/asm-generic/unistd.h linux_original/include/uapi/asm-generic/unistd.h
854,859d853
< #define __NR_read_mc504 468
< __SYSCALL(__NR_read_mc504, sys_read_mc504)
< #define __NR_set_logging_level 469
< __SYSCALL(__NR_set_logging_level, sys_set_logging_level)
< #define __NR_get_logging_level 470
< __SYSCALL(__NR_get_logging_level, sys_get_logging_level)
862,863c856
< #define __NR_syscalls 471 //aumentei o valor para replicar a mudança no systema
<
---
> #define __NR_syscalls 468

diff --color -r '--exclude=.git' linux/init/Kconfig linux_original/init/Kconfig
2104,2112d2103
< #MC504, ainda não achei onde consigo definir isso como y ou n, para ativar e desativar durante a compilação
< config CONFIG_MC504_SYSCALL
< 	bool
< 	help
< 		Ativa as syscalls que foram implementadas para a disciplina MC504, que são
< 		* read_mc504: uma syscall que faz a leitura, porém diferente da syscall padrão de leitura, ela coloca "MC504 TESTE"
< 		* sys_set_logging_level: uma syscall que define o nível de log de uma parte específica do kernel;
< 		* sys_get_logging_level: uma syscall que lê o nível de log de uma parte específica do kernel;

diff --color -r '--exclude=.git' linux/kernel/Makefile linux_original/kernel/Makefile
166,169d165
< # Irei sempre adicionar por padrão, sem a necessidade de flag
< obj-y += syscall_mc504.o

Only in linux/kernel: syscall_mc504.c
```

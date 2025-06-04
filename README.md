# Projeto Kernel Linux I - Syscall

Este repositório é dedicado a demonstrar a funcionalidade da implementação de 3 syscalls para a disciplina MC504, que estão nesse [fork](https://github.com/viniciuskant/syscall_MC504/tree/mc504) que fizemos do kernel. 

Para a implementação dessas syscalls, seguimos os seguintes materiais de referência:

- [Documentação oficial do kernel.org sobre syscalls](https://www.kernel.org/doc/html/latest/process/adding-syscalls.html#compatibility-system-calls-generic)
- [Material do LKCamp sobre system calls](https://docs.lkcamp.dev/unicamp_group/systemcalls/)

---

## Mudanças feitas no kernel

Em resumo, alteramos esses arquivos:

- **Kconfig**: [Documentação](https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html). Colocar uma descrição da nova syscall (feito na linha 2104).

- **arch/x86/entry/syscalls/syscall_64.tbl**: Define o nome e o ponteiro para a syscall (essa é a tabela de syscall do x86).  
  Exemplo adicionado:
  ```text
  468 64      read_mc504          sys_read_mc504
  ```
  (linha 394)

- **kernel/Makefile**: Responsável por adicionar de fato o arquivo da syscall na compilação. Por uma questão de organização, o código da syscall em si fica na mesma pasta que esse Makefile.
  ```makefile
  obj-y += syscall_mc504.o
  ```
  (linha 19)

- **include/uapi/asm-generic/unistd.h**: Para adicionar a nova syscall à lista genérica de chamadas de sistema. Define uma espécie de contador das syscalls e deve atualizar o `__NR_syscalls` para refletir a chamada adicional do sistema (linha 856 do arquivo).  
  Existe um macro `__SYSCALL`, que tem dois parâmetros: o número da syscall e o rótulo dela, que fica definido em `include/linux/syscall.h`.

- **include/linux/syscall.h**: Define o cabeçalho da função, o ponto de entrada da syscall para o sistema, e são marcadas com `asmlinkage`. Isso é para um sistema genérico, mas há algumas arquiteturas, como o x86, que têm uma maneira específica de declarar a syscall.
  ```c
  asmlinkage long sys_read_mc504(int fd, char __user *buf, size_t count);
  ```
  (linha 1002)

- **Implementação da syscall**:  
  A syscall `read_mc504` realiza uma leitura modificada de um arquivo: além de ler até `count` bytes do descritor de arquivo `fd`, ela concatena ao final do conteúdo lido a string `"MC504 TESTE"` antes de copiá-lo para o buffer de usuário `buf`. Se um ponteiro de posição `pos` for fornecido, ele é atualizado conforme a leitura.  
  Essa syscall pode ser útil para fins de teste durante o desenvolvimento do kernel.

  A syscall `set_logging_level` permite configurar o nível de log de um subsistema específico do kernel. Ela recebe dois parâmetros: um identificador do subsistema (`subsystem_id`) e o nível desejado (`level`). Com isso, é possível controlar dinamicamente a verbosidade da saída de logs de partes específicas do kernel.

  A syscall `get_logging_level` retorna o nível de log atual de um subsistema identificado por `subsystem_id`. Ela facilita a inspeção e depuração do sistema, permitindo que programas em espaço de usuário verifiquem o estado da configuração de logging de diferentes partes do kernel.


---

## Conceito de Subsistema

Dentro do kernel, um **subsistema** é uma parte isolada e especializada que implementa funcionalidades específicas, como gerenciamento de processos, rede, sistema de arquivos, etc.

No nosso caso, as syscalls `set_logging_level` e `get_logging_level` trabalham com o conceito de subsistemas para permitir ativar/desativar logs para partes específicas do kernel.

---

## Diff entre o kernel original e o modificado

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
< config CONFIG_MC504_SYSCALL
< 	bool
< 	help
< 		Ativa as syscalls que foram implementadas para a disciplina MC504, que são
< 		* read_mc504: uma syscall que faz a leitura, porém diferente da syscall padrão de leitura, ela coloca "MC504 TESTE"
< 		* sys_set_logging_level: uma syscall que define o nível de log de uma parte específica do kernel;
< 		* sys_get_logging_level: uma syscall que lê o nível de log de uma parte específica do kernel;

diff --color -r '--exclude=.git' linux/kernel/Makefile linux_original/kernel/Makefile
166,169d165
< obj-y += syscall_mc504.o

Only in linux/kernel: syscall_mc504.c
```

## Arquivos de Teste das Syscalls

Foram desenvolvidos dois arquivos de teste em C para validar o funcionamento das syscalls implementadas: `read_mc504`, `set_logging_level` e `get_logging_level`.

### `test_file.c`

Este programa testa a syscall `read_mc504`, que é uma variação da syscall de leitura padrão do Linux. Essa syscall faz a leitura de dados do descritor de arquivo `fd` (no caso, `0`, que é a entrada padrão) e concatena a string `"MC504 TESTE"` ao final dos dados lidos antes de copiá-los para o buffer do usuário.

**Funcionamento do programa:**
- Declara um buffer de 128 bytes.
- Faz uma chamada direta à syscall `read_mc504` usando `syscall(__NR_read_mc504, 0, buffer, sizeof(buffer))`.
- Caso a chamada falhe, imprime uma mensagem de erro.
- Caso tenha sucesso, imprime o conteúdo do buffer (que deve incluir `"MC504 TESTE"`).

Esse programa serve para verificar rapidamente se a syscall foi corretamente registrada e executa como esperado no kernel modificado.

---

### `logging_level.c`

Este programa testa as syscalls `set_logging_level` e `get_logging_level`, responsáveis por definir e consultar o nível de logging de um subsistema do kernel.

**Funcionamento do programa:**
- Define dois wrappers simples para as syscalls:
  - `set_logging_level(int subsystem_id, int level)`
  - `get_logging_level(int subsystem_id)`
- Usa `syscall()` diretamente para verificar se as syscalls estão disponíveis no sistema (verificando `errno == ENOSYS`).
- Consulta o nível atual de logging do subsistema `subsystem_id = 5`.
- Alterna o nível de logging: se for `0`, define como `2`; caso contrário, define como `0`.
- Consulta novamente para verificar se a mudança foi aplicada.
- Por fim, restaura o nível original para fins de consistência na demonstração.

Esse programa permite validar o funcionamento das duas syscalls associadas ao controle de logging, além de servir como exemplo prático de uso em espaço de usuário.

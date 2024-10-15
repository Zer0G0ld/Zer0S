# Zer0S - Sistema Operacional Personalizado

Este projeto é uma tentativa de criar um sistema operacional básico do zero. Ele envolve o desenvolvimento de um bootloader simples em Assembly e um kernel em C, utilizando o NASM para compilar o código Assembly e o GCC para o código C. O objetivo é construir um sistema operacional básico com funcionalidades iniciais, como um terminal simples para interações com o usuário.

Para esse projeto, vou usar um outro repositório que estou desenvolvendo, o [ShellZer0](https://github.com/Zer0G0ld/ShellZer0), para ser um tipo de "Shell" deste SO.

> **Nota Importante**: Como o ShellZer0 é um submódulo, é importante saber como gerenciá-lo corretamente, especialmente ao clonar ou atualizar o repositório.

## Estrutura do Projeto

```
Zer0S/
├── boot/               # Código do bootloader em Assembly
│   └── boot.asm        # Código do bootloader
├── src/                # Código-fonte do kernel e outros módulos em C
│   ├── kernel.c        # Código principal do kernel
│   ├── gdt.c           # Inicialização da GDT
│   ├── isr.c           # Tratadores de interrupção
│   ├── idt.c           # Inicialização da IDT (interrupção)
├── include/            # Arquivos de cabeçalho
│   ├── gdt.h           # Arquivo de cabeçalho da GDT
│   ├── isr.h           # Arquivo de cabeçalho dos tratadores de interrupção
│   ├── idt.h           # Arquivo de cabeçalho da IDT
│   ├── gdt.h           # Cabeçalho para GDT
│   └── kernel.h        # Cabeçalho do kernel
├── build/              # Diretório onde os binários serão gerados
│   ├── boot.bin        # Arquivo do bootloader compilado
│   ├── kernel.o        # Arquivo objeto do kernel
│   ├── kernel.bin      # Arquivo binário do kernel
│   ├── myos.iso        # Imagem ISO gerada
│   └── boot.img        # Imagem IMG (se necessário)
├── terminal/           # Código-fonte do terminal (shell)
│   ├── shell.c         # Código do shell interativo
│   ├── shell.h         # Cabeçalho do shell
├── grub/               # Arquivos relacionados ao GRUB
│   └── grub.cfg        # Arquivo de configuração do GRUB
├── README.md           # Documentação do projeto
├── Makefile            # Arquivo Makefile para automação do processo
├── linker.ld           # Arquivo de script do linker
└── iso/                # Estrutura de diretórios para gerar a ISO
    └── boot/           # Diretórios para o GRUB e kernel dentro da imagem ISO
        └── grub/
            └── grub.cfg # Arquivo de configuração do GRUB na imagem ISO

```

## Requisitos

Antes de rodar o projeto, você precisa instalar as seguintes ferramentas no **Debian** ou **WSL** (Windows Subsystem for Linux):

- **NASM**: Para compilar o código Assembly.
  ```bash
  sudo apt install nasm
  ```
- **GCC (cross-compiler)**: Para compilar o kernel em C. Certifique-se de instalar o compilador apropriado para sistemas operacionais (`i686-elf-gcc`).
  ```bash
  sudo apt install build-essential
  sudo apt install gcc-multilib
  ```
- **QEMU**: Um emulador para testar o sistema operacional sem hardware físico.
  ```bash
  sudo apt install qemu-system-x86
  ```

> **Nota**: Se estiver utilizando outra distribuição do Linux, consulte o manual de sua distribuição para a instalação das dependências.

## Lógica do Projeto

### Bootloader (boot.asm)

O bootloader é responsável por inicializar o sistema e carregar o kernel na memória. Neste projeto, o bootloader é um programa simples escrito em Assembly que apenas exibe uma mensagem na tela ("HI") e depois trava o sistema (CLI e HLT).

- O bootloader opera no **modo real (16 bits)**.
- Ele escreve diretamente no endereço de vídeo de texto (0xB8000) para exibir caracteres na tela.

### Kernel (kernel.c)

O kernel é um programa básico escrito em C que, quando carregado pelo bootloader, escreve a mensagem "Hello, OS World!" na tela.

- O kernel opera no **modo protegido (32 bits)**.
- Ele também escreve diretamente na memória de vídeo para exibir caracteres na tela.

### Terminal (shell)

O projeto agora inclui um terminal (ou shell), que permite ao usuário interagir com o sistema operacional. O terminal é um loop que aguarda os comandos do usuário, processa-os e executa ações como exibir mensagens ou encerrar o sistema.

- O terminal é escrito em C e funciona dentro do kernel.
- O shell permite comandos simples como `hello` para exibir uma saudação e `shutdown` para encerrar o sistema.
- O terminal será integrado ao Zer0S através do submódulo [ShellZer0](https://github.com/Zer0G0ld/ShellZer0).

## Como rodar:

Primeiro, clone o repositório:

```bash
git clone https://github.com/Zer0G0ld/Zer0S.git
```

Acesse o diretório:

```bash
cd Zer0S
```

## Compilando o Projeto

### Compilação do Bootloader

Para compilar o bootloader em Assembly, navegue até o diretório `boot/` e execute o seguinte comando:

```bash
nasm -f bin boot.asm -o ../build/boot.bin
```

### Compilação do Kernel

Para compilar o kernel em C, navegue até o diretório `src/` e execute:

```bash
i686-elf-gcc -ffreestanding -c kernel.c -o ../build/kernel.o
```

### Ligação do Bootloader e Kernel

Após compilar ambos, você precisa combinar o bootloader e o kernel. Isso pode ser feito com o uso de ferramentas adicionais, como `ld` (linker). Um exemplo de comando de linkagem seria:

```bash
ld -T link.ld -o ../build/MyOS.bin -O binary boot.o kernel.o
```

Mas para simplificar, podemos inicialmente usar o bootloader isoladamente.

## Executando o Projeto

Para rodar o sistema operacional, você pode utilizar o **QEMU**. Após compilar o bootloader, execute o seguinte comando para emular o SO:

```bash
qemu-system-x86_64 -drive format=raw,file=build/boot.bin
```

Este comando vai iniciar a emulação e exibir a mensagem do bootloader.

## Compilando e Executando o Projeto com Makefile

Para facilitar o processo de compilação, linkagem e execução, foi adicionado um `Makefile` ao projeto. Com ele, você pode compilar o bootloader, o kernel e gerar a imagem ISO para rodar o sistema operacional de maneira automatizada.

### Comandos Disponíveis no Makefile

- **Compilar o Projeto**: Este comando irá compilar tanto o bootloader quanto o kernel, além de gerar a imagem ISO pronta para ser executada.
  
  ```bash
  make
  ```

- **Limpar o Diretório Build**: Este comando remove todos os arquivos gerados durante a compilação (binários, objetos, e a ISO), permitindo uma recompilação limpa.

  ```bash
  make clean
  ```

- **Executar o Sistema Operacional no QEMU**: Após a compilação, você pode usar este comando para rodar a imagem ISO diretamente no emulador QEMU e testar o sistema operacional.

  ```bash
  make run
  ```

### Estrutura Interna do Makefile

O **Makefile** foi configurado para automatizar os seguintes passos:

1. Compilação do bootloader (código Assembly).
2. Compilação do kernel (código C).
3. Linkagem dos arquivos compilados (bootloader e kernel) para gerar um binário combinado.
4. Criação da imagem ISO utilizando os arquivos necessários (bootloader, kernel e GRUB).
5. Emulação do sistema operacional no QEMU.

### Exemplo de Uso

1. Para compilar o projeto:

  ```bash
  make
  ```

Isso irá compilar o bootloader e o kernel e gerar a ISO (`zer0.iso`) na pasta `build/`.

2. Para rodar o sistema no QEMU após a compilação:

  ```bash
  make run
  ```

3. Para limpar os arquivos gerados e recompilar do zero:

  ```bash
  make clean
  ```

### Futuras Melhorias no Makefile

- **Teste Automatizado**: No futuro, podemos adicionar testes automatizados ao Makefile para verificar o funcionamento de novos componentes à medida que o projeto evolui.
- **Suporte a Várias Arquiteturas**: Expandir o Makefile para compilar o sistema operacional para outras arquiteturas, além do x86.

### Testando o Terminal

Para testar

 o terminal, após compilar o projeto, execute o comando `make run` e interaja com o terminal utilizando comandos simples como `hello` e `shutdown`.

## Melhorias Futuras

- **Sistema de Gerenciamento de Processos**: Planejo implementar a funcionalidade de gerenciamento de processos no kernel, permitindo que múltiplos programas sejam executados simultaneamente.
- **Suporte a Múltiplos Terminais**: Aumentar a funcionalidade do terminal, permitindo que múltiplos terminais possam ser abertos simultaneamente.

## Contribuições

Contribuições são bem-vindas! Se você gostaria de contribuir para o projeto, basta seguir as etapas abaixo:

1. Faça um fork deste repositório.
2. Crie uma branch com sua feature ou correção.
3. Envie um pull request com uma descrição clara da sua alteração.

## Licença

Este projeto está licenciado sob a MIT License. Veja o arquivo LICENSE para mais informações.


### Principais Modificações:
- Uniformização dos títulos e formatação.
- Adição de notas para alertar sobre etapas críticas (como o uso do submódulo).
- Explicações mais claras sobre a instalação das dependências.
- Descrição detalhada do Makefile e seus comandos.
- Links e referências extras para facilitar a navegação e entendimento.


# Zer0S - Sistema Operacional Personalizado

Este projeto é uma tentativa de criar um sistema operacional básico do zero. Ele envolve o desenvolvimento de um bootloader simples em Assembly e um kernel em C, utilizando o NASM para compilar o código Assembly e o GCC para o código C. O objetivo é construir um sistema operacional básico com funcionalidades iniciais, como um terminal simples para interações com o usuário.

## Estrutura do Projeto

```
Zer0S/
├── boot/               # Código do bootloader em Assembly
│   └── boot.asm
├── src/                # Código-fonte do kernel em C
│   └── kernel.c
├── include/            # Arquivos de cabeçalho, se necessário
├── build/              # Diretório onde os binários serão gerados
│   ├── boot.bin        # Arquivo do bootloader compilado
│   ├── kernel.o        # Arquivo objeto do kernel
│   ├── kernel.bin      # Arquivo binário do kernel
│   ├── myos.iso        # Imagem ISO gerada
│   └── boot.img        # Imagem IMG (se necessário)
├── terminal/           # Código-fonte do terminal (shell)
├── grub/               # Arquivos relacionados ao GRUB
│   └── grub.cfg        # Arquivo de configuração do GRUB
├── README.md           # Documentação do projeto
├── Makefile            # Arquivo Makefile para automação do processo
├── linker.ld           # Arquivo de script do linker (novo)
└── iso/                # Estrutura de diretórios para gerar a ISO
    └── boot/           # Diretórios para o GRUB e kernel dentro da imagem ISO
        └── grub/
            └── grub.cfg # Arquivo de configuração do GRUB na imagem ISO
```

## Requisitos

Antes de rodar o projeto, você precisa instalar as seguintes ferramentas no Debian ou WSL:

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

## Lógica do Projeto

### Bootloader (boot.asm)

O bootloader é responsável por inicializar o sistema e carregar o kernel na memória. Neste projeto, o bootloader é um programa simples escrito em Assembly que apenas exibe uma mensagem na tela ("HI") e depois trava o sistema (CLI e HLT).

- O bootloader opera no modo real (16 bits).
- Ele escreve diretamente no endereço de vídeo de texto (0xB8000) para exibir caracteres na tela.

### Kernel (kernel.c)

O kernel é um programa básico escrito em C que, quando carregado pelo bootloader, escreve a mensagem "Hello, OS World!" na tela.

- O kernel opera no modo protegido (32 bits).
- Ele também escreve diretamente na memória de vídeo para exibir caracteres na tela.

### Terminal (shell)

O projeto agora inclui um terminal (ou shell), que permite ao usuário interagir com o sistema operacional. O terminal é um loop que aguarda os comandos do usuário, processa-os e executa ações como exibir mensagens ou encerrar o sistema.

- O terminal é escrito em C e funciona dentro do kernel.
- O shell permite comandos simples como `hello` para exibir uma saudação e `shutdown` para encerrar o sistema.

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

Para rodar o sistema operacional, você pode utilizar o QEMU. Após compilar o bootloader, execute o seguinte comando para emular o SO:

```bash
qemu-system-x86_64 -drive format=raw,file=build/boot.bin
```

Este comando vai iniciar a emulação e exibir a mensagem do bootloader.

### Testando o Terminal

Para testar o terminal, você pode compilar o kernel com o código do shell e emular novamente no QEMU. A interação com o terminal permite que você digite comandos simples como `hello` ou `shutdown`.

### Futuras Melhorias

- Carregamento do kernel pelo bootloader.
- Gerenciamento básico de memória.
- Interação com dispositivos de hardware.
- Implementação de um sistema de arquivos rudimentar.
- Expansão das funcionalidades do terminal com novos comandos.

## Referências

- [OSDev Wiki](https://wiki.osdev.org)
- Livro: **"Operating Systems: Three Easy Pieces"**

## Licença

Este projeto está em fase experimental e não possui uma licença formal no momento.

# Zer0S - Sistema Operacional Personalizado 🚀

Zer0S é um sistema operacional hobby, feito do zero em **C e Assembly**, com foco educacional. A ideia é aprender, experimentar arquitetura x86, entender como funcionam **bootloaders**, **GDT**, **IDT**, **interrupções**, **modo protegido**, **VGA Text Mode** e um **Shell simples**.

> **Importante:** Este projeto não é um sistema operacional para produção. É um projeto educacional/de aprendizado. Mas quem sabe no futuro... pode virar algo maior e mais legal 😄.



---

## Funcionalidades atuais ✅

* Boot com GRUB (Multiboot)
* GDT (Global Descriptor Table)
* IDT (Interrupt Descriptor Table)
* Tratamento básico de exceções (ISRs e IRQs)
* VGA Text Mode (modo texto 80x25, 16 cores)
* Print de strings direto na VRAM
* Limpeza de tela com cores configuráveis
* Loop de Shell simples (`shell_main()`)
* Compilação automatizada com `Makefile`
* Geração de imagem ISO bootável
* Execução via QEMU

---

## Estrutura de Pastas 🗂️

```bash
Zer0S/
├── build/              # Arquivos compilados (binários, objetos, ISO)
├── grub/               # Arquivo grub.cfg (Multiboot config)
├── include/            # Headers (GDT, IDT, ISR, screen, shell, etc)
├── iso/                # Estrutura da ISO final (boot/grub + kernel)
├── src/                # Código-fonte (C e Assembly)
├── linker.ld           # Linker script
├── Makefile            # Compilação e automação
├── README.md           # Este arquivo
└── LICENSE             # Licença MIT
```

---

## Requisitos 🛠️

* **GCC Cross Compiler 32-bit (i686-elf-gcc)**
* **NASM** (para o Assembly)
* **QEMU** (para testar)

### Instalação (Debian/Ubuntu/WSL)

```bash
sudo apt install build-essential gcc-multilib nasm qemu-system-x86
```

Para o GCC cross-compiler:

```bash
sudo apt install binutils gcc
# Ou siga tutoriais de como criar um i686-elf-gcc (recomendado para kernels reais)
```

---

## Compilação 🚧

```bash
make iso
```

Isso vai:

* Compilar todo o kernel
* Gerar `kernel.elf`
* Criar a ISO bootável (`zer0s.iso` dentro da pasta build/)

### Para limpar a build:

```bash
make clean
```

---

## Execução 🚀

Para rodar no QEMU:

```bash
make run
```

Ou manualmente:

```bash
qemu-system-x86_64 -cdrom build/zer0s.iso
```

---

## O que o Zer0S faz hoje?

✅ Inicializa com GRUB

✅ Configura a GDT (modo protegido)

✅ Inicializa a IDT

✅ Habilita interrupções

✅ Escreve mensagens de boas-vindas na tela

✅ Limpa a tela com fundo/texto configurável

✅ Entra no loop do shell (por enquanto um `while(1)`, mas pronto pra expansão!)

---

## Próximos Passos 🚀 (Roadmap Pessoal)

✔️ Capturar teclado (IRQ1 - PS/2)

✔️ Fazer o shell aceitar entrada de texto

✔️ Tratar comandos simples

✔️ Implementar suporte a timers (PIT)

✔️ Trabalhar com memória (paging e heap)

✔️ Fazer multitarefa rudimentar

✔️ Criar um mini filesystem

✔️ Quem sabe... carregar binários ELF no futuro 😄

---

## Licença 📝

[GNU3](./LICENSE)
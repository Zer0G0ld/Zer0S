# Zer0S Kernel - Global Descriptor Table (GDT)

## O que é a GDT?

A **Global Descriptor Table (GDT)** é uma tabela especial usada pelos processadores x86 para definir as características dos **segmentos de memória** que o sistema vai utilizar.

Cada entrada da GDT descreve um pedaço da memória (um **segmento**), informando:

* Onde o segmento começa (Base Address)
* Qual o tamanho dele (Limit)
* Quais são as permissões e características de acesso (Flags de Acesso e Granularidade)

---

## Por que a GDT é importante?

A GDT é essencial para:

✅ Organizar a memória em **segmentos de código**, **dados** e **pilha**
✅ Definir os **níveis de privilégio** (Kernel Mode / User Mode)
✅ Permitir **proteção de memória**, evitando que processos de usuário mexam no kernel
✅ Suportar operações como **mudança de contexto** e **multitarefa**
✅ Preparar o terreno para sistemas mais modernos de gerenciamento de memória, como o **Paging**

---

## Breve Histórico

* **Intel 8086 (1978):**
  Primeira arquitetura com segmentação simples (CS, DS, SS, ES) para acessar até 1MB de memória.

* **Intel 80286 (1982):**
  Introdução do **modo protegido** e das primeiras GDTs com controle de acesso e proteção.

* **Intel 80386 e além (1985+):**
  Expansão para 32 bits, surgimento do **Paging**, mas a GDT continuou sendo usada como base da proteção de memória.

---

## Estrutura de uma Entrada da GDT (Segment Descriptor)

Cada entrada na GDT tem **8 bytes (64 bits)** de tamanho. Aqui está a estrutura típica:

| Campo                   | Tamanho | Função                                                                       |
| ----------------------- | ------- | ---------------------------------------------------------------------------- |
| **Base Address**        | 32 bits | Endereço inicial do segmento                                                 |
| **Limit**               | 20 bits | Tamanho máximo do segmento                                                   |
| **Access Byte**         | 8 bits  | Tipo do segmento, nível de privilégio, presença                              |
| **Flags (Granularity)** | 4 bits  | Tamanho do segmento (4KB ou bytes), tamanho das instruções (16/32 bits), etc |

---

## Exemplo de Segmentos comuns na GDT

| Nº | Nome            | Função                                                |
| -- | --------------- | ----------------------------------------------------- |
| 0  | Null Descriptor | Reservado (proibição de usar segmento 0)              |
| 1  | Code Segment    | Segmento de código do Kernel (execução de instruções) |
| 2  | Data Segment    | Segmento de dados do Kernel                           |

> 🧱 Muitos kernels simples começam com exatamente **esses 3 segmentos básicos**.

---

## Como a GDT é usada no Zer0S

### Passos básicos:

1. **Criar a Tabela GDT na Memória:**

   * Reservamos espaço para os descritores.
   * Definimos base, limite e flags de cada entrada.

2. **Criar o ponteiro especial (GDT Pointer):**

   * Contém o tamanho da tabela (limit - 1).
   * Contém o endereço base da GDT.

3. **Carregar a GDT no processador (instrução `lgdt`):**

   * O processador passa a usar a nova GDT.

4. **Atualizar os Segment Registers (CS, DS, SS, etc):**

   * Fazemos isso com uma **far jump** para recarregar o CS.
   * Em seguida, atualizamos DS, ES, FS, GS, SS.

---

### Exemplo de código (C):

```c
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_flush();
```

---

## Proteção por Nível de Privilégio (Ring Levels)

A GDT define o **DPL (Descriptor Privilege Level)** de cada segmento.

| Ring | Nível       | Quem usa              |
| ---- | ----------- | --------------------- |
| 0    | Kernel Mode | Kernel e drivers      |
| 3    | User Mode   | Aplicações de usuário |

➡️ **Regra de Ouro:**
**Código em Ring 3 (usuário) não pode acessar segmentos Ring 0 (kernel).**

---

## Granularidade e Tamanho de Operação

A GDT também define:

* Se o segmento é endereçado por **bytes** ou **páginas de 4KB** (bit de Granularity).
* Se o código usa **16 bits** ou **32 bits** (bit de Size).

---

## Armadilha Clássica: Triple Fault

⚠️ Se a GDT for mal configurada ou estiver em um endereço inválido, o processador pode gerar:

* **General Protection Fault**
* **Double Fault**
* **Triple Fault** → Resultado: **RESET da máquina**

Por isso, carregar a GDT com cuidado é crucial no boot do kernel.

---

## Conclusão

A **GDT é a base da segmentação de memória no x86**, definindo as regras de acesso, proteção e layout de memória. Ela é um dos primeiros blocos fundamentais no desenvolvimento de qualquer kernel. Sem ela, o sistema não consegue nem rodar código de forma segura.

Próxima etapa da documentação: **Task State Segment (TSS)** e depois **Paging (Paginação)**.


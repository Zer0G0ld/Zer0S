# Zer0S Kernel - Interrupt Descriptor Table (IDT)

## O que é a IDT?

A **Interrupt Descriptor Table (IDT)** é uma tabela especial usada pelo processador x86 para gerenciar **interrupções** e **exceções**. Ela define como o sistema deve reagir quando um evento externo (como um hardware) ou interno (como uma exceção de software) ocorre.

Cada entrada na IDT indica **qual rotina de tratamento (Interrupt Handler)** o processador deve chamar para lidar com determinado tipo de interrupção.

---

## Função da IDT

* **Mapeamento de Interrupções:** A IDT associa um número de interrupção (vetor) a uma função específica de tratamento.
* **Resposta a Eventos:** Pode ser usada para lidar com eventos de hardware (teclado, timer, etc.) ou exceções internas (divisão por zero, página inválida, etc.).
* **Controle de Privilegios:** Define quais interrupções podem ser disparadas por processos em modo usuário ou apenas no kernel.

---

## Estrutura de uma Entrada da IDT

Cada entrada da IDT (chamada de **Gate**) contém informações como:

* **Offset:** Endereço da função de tratamento de interrupção.
* **Segment Selector:** Segmento de código onde a rotina está.
* **Tipo de Gate:** Pode ser **Interrupt Gate**, **Trap Gate** ou **Task Gate**.
* **DPL (Descriptor Privilege Level):** Define o nível de privilégio necessário para disparar aquela interrupção.
* **P (Present):** Se o handler está ativo (1) ou não (0).

---

### Tipos de Gates na IDT (x86):

| Tipo                      | Acesso              | Comportamento                                                           |
| ------------------------- | ------------------- | ----------------------------------------------------------------------- |
| **Interrupt Gate**        | Kernel Mode (DPL=0) | Bloqueia novas interrupções enquanto o handler roda                     |
| **Trap Gate**             | Kernel Mode (DPL=0) | Permite interrupções aninhadas                                          |
| **System Gate**           | User Mode (DPL=3)   | Permite que o usuário invoque interrupções controladas (ex: `int 0x80`) |
| **System Interrupt Gate** | User Mode (DPL=3)   | Permite certos handlers serem acionados pelo usuário (ex: `int3`)       |
| **Task Gate**             | Kernel Mode         | Realiza um switch de tarefa ao ser chamado                              |

---

## Como a IDT funciona?

Fluxo básico:

1. Um evento (hardware/software) dispara uma interrupção.
2. O processador consulta a IDT para saber qual função deve executar.
3. O processador salva o contexto da execução atual.
4. O handler configurado na IDT é chamado.
5. Ao terminar, a execução original é retomada via instrução `iret`.

---

## Inicialização da IDT no Zer0S

Durante a inicialização do Zer0S:

* Reservamos um espaço de memória para a IDT com **256 entradas** (padrão x86, uma para cada vetor de interrupção possível).
* Usamos instruções Assembly (como `lidt`) para carregar o endereço da IDT no **registrador IDTR**.
* Inicialmente todas as entradas da IDT apontam para um **handler padrão (ex: `ignore_int`)**, que apenas exibe "Unknown Interrupt" e retorna.
* Depois, vamos sobrescrevendo as entradas com os handlers reais (exceções, IRQs, etc.).

---

## Exemplo de Inicialização (Pseudo-Assembly / C):

```assembly
; Carregando o endereço da IDT no IDTR
lidt [idt_descriptor]
```

```c
void setup_idt() {
    for (int i = 0; i < 256; i++) {
        set_intr_gate(i, (uint32_t)ignore_int);
    }

    // Exemplo: configurando o handler de divisão por zero
    set_intr_gate(0, (uint32_t)div_zero_handler);
}
```

---

## Proteções de Segurança (DPL e User Mode)

Para evitar que processos em modo usuário disparem interrupções não autorizadas:

* As entradas da IDT para exceções críticas têm **DPL = 0** (só kernel acessa).
* As poucas interrupções que podem ser chamadas pelo usuário (ex: `int 0x80` para chamadas de sistema) têm **DPL = 3**.

Exemplo de proteção:
Se um processo usuário tentar chamar uma interrupção de DPL=0, o processador gera uma exceção de **General Protection Fault (GPF)**.

---

## Importância da IDT no Kernel

* Sem uma IDT válida e bem configurada, o sistema **não responde a hardware**, **não trata falhas**, e **não suporta multitarefa**.
* O correto funcionamento da IDT é um dos **fundamentos básicos de qualquer kernel funcional**.

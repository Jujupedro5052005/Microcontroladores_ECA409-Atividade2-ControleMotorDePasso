# 🚀 Desafio Embarcados — Controle de Posição de Motor de Passo com Potenciômetro  
### Raspberry Pi Pico + Pico SDK + Linguagem C

---

## 🎯 Objetivo

Desenvolver uma aplicação embarcada utilizando o **Raspberry Pi Pico** e o **Pico SDK**, na linguagem **C**, para realizar o **controle de posição de um motor de passo** a partir da leitura de um **potenciômetro**.

O sistema deverá:

- 🎚️ Ler continuamente a posição do potenciômetro
- 🎯 Converter essa leitura em uma **posição alvo**
- 🔄 Movimentar automaticamente o motor até a posição desejada
- ⏱️ Utilizar **timer repetitivo** para controle dos passos
- ⚙️ Garantir estabilidade e suavidade no movimento

---

## 🧠 Descrição do Funcionamento

O potenciômetro representa a **posição desejada do eixo do motor**.

O software mantém duas variáveis principais:

- 📍 `posicao_atual`
- 🎯 `posicao_alvo`

A cada ciclo:

1. O ADC lê o potenciômetro
2. O valor é convertido em posição alvo
3. O timer movimenta o motor **um passo por vez**
4. O motor gira:
   - ↻ **horário**
   - ↺ **anti-horário**

até que:

```c
posicao_atual == posicao_alvo
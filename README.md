# bisX OS (v0.1)

Minimalist, high-performance kernel and OS written in C and Assembly with a custom 32-bit TrueColor Direct-Framebuffer GUI. Designed for servers and embedded systems.

[English](#english) | [Русский](#русский) | [中文](#中文)

---

## English

### About
**bisX OS** is an independent operating system built from scratch. It bypasses traditional display servers (X11/Wayland) by implementing a direct VESA/VBE Framebuffer driver capable of rendering 32-bit TrueColor graphics (~16.7M colors).

### Tech Stack
* **Languages:** C, Assembly (NASM)
* **Bootloader:** GRUB (Multiboot)
* **Architecture:** x86_64
* **Display:** Custom Direct-Framebuffer Driver

### Features
* Bare-metal C core & assembly setup
* 32-bit TrueColor graphics engine
* Built-in `bisX Shell`
* Hardware interrupt handling (IDT/ISR)

---

## Русский

### О проекте
**bisX OS** — независимая операционная система, разрабатываемая с нуля. Работает напрямую с видеопамятью через собственный драйвер VESA/VBE Framebuffer (32-битный TrueColor, ~16.7 млн цветов) без использования X11 или Wayland.

### Стек
* **Языки:** C, Ассемблер (NASM)
* **Загрузчик:** GRUB (Multiboot)
* **Архитектура:** x86_64
* **Графика:** Кастомный драйвер фреймбуфера

### Фичи
* Чистое ядро на C и стартовый код на Ассемблере
* Графический движок (16.7 млн цветов)
* Встроенная оболочка `bisX Shell`
* Обработка прерываний (IDT/ISR)

---

## 中文

### 关于
**bisX OS** 是一个从零开发的独立操作系统。它绕过传统的显示服务器（X11/Wayland），直接通过自研的 VESA/VBE Framebuffer 驱动程序渲染 32 位 TrueColor 图形（约 1670 万色）。

### 技术栈
* **语言:** C, 汇编 (NASM)
* **引导程序:** GRUB (Multiboot)
* **架构:** x86_64
* **显示:** 自定义 Direct-Framebuffer 驱动

---

## Build & Run

```bash
make
make run

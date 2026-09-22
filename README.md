# bisX OS (v0.1)

![bisX OS](https://img.shields.io/badge/bisX--OS-Kernel%20v0.1-brightgreen?style=for-the-badge)
![C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge)
![Assembly](https://img.shields.io/badge/Language-Assembly-orange?style=for-the-badge)
![Architecture](https://img.shields.io/badge/Arch-x86__64-red?style=for-the-badge)

[![](https://img.shields.io/badge/Language-English-blue?style=for-the-badge)](#english)
[![](https://img.shields.io/badge/Язык-Русский-red?style=for-the-badge)](#русский)
[![](https://img.shields.io/badge/语言-中文-lightgrey?style=for-the-badge)](#中文)

---

## English

### Overview
bisX OS is a lightweight, high-performance operating system written from scratch in C and Assembly. It utilizes a custom 32-bit TrueColor Direct-Framebuffer GUI engine (~16.7M colors) operating directly on hardware without relying on display servers such as X11 or Wayland.

### Tech Stack
* Kernel Languages: C, Assembly (NASM)
* Bootloader: GRUB (Multiboot)
* Target Architecture: x86_64
* Display Engine: VBE/VESA Direct Framebuffer Driver

### Key Features
* Custom C core and assembly entry point
* 32-bit TrueColor graphics output
* Built-in bisX Shell interpreter
* Interrupt Handling (IDT/ISR)

---

## Русский

### Описание
bisX OS — легкая и высокопроизводительная операционная система, разрабатываемая с нуля на C и Ассемблере. Использует собственный графический движок 32-bit TrueColor Direct-Framebuffer (~16.7 млн цветов), работающий напрямую с видеопамятью без использования графических серверов вроде X11 или Wayland.

### Технологический стек
* Языки ядра: C, Ассемблер (NASM)
* Загрузчик: GRUB (Multiboot)
* Архитектура: x86_64
* Графический движок: Драйвер VBE/VESA Direct Framebuffer

### Основные возможности
* Собственное ядро на C и ассемблерный блок загрузки
* Вывод 32-битной TrueColor графики
* Встроенный командный интерпретатор bisX Shell
* Обработка аппаратных прерываний (IDT/ISR)

---

## 中文

### 概述
bisX OS 是一个从零开始采用 C 和汇编语言编写的高性能轻量级操作系统。它包含一个自定义的 32位 TrueColor Direct-Framebuffer 图形引擎（约 1670 万色），直接在硬件上运行，无需依赖 X11 或 Wayland 等显示服务器。

### 技术栈
* 内核语言: C, 汇编 (NASM)
* 引导程序: GRUB (Multiboot)
* 目标架构: x86_64
* 显示引擎: VBE/VESA Direct Framebuffer 驱动程序

---

## Preview

```text
+-------------------------------------------------------------+
| bisX OS v0.1 [Server Edition]                  Memory: 64MB |
+-------------------------------------------------------------+
|                                                             |
|  bisX Kernel Initialized Successfully.                       |
|  Framebuffer: 1024x768 (32-bit TrueColor)                   |
|                                                             |
|  bisX# sysinfo                                              |
|  OS Name: bisX OS                                           |
|  Version: 0.1                                               |
|  Architecture: x86_64                                       |
|                                                             |
|  bisX# _                                                    |
+-------------------------------------------------------------+

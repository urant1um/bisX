<div align="center">

# 🚀 bisX OS (v0.1)

**A lightweight, high-performance kernel & OS written in C and Assembly.**
Designed for servers, embedded hardware, and minimal systems with a custom Direct-Framebuffer GUI.

---

### 🌐 Select Language / Выберите язык / 語言選擇

[![English](https://img.shields.io/badge/Language-English-blue.svg?style=for-the-badge)](#-english)
[![Русский](https://img.shields.io/badge/Язык-Русский-red.svg?style=for-the-badge)](#-русский)
[![中文](https://img.shields.io/badge/語言-中文-red.svg?style=for-the-badge)](#-中文)

---

![bisX OS Banner](https://img.shields.io/badge/bisX--OS-Kernel%20v0.1-brightgreen?style=for-the-badge&logo=linux&logoColor=white)
![C](https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white)
![Assembly](https://img.shields.io/badge/Language-Assembly-6E4C13?style=for-the-badge&logo=assemblyscript&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)

</div>

---

## 🇬🇧 English

### 📌 Overview
**bisX OS** is a custom operating system built from scratch for servers and special-purpose embedded devices. It features a standalone monolithic kernel with direct hardware access, a minimal built-in command shell, and a custom **Direct Framebuffer GUI** capable of rendering **32-bit TrueColor Graphics** (16.7M+ colors) without relying on heavy display servers like X11 or Wayland.

### 🛠️ Tech Stack
* **Kernel Language:** C (Core Logic & Drivers) + Assembly (NASM/GAS for Bootstrapping & Interrupts)
* **Bootloader:** GRUB / Multiboot Specification
* **Target Architecture:** x86_64
* **Display Engine:** Custom VBE / VESA Framebuffer Driver (32-bit TrueColor)
* **Target Environment:** QEMU Virtual Machine & Bare-Metal Hardware

### 🚀 Key Features
- [x] Bare-metal C runtime & Assembly bootloader
- [x] Custom VGA/VESA Framebuffer GUI driver (16.7M Colors)
- [x] Integrated `bisX Shell` command interpreter
- [x] Memory management & IDT/ISR interrupt handling
- [x] Serial port logging & hardware debugging

---

## 🇷🇺 Русский

### 📌 Описание
**bisX OS** — это пользовательская операционная система, разрабатываемая с нуля для серверов и встраиваемых систем. Проект содержит собственное монолитное ядро, встроенную командную оболочку и уникальный **Direct Framebuffer GUI**, способный выводить **32-битный TrueColor графический интерфейс** (16.7 млн цветов) напрямую через видеопамять без тяжёлых подсистем вроде X11 или Wayland.

### 🛠️ Технологический стек
* **Язык ядра:** C (Основная логика и драйверы) + Ассемблер (NASM/GAS для инициализации процессора и прерываний)
* **Загрузчик:** GRUB / Спецификация Multiboot
* **Целевая архитектура:** x86_64
* **Графический движок:** Собственный VBE / VESA Framebuffer драйвер (32-бит TrueColor)
* **Среда тестирования:** Виртуальная машина QEMU и реальное железо

### 🚀 Основные возможности
- [x] Собственная C-среда исполнения и загрузочный код на Ассемблере
- [x] Кастомный графический движок Framebuffer (16.7 млн цветов)
- [x] Встроенный командный толмач `bisX Shell`
- [x] Менеджер памяти и обработка прерываний (IDT/ISR)
- [x] Вывод логов через Serial-порт для отладки сервера

---

## 🇨🇳 中文

### 📌 概述
**bisX OS** 是一个从零开始开发的自定义操作系统，专为服务器和专用嵌入式设备设计。它包含一个具有直接硬件访问权限的独立单体内核、内置的命令行 Shell 以及自定义的 **Direct Framebuffer GUI**，能够直接渲染 **32位 TrueColor 图形界面**（1670万色），无需依赖 X11 或 Wayland 等重型显示服务器。

### 🛠️ 技术栈
* **内核语言:** C语言 (核心逻辑与驱动) + 汇编语言 (NASM/GAS 用于引导与中断)
* **引导加载程序:** GRUB / Multiboot 规范
* **目标架构:** x86_64
* **显示引擎:** 自定义 VBE / VESA Framebuffer 驱动程序 (32位 TrueColor)
* **测试环境:** QEMU 虚拟机与物理硬件

### 🚀 核心特性
- [x] 独立 C 语言运行时与汇编引导程序
- [x] 自定义 Framebuffer 图形驱动程序 (1670万色)
- [x] 集成 `bisX Shell` 命令行解释器
- [x] 内存管理与 IDT/ISR 中断处理
- [x] 串口日志输出与硬件调试

---

## 💻 Building & Running

### Prerequisites
Make sure you have `gcc`, `nasm`, `make`, `grub-mkrescue`, and `qemu-system-x86_64` installed on your Linux system.

```bash
# Clone the repository
git clone [https://github.com/your-username/bisX-OS.git](https://github.com/your-username/bisX-OS.git)
cd bisX-OS

# Compile the kernel & create ISO image
make

# Run in QEMU
make run

# bisX OS (v0.1)

![bisX OS](https://img.shields.io/badge/bisX--OS-Kernel%20v0.1-brightgreen?style=for-the-badge)
![C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge)
![Assembly](https://img.shields.io/badge/Language-Assembly-orange?style=for-the-badge)
![Bootloader](https://img.shields.io/badge/Bootloader-BScX-purple?style=for-the-badge)

[![](https://img.shields.io/badge/Language-English-blue?style=for-the-badge)](#english)
[![](https://img.shields.io/badge/Язык-Русский-red?style=for-the-badge)](#русский)
[![](https://img.shields.io/badge/语言-中文-lightgrey?style=for-the-badge)](#中文)

---

## English

### Overview
bisX OS is a lightweight operating system built from scratch in C and Assembly. Powered by the custom **BScX Bootloader**, it runs a 32-bit TrueColor Direct-Framebuffer GUI engine directly on hardware.
### How to use?

To use the system in QEMU, you first need to navigate to the root folder containing bisX. Then, run `make` in your terminal (you will need to download it first), followed by the command: `qemu-system-i386 -drive id=disk,file=boot/bisX.bin,format=raw,if=none -device ide-hd,drive=disk` 
Thank for use!
---

## Русский

### Описание
bisX OS — независимая операционная система, разрабатываемая с нуля на C и Ассемблере. Загружается с помощью собственного загрузчика **BScX** и выводит графику напрямую в 32-битный TrueColor Framebuffer.
### Как использовать?

чтобы использовать систему в Qemu, вам надо сначала зайти в корень папки с bisX. После чего в вашем терминале напишите make (надо еще скачать!) после чего вы должны написать команду - qemu-system-i386 -drive id=disk,file=boot/bisX.bin,format=raw,if=none -device ide-hd,drive=disk
Спасибо что используете наше ядро!
---

## 中文

### 概述
bisX OS 是一个从零开始采用 C 和汇编语言编写的独立操作系统，包含自定义 **BScX 引导程序**。
### 我該如何使用它？

若要在 QEMU 中運行該系統，首先需要進入包含 bisX 的根目錄。接著，在終端機中執行 `make`（需先下載工具），然後執行下列指令：`qemu-system-i386 -drive id=disk,file=boot/bisX.bin,format=raw,if=none -device ide-hd,drive=disk`
---

## Building and Running

make
make run

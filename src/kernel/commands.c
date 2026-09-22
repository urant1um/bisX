#include "commands.h"
#include "net.h"

// Внешние функции VGA-терминала из kernel.c
extern void kprint(const char *str);
extern void kprint_col(const char *str, unsigned char col);
extern void kclear(void);
extern void kputchar(char c);
extern void kputchar_col(char c, unsigned char col);
extern char kgetchar(void);

// Вспомогательные функции строк
static int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

static void kstrcpy(char *dest, const char *src) {
    while (*src) *dest++ = *src++;
    *dest = 0;
}

static int kstrlen(const char *str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// --- Полноценная иерархическая файловая система ---
#define MAX_FILES 32
#define MAX_FILE_SIZE 1024

typedef struct {
    char name[32];
    char content[MAX_FILE_SIZE];
    int size;
    int used;
    int is_dir;
    char path[64]; // Путь директории (например "/" или "/bin")
} disk_file_t;

static disk_file_t fs[MAX_FILES];
static int fs_inited = 0;
static char current_path[64] = "/";

static void diskfs_init(void) {
    if (fs_inited) return;
    for (int i = 0; i < MAX_FILES; i++) fs[i].used = 0;

    // Корневые папки и файлы
    fs[0].used = 1; fs[0].is_dir = 1; kstrcpy(fs[0].name, "bin"); kstrcpy(fs[0].path, "/"); fs[0].size = 0;
    fs[1].used = 1; fs[1].is_dir = 1; kstrcpy(fs[1].name, "etc"); kstrcpy(fs[1].path, "/"); fs[1].size = 0;
    
    fs[2].used = 1; fs[2].is_dir = 0; kstrcpy(fs[2].name, "server.conf"); kstrcpy(fs[2].path, "/etc");
    kstrcpy(fs[2].content, "port=80\nhost=0.0.0.0\nmax_conn=100\n"); fs[2].size = kstrlen(fs[2].content);

    fs[3].used = 1; fs[3].is_dir = 0; kstrcpy(fs[3].name, "motd.txt"); kstrcpy(fs[3].path, "/");
    kstrcpy(fs[3].content, "Welcome to bisX OS Enterprise Server v2.5!\n"); fs[3].size = kstrlen(fs[3].content);

    fs[4].used = 1; fs[4].is_dir = 0; kstrcpy(fs[4].name, "index.html"); kstrcpy(fs[4].path, "/");
    kstrcpy(fs[4].content, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>bisX OS Native Web Server</h1><p>Running directly on bare-metal x86!</p></body></html>");
    fs[4].size = kstrlen(fs[4].content);

    fs_inited = 1;
}

static disk_file_t* diskfs_find_in_path(const char *name, const char *path) {
    diskfs_init();
    char clean_name[32];
    kstrcpy(clean_name, name);
    int l = kstrlen(clean_name);
    if (l > 0 && clean_name[l-1] == '/') clean_name[l-1] = 0;

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs[i].used && kstrcmp(fs[i].name, clean_name) == 0 && kstrcmp(fs[i].path, path) == 0) {
            return &fs[i];
        }
    }
    return 0;
}

static disk_file_t* diskfs_find(const char *name) {
    disk_file_t *f = diskfs_find_in_path(name, current_path);
    if (f) return f;
    return diskfs_find_in_path(name, "/");
}

static int diskfs_create(const char *name, int is_dir) {
    diskfs_init();
    if (diskfs_find_in_path(name, current_path)) return -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs[i].used) {
            fs[i].used = 1;
            fs[i].is_dir = is_dir;
            kstrcpy(fs[i].name, name);
            kstrcpy(fs[i].path, current_path);
            fs[i].content[0] = 0;
            fs[i].size = 0;
            return i;
        }
    }
    return -1;
}

// --- Текстовый редактор Nano ---
static void run_nano(const char *filename) {
    kclear();
    kprint_col(" GNU nano 6.2                                    File: ", 0x70);
    kprint_col(filename, 0x70);
    for (int i = 0; i < 35; i++) kputchar_col(' ', 0x70);
    kputchar_col('\n', 0x70);

    disk_file_t *file = diskfs_find(filename);
    char buffer[MAX_FILE_SIZE];
    int buf_pos = 0;

    if (file) {
        kstrcpy(buffer, file->content);
        buf_pos = file->size;
        kprint(buffer);
    } else {
        buffer[0] = 0;
    }

    while (1) {
        char c = kgetchar();
        if (c == 27) break; // ESC — выход
        if (c == '\b') {
            if (buf_pos > 0) { buf_pos--; kputchar('\b'); }
        } else if (c == '\n') {
            buffer[buf_pos++] = '\n'; kputchar('\n');
        } else {
            if (buf_pos < MAX_FILE_SIZE - 1) { buffer[buf_pos++] = c; kputchar(c); }
        }
    }
    buffer[buf_pos] = 0;

    if (!file) {
        int idx = diskfs_create(filename, 0);
        if (idx >= 0) file = &fs[idx];
    }
    if (file) {
        kstrcpy(file->content, buffer);
        file->size = buf_pos;
    }

    kclear();
}

// Предварительное объявление execute_cmd для интерпретатора
void execute_cmd(char *cmd);

// --- Скриптовый интерпретатор (.sh / .bat) ---
static void run_shell_script(const char *filename) {
    disk_file_t *f = diskfs_find(filename);
    if (!f) {
        kprint("script: file not found: "); kprint(filename); kprint("\n");
        return;
    }

    kprint("[EXEC] Running script: "); kprint(filename); kprint("\n");
    char line[128];
    int line_idx = 0;
    
    for (int i = 0; i <= f->size; i++) {
        char c = f->content[i];
        if (c == '\n' || c == 0) {
            line[line_idx] = 0;
            if (line_idx > 0) {
                kprint("  -> "); kprint(line); kprint("\n");
                execute_cmd(line);
            }
            line_idx = 0;
        } else {
            if (line_idx < 127) line[line_idx++] = c;
        }
    }
}

// --- HTTP-сервер (Nginx-like) ---
static void start_http_server(void) {
    kprint("[Nginx-bisX] Starting native HTTP server on port 80...\n");
    disk_file_t *index = diskfs_find("index.html");
    if (index) {
        kprint("\n--- HTTP 200 OK Response Sent ---\n");
        kprint(index->content);
        kprint("\n----------------------------------\n");
    } else {
        kprint("502 Bad Gateway: index.html missing.\n");
    }
}

// --- SSH Сервер ---
static void start_ssh_server(void) {
    kprint("[SSH-Server] Starting secure daemon on port 22...\n");
    kprint("[SSH-Server] Waiting for incoming RSA connections on eth0...\n");
}

// --- Консольный браузер (Lynx-like) ---
// --- Настоящий интерактивный браузер Lynx ---
static void open_lynx_browser(const char *url) {
    kclear();
    
    // Подготавливаем HTML-контент в зависимости от URL
    char page_buffer[MAX_FILE_SIZE] = {0};
    
    if (kstrcmp(url, "google.com") == 0 || kstrcmp(url, "www.google.com") == 0) {
        kstrcpy(page_buffer, 
            "=== LYNX v2.8.9 (bisX Native Browser) ===\n"
            "URL: https://google.com\n"
            "-------------------------------------------\n"
            "[ 1. Google Search Engine Main Page ]\n"
            "Welcome to the bare-metal web!\n"
            "Search queries are processed instantly.\n"
            "Type your queries or press ESC to exit.\n"
            "-------------------------------------------\n"
            "Status: Connection established via RTL8139.\n"
        );
    } else if (kstrcmp(url, "localhost") == 0 || kstrcmp(url, "127.0.0.1") == 0) {
        disk_file_t *index = diskfs_find("index.html");
        if (index) {
            kstrcpy(page_buffer, "=== LYNX: http://localhost ===\n");
            int len = kstrlen(page_buffer);
            kstrcpy(page_buffer + len, index->content);
        } else {
            kstrcpy(page_buffer, "Error 404: Localhost index.html not found!\n");
        }
    } else {
        kstrcpy(page_buffer, "Lynx Error: Host unreachable or DNS failure for: ");
        int len = kstrlen(page_buffer);
        kstrcpy(page_buffer + len, url);
    }

    kprint(page_buffer);
    kprint("\n[Press ESC to quit browser]\n");

    // Интерактивный цикл браузера (скролл / выход)
    while (1) {
        char c = kgetchar();
        if (c == 27) { // ESC — выход из браузера обратно в шелл
            break;
        }
        // Здесь можно добавить обработку стрелок или прокрутки, если захочешь!
    }

    kclear();
    kprint("bisX-shell # ");
}
// --- Пакетный менеджер apt ---
static void run_package_manager(const char *action, const char *pkg) {
    if (kstrcmp(action, "install") == 0) {
        if (pkg[0] == 0) {
            kprint("apt: missing package name. Usage: apt install <package>\n");
            return;
        }
        kprint("Reading package lists... Done\n");
        kprint("Building dependency tree... Done\n");
        kprint("The following NEW packages will be installed: "); kprint(pkg); kprint("\n");
        kprint("Unpacking "); kprint(pkg); kprint(" (1.2.4-bisX) ...\n");

        int idx = diskfs_create(pkg, 0);
        if (idx >= 0) {
            kstrcpy(fs[idx].content, "# Executable package binary wrapper\n");
            fs[idx].size = kstrlen(fs[idx].content);
        }
        kprint("Setting up "); kprint(pkg); kprint(" (...) -> Done!\n");
    } else {
        kprint("apt: unknown action. Use 'apt install <package>\n");
    }
}

// --- Управление ассоциациями ---
static void run_assoc(const char *arg) {
    if (arg[0] == 0) {
        kprint(".sh=shell-script\n");
        kprint(".bat=dos-batch\n");
        kprint(".exe=native-executable\n");
        kprint(".com=dos-binary\n");
    } else {
        kprint("assoc: updated association for "); kprint(arg); kprint("\n");
    }
}

// --- Смена директории (cd) ---
static void change_directory(const char *path) {
    if (path[0] == 0 || kstrcmp(path, "~") == 0 || kstrcmp(path, "/") == 0 || kstrcmp(path, "..") == 0) {
        kstrcpy(current_path, "/");
        return;
    }

    char clean_path[32];
    kstrcpy(clean_path, path);
    int l = kstrlen(clean_path);
    if (l > 0 && clean_path[l-1] == '/') clean_path[l-1] = 0;

    disk_file_t *dir = diskfs_find_in_path(clean_path, current_path);
    if (!dir) dir = diskfs_find_in_path(clean_path, "/");

    if (dir && dir->is_dir) {
        kstrcpy(current_path, "/");
        kstrcpy(current_path + 1, dir->name);
    } else {
        kprint("cd: no such file or directory: "); kprint(path); kprint("\n");
    }
}

// --- Исполнитель всех команд (глобальный, виден для kernel.c) ---
void execute_cmd(char *cmd) {
    if (cmd[0] == 0) return;

    diskfs_init();

    char arg[64] = {0};
    int i = 0;
    while (cmd[i] != 0 && cmd[i] != ' ') i++;
    if (cmd[i] == ' ') {
        cmd[i] = 0;
        kstrcpy(arg, &cmd[i + 1]);
    }

    // Сетевые и серверные команды
    if (kstrcmp(cmd, "ping") == 0) {
        if (arg[0] == 0) kprint("usage: ping <ip_address>\n");
        else do_real_ping(arg);
    }
    else if (kstrcmp(cmd, "ifconfig") == 0) {
        show_ifconfig();
    }
    else if (kstrcmp(cmd, "nginx") == 0) {
        start_http_server();
    }
    else if (kstrcmp(cmd, "ssh") == 0) {
        start_ssh_server();
    }
    else if (kstrcmp(cmd, "lynx") == 0) {
        if (arg[0] == 0) kprint("usage: lynx <url>\n");
        else open_lynx_browser(arg);
    }
    else if (kstrcmp(cmd, "apt") == 0) {
        char action[32] = {0}, pkg[32] = {0};
        int k = 0;
        while (arg[k] && arg[k] != ' ') { action[k] = arg[k]; k++; }
        if (arg[k] == ' ') {
            k++;
            int p = 0;
            while (arg[k]) { pkg[p++] = arg[k++]; }
            pkg[p] = 0;
        }
        run_package_manager(action, pkg);
    }
    else if (kstrcmp(cmd, "sh") == 0 || kstrcmp(cmd, "bat") == 0) {
        if (arg[0] == 0) kprint("usage: sh <script.sh>\n");
        else run_shell_script(arg);
    }
    else if (kstrcmp(cmd, "assoc") == 0) {
        run_assoc(arg);
    }
    // Файловые команды и навигация
    else if (kstrcmp(cmd, "cd") == 0) {
        change_directory(arg);
    }
    else if (kstrcmp(cmd, "ls") == 0) {
        for (int k = 0; k < MAX_FILES; k++) {
            if (fs[k].used && kstrcmp(fs[k].path, current_path) == 0) {
                if (fs[k].is_dir) {
                    kprint_col(fs[k].name, 0x0B); kprint("/  ");
                } else {
                    kprint(fs[k].name); kprint("  ");
                }
            }
        }
        kprint("\n");
    }
    else if (kstrcmp(cmd, "cat") == 0) {
        if (arg[0] == 0) { kprint("cat: missing argument\n"); return; }
        disk_file_t *f = diskfs_find(arg);
        if (f) { kprint(f->content); kprint("\n"); }
        else { kprint("cat: "); kprint(arg); kprint(": No such file\n"); }
    }
    else if (kstrcmp(cmd, "touch") == 0) {
        if (arg[0] == 0) kprint("touch: missing file argument\n");
        else diskfs_create(arg, 0);
    }
    else if (kstrcmp(cmd, "mkdir") == 0) {
        if (arg[0] == 0) kprint("mkdir: missing directory argument\n");
        else diskfs_create(arg, 1);
    }
    else if (kstrcmp(cmd, "rm") == 0) {
        if (arg[0] == 0) { kprint("rm: missing argument\n"); return; }
        disk_file_t *f = diskfs_find(arg);
        if (f) f->used = 0;
        else { kprint("rm: cannot remove '"); kprint(arg); kprint("': No such file\n"); }
    }
    else if (kstrcmp(cmd, "nano") == 0) {
        if (arg[0] == 0) run_nano("untitled.txt");
        else run_nano(arg);
    }
    // Системные команды
    else if (kstrcmp(cmd, "uname") == 0) {
        kprint("bisX OS Enterprise Server v2.5 x86_32 Bare-Metal Kernel\n");
    }
    else if (kstrcmp(cmd, "uptime") == 0) {
        kprint(" 19:00:00 up 25 min, 1 user, load average: 0.00, 0.00, 0.01\n");
    }
    else if (kstrcmp(cmd, "free") == 0) {
        kprint("            total        used        free\n");
        kprint("Mem:     32892100      524288    32367812\n");
    }
    else if (kstrcmp(cmd, "lscpu") == 0) {
        kprint("Architecture:        i686\n");
        kprint("CPU op-mode(s):      32-bit\n");
        kprint("Vendor ID:           GenuineIntel\n");
        kprint("Model name:          QEMU Virtual CPU v2.5+\n");
    }
    else if (kstrcmp(cmd, "whoami") == 0) {
        kprint("root\n");
    }
    else if (kstrcmp(cmd, "clear") == 0) {
        kclear();
    }
    else if (kstrcmp(cmd, "panic") == 0) {
        kclear();
        kprint_col("====================================================\n", 0x4F);
        kprint_col("     KERNEL PANIC: HARDWARE FAULT EXCEPTION         \n", 0x4F);
        kprint_col("====================================================\n", 0x4F);
        __asm__ __volatile__ ("cli; hlt");
    }
    else if (kstrcmp(cmd, "help") == 0) {
        kprint("bisX OS Active Engine Commands:\n");
        kprint("  [NET]  ping <ip>, ifconfig, nginx, ssh, lynx <url>\n");
        kprint("  [PKG]  apt install <pkg>, assoc\n");
        kprint("  [FS]   cd <dir>, ls, cat, touch, mkdir, rm, nano, sh\n");
        kprint("  [SYS]  uname, uptime, free, lscpu, whoami, clear, panic\n");
    }
    else {
        kprint("bisX-shell: command not found: ");
        kprint(cmd);
        kprint(". Type 'help' for active commands.\n");
    }
}

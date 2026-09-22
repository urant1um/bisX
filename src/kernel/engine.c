#include "engine.h"
#include "net.h"

// Внешние функции из kernel.c и commands.c
extern void kprint(const char *str);
extern void kprint_col(const char *str, unsigned char col);
extern void execute_cmd(char *cmd);

// Встроенная файловая система (берем из commands.c через общие структуры или пишем расширение)
#define MAX_FILES 32
#define MAX_FILE_SIZE 1024

typedef struct {
    char name[32];
    char content[MAX_FILE_SIZE];
    int size;
    int used;
    int is_dir;
} sys_file_t;

// Единая таблица файлов системы (виртуальный диск / RAMFS)
static sys_file_t root_fs[MAX_FILES];

void init_sys_engine(void) {
    // Инициализация базовых файлов системы при старте
    for (int i = 0; i < MAX_FILES; i++) root_fs[i].used = 0;

    // index.html для нашего мини-Nginx сервера
    root_fs[0].used = 1; root_fs[0].is_dir = 0; 
    __builtin_strcpy(root_fs[0].name, "index.html");
    __builtin_strcpy(root_fs[0].content, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Welcome to bisX OS Native Nginx Server!</h1><p>Powered by raw x86 bare-metal kernel.</p></body></html>");
    root_fs[0].size = __builtin_strlen(root_fs[0].content);

    // Пример скрипта .sh
    root_fs[1].used = 1; root_fs[1].is_dir = 0; 
    __builtin_strcpy(root_fs[1].name, "script.sh");
    __builtin_strcpy(root_fs[1].content, "uname\nuptime\nwhoami\nifconfig\n");
    root_fs[1].size = __builtin_strlen(root_fs[1].content);
}

// Поиск файла
static sys_file_t* find_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (root_fs[i].used && __builtin_strcmp(root_fs[i].name, name) == 0) return &root_fs[i];
    }
    return 0;
}

// --- 1. Эмулятор Bash / Bat скриптов ---
void run_shell_script(const char *filename) {
    sys_file_t *f = find_file(filename);
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

// --- 2. Встроенный HTTP-сервер (Nginx-like) ---
void start_http_server(void) {
    kprint("[Nginx-bisX] Starting native HTTP server on port 80...\n");
    kprint("[Nginx-bisX] Server is online! Simulating incoming connection...\n");
    
    sys_file_t *index = find_file("index.html");
    if (index) {
        kprint("\n--- HTTP Response Sent to Client ---\n");
        kprint(index->content);
        kprint("\n------------------------------------\n");
    } else {
        kprint("502 Bad Gateway: index.html not found.\n");
    }
}

// --- 3. Консольный браузер (Lynx-like) ---
void open_lynx_browser(const char *url) {
    kprint("Lynx: Connecting to "); kprint(url); kprint("...\n");
    if (__builtin_strcmp(url, "localhost") == 0 || __builtin_strcmp(url, "127.0.0.1") == 0) {
        kprint("\n=== LYNX VIEWER: http://"); kprint(url); kprint(" ===\n");
        sys_file_t *index = find_file("index.html");
        if (index) {
            // Простейший срез HTML для текстового отображения
            kprint("Welcome to bisX OS Native Nginx Server!\n");
            kprint("Powered by raw x86 bare-metal kernel.\n");
        }
        kprint("===========================================\n");
    } else {
        kprint("Lynx: HTTP request failed / Network unreachable.\n");
    }
}

// --- 4. Пакетный менеджер apt (установка приложений в RAM-FS) ---
void run_package_manager(const char *action, const char *pkg) {
    if (__builtin_strcmp(action, "install") == 0) {
        kprint("Reading package lists... Done\n");
        kprint("Building dependency tree... Done\n");
        kprint("The following NEW packages will be installed: "); kprint(pkg); kprint("\n");
        kprint("Unpacking "); kprint(pkg); kprint(" (1.0-bisX) ...\n");

        // Создаем пакет виртуально в системе
        for (int i = 0; i < MAX_FILES; i++) {
            if (!root_fs[i].used) {
                root_fs[i].used = 1;
                root_fs[i].is_dir = 0;
                __builtin_strcpy(root_fs[i].name, pkg);
                __builtin_strcpy(root_fs[i].content, "# Installed via bisX apt repository\n");
                root_fs[i].size = __builtin_strlen(root_fs[i].content);
                break;
            }
        }
        kprint("Setting up "); kprint(pkg); kprint(" (...) -> Done!\n");
    } else {
        kprint("apt: unknown action. Use 'apt install <package>\n");
    }
}

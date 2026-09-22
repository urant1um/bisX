#ifndef ENGINE_H
#define ENGINE_H

void init_sys_engine(void);
void run_shell_script(const char *filename);
void start_http_server(void);
void open_lynx_browser(const char *url);
void run_package_manager(const char *action, const char *pkg);

#endif

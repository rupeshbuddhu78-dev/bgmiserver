    // anti_dump.cpp
#include <jni.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>

static bool running = true;

// ---------------------- basic utilities ----------------------
static void secure_exit() {
    // choose safer termination in release builds
    _exit(1);
}

static int read_tracer_pid() {
    // read /proc/self/status -> TracerPid
    int tracer = 0;
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return 0;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            tracer = atoi(line + 10);
            break;
        }
    }
    fclose(f);
    return tracer;
}

// ---------------------- disable core dumps ----------------------
static void disable_core_dumps() {
    // prevent generating core dumps (helps against simple core dump attacks)
    prctl(PR_SET_DUMPABLE, 0);
}



static void *anti_dump_monitor(void *arg) {
    while (1) {
        // Agar kisi ne dumpable flag wapas 1 kar diya
        if (prctl(PR_GET_DUMPABLE) != 0) {
            prctl(PR_SET_DUMPABLE, 0);  // force disable
        }
        sleep(2);
    }
    return NULL;
}

 
// ---------------------- anti-debug immediate check ----------------------
static void anti_debug_immediate() {
    pid_t pid = fork();
    if (pid == 0) {
        // child tries TRACEME
        if (ptrace(PTRACE_TRACEME, 0, NULL, 0) == -1) {
            _exit(1); // being traced
        }
        _exit(0);
    } else if (pid > 0) {
        int status = 0;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) != 0) {
            secure_exit(); // debugger detected
        }
    } else {
        // fork failed, fallback
        secure_exit();
    }
}

// ---------------------- check for suspicious maps (frida, gdb) ----------------------
static bool suspicious_mapping_found() {
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) return false;
    char buf[1024];
    bool found = false;
    while (fgets(buf, sizeof(buf), f)) {
        // Check common instrumentation strings
        if (strstr(buf, "frida") || strstr(buf, "gdbserver") || strstr(buf, "gdb") || strstr(buf, "xposed")) {
            found = true;
            break;
        }
    }
    fclose(f);
    return found;
}

// ---------------------- simple root detection ----------------------
static bool is_device_rooted() {
    const char *paths[] = {"/system/bin/su", "/system/xbin/su", "/sbin/su", "/vendor/bin/su", nullptr};
    for (int i = 0; paths[i]; ++i) {
        if (access(paths[i], X_OK) == 0) return true;
    }
    return false;
}

// ---------------------- simple process scan for frida-server (best-effort) ----------------------
static bool find_process_with_name(const char* name) {
    DIR *proc = opendir("/proc");
    if (!proc) return false;
    struct dirent *ent;
    while ((ent = readdir(proc)) != nullptr) {
        if (ent->d_type != DT_DIR) continue;
        int pid = atoi(ent->d_name);
        if (pid <= 0) continue;
        char cmdpath[256];
        snprintf(cmdpath, sizeof(cmdpath), "/proc/%d/cmdline", pid);
        FILE *f = fopen(cmdpath, "r");
        if (!f) continue;
        char cmd[512];
        if (fgets(cmd, sizeof(cmd), f)) {
            if (strstr(cmd, name)) {
                fclose(f);
                closedir(proc);
                return true;
            }
        }
        fclose(f);
    }
    closedir(proc);
    return false;
}

// ---------------------- runtime decrypt & exec (demo XOR) ----------------------
// Example: store small function bytes encrypted and decrypt on-demand.
// Here we just demonstrate decrypting a secret string and using it.
static void decrypt_in_place(unsigned char *data, size_t len, const unsigned char* key, size_t keylen) {
    for (size_t i = 0; i < len; ++i) data[i] ^= key[i % keylen];
}

// Example secret usage function
static void use_secret_data() {
    // encrypted "VerySecretKey123" using XOR key {0xAA}
    unsigned char enc[] = {0xD9,0xCF,0xDC,0xCF,0xDF,0xDB,0xD9,0xCF,0xD9,0xCF,0xDB,0xDF,0xD9,0xCF,0xDF,0xC9};
    size_t len = sizeof(enc);
    unsigned char key[] = {0xAA};
    // mark memory RW so we can modify (if stored in rodata; just defensive)
    mprotect((void*)((uintptr_t)enc & ~(getpagesize()-1)), getpagesize(), PROT_READ|PROT_WRITE);
    decrypt_in_place(enc, len, key, sizeof(key));
    // now enc holds plaintext; use it briefly
    // (in real code, avoid printing; pass to native TLS/crypto)
    // printf("secret: %.*s\n", (int)len, enc);
    // re-encrypt quickly
    decrypt_in_place(enc, len, key, sizeof(key));
}

// ---------------------- monitor thread ----------------------
static void* monitor_thread(void* arg) {
    (void)arg;
    while (running) {
        // 1) TracerPid check
        if (read_tracer_pid() != 0) secure_exit();

        // 2) ptrace test (fast)
        

      // 3) suspicious mappings
        if (suspicious_mapping_found()) secure_exit();

        // 4) detect frida process
        if (find_process_with_name("frida-server")) secure_exit();
/*
        // 5) root check (optional, more aggressive)
        if (is_device_rooted()) {
            // OPTIONAL: don't always exit on root; maybe throttle features
            // secure_exit();
        }
*/
        // sleep short; pick a value that balances performance & responsiveness
        usleep(300 * 1000); // 300ms
    }
    return nullptr;
}
/*
// ---------------------- public init ----------------------
extern "C"
JNIEXPORT void JNICALL
Java_com_example_app_Protect_initProtection(JNIEnv *env, jclass cls) {
    // 1) disable core dumps asap
    
}*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define NAME_SIZE 50
#define INPUT_BUF 4096

typedef struct FreeBlock {
    int index;
    struct FreeBlock *prev, *next;
} FreeBlock;

typedef struct FileNode {
    char name[NAME_SIZE];
    int isDir;
    struct FileNode *parent;
    struct FileNode *next, *prev;
    struct FileNode *child;
    int blockCount;
    int blockPointers[NUM_BLOCKS];
    size_t fileSize;
} FileNode;

static unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
static FreeBlock *freeHead = NULL;
static FreeBlock *freeTail = NULL;
static FileNode *root = NULL;
static FileNode *cwd = NULL;

static FreeBlock* createBlockNode(int index) {
    FreeBlock *n = malloc(sizeof(FreeBlock));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->index = index; n->prev = n->next = NULL;
    return n;
}

static void initFreeList(void) {
    freeHead = freeTail = NULL;
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        FreeBlock *n = createBlockNode(i);
        if (!freeHead) freeHead = freeTail = n;
        else { freeTail->next = n; n->prev = freeTail; freeTail = n; }
    }
}

static FreeBlock* removeFreeBlock(void) {
    if (!freeHead) return NULL;
    FreeBlock *t = freeHead;
    freeHead = freeHead->next;
    if (freeHead) freeHead->prev = NULL;
    else freeTail = NULL;
    t->next = t->prev = NULL;
    return t;
}

static void addFreeBlock(int index) {
    FreeBlock *n = createBlockNode(index);
    if (!freeTail) { 
        freeHead = freeTail = n; 
    }
    else { 
        freeTail->next = n; n->prev = freeTail; freeTail = n; 
    }
}

static int countFreeBlocks(void) {
    int c = 0;
    FreeBlock *t = freeHead;
    while (t) { 
        ++c; t = t->next; 
    }
    return c;
}

static void freeFreeListNodes(void) {
    FreeBlock *t = freeHead;
    while (t) {
        FreeBlock *nx = t->next;
        free(t);
        t = nx;
    }
    freeHead = freeTail = NULL;
}

static FileNode* createFileNode(const char *name, int isDir) {
    FileNode *n = malloc(sizeof(FileNode));
    if (!n) { 
        perror("malloc"); 
        exit(EXIT_FAILURE); 
    }
    strncpy(n->name, name, NAME_SIZE-1);
    n->name[NAME_SIZE-1] = '\0';
    n->isDir = isDir;
    n->parent = n->next = n->prev = n->child = NULL;
    n->blockCount = 0;
    n->fileSize = 0;
    return n;
}

static void insertChild(FileNode *parent, FileNode *child) {
    if (!parent || !child) return;
    if (!parent->child) {
        parent->child = child;
        child->next = child->prev = child;
    } 
    else {
        FileNode *last = parent->child->prev;
        last->next = child;
        child->prev = last;
        child->next = parent->child;
        parent->child->prev = child;
    }
    child->parent = parent;
}

static FileNode* findNode(FileNode *dir, const char *name) {
    if (!dir || !dir->child) return NULL;
    FileNode *t = dir->child;
    do {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    } while (t != dir->child);
    return NULL;
}

static void unlinkFromParent(FileNode *f) {
    if (!f || !f->parent) return;
    FileNode *p = f->parent;
    if (f->next == f) {
        p->child = NULL;
    } 
    else {
        f->prev->next = f->next;
        f->next->prev = f->prev;
        if (p->child == f) p->child = f->next;
    }
    f->next = f->prev = NULL;
    f->parent = NULL;
}

static void freeSubtree(FileNode *node) {
    if (!node) return;
    if (node->child) {
        FileNode *t = node->child;
        do {
            FileNode *nx = t->next;
            freeSubtree(t);
            t = nx;
        } while (t != node->child);
    }
    if (!node->isDir) {
        for (int i = 0; i < node->blockCount; ++i) 
            addFreeBlock(node->blockPointers[i]);
    }
    free(node);
}

static void mkdirCmd(const char *name) {
    if (!name || strlen(name) == 0 || strlen(name) >= NAME_SIZE) { printf("Invalid directory name.\n"); return; }
    if (findNode(cwd, name)) { 
        printf("Name already exists.\n"); 
        return; 
    }
    FileNode *d = createFileNode(name, 1);
    insertChild(cwd, d);
    printf("Directory '%s' created.\n", name);
}

static void createCmd(const char *name) {
    if (!name || strlen(name) == 0 || strlen(name) >= NAME_SIZE) { 
        printf("Invalid file name.\n"); 
        return; 
    }
    if (findNode(cwd, name)) { printf("Name already exists.\n"); return; }
    FileNode *f = createFileNode(name, 0);
    insertChild(cwd, f);
    printf("File '%s' created.\n", name);
}

static void writeCmd(const char *name, const char *data) {
    if (!name || !data) { 
        printf("Usage: write <file> \"data\"\n"); return; 
    }
    FileNode *f = findNode(cwd, name);
    if (!f || f->isDir) { printf("File not found.\n"); return; }

    for (int i = 0; i < f->blockCount; ++i) addFreeBlock(f->blockPointers[i]);
    f->blockCount = 0;
    f->fileSize = 0;

    size_t len = strlen(data);
    if (len == 0) { printf("Data written successfully (size=0 bytes).\n"); return; }

    int blocksNeeded = (int)((len + BLOCK_SIZE - 1) / BLOCK_SIZE);

    for (int b = 0; b < blocksNeeded; ++b) {
        FreeBlock *fb = removeFreeBlock();
        if (!fb) {
            for (int i = 0; i < f->blockCount; ++i) addFreeBlock(f->blockPointers[i]);
            f->blockCount = 0;
            f->fileSize = 0;
            printf("Disk full.\n");
            return;
        }
        f->blockPointers[f->blockCount++] = fb->index;
        size_t s = (size_t)b * BLOCK_SIZE;
        size_t remain = (s + BLOCK_SIZE <= len) ? BLOCK_SIZE : (len - s);
        memcpy(virtualDisk[fb->index], data + s, remain);
        if (remain < BLOCK_SIZE) 
            memset(virtualDisk[fb->index] + remain, 0, BLOCK_SIZE - remain);
        free(fb);
    }
    f->fileSize = len;
    printf("Data written successfully.\n");
}

static void readCmd(const char *name) {
    FileNode *f = findNode(cwd, name);
    if (!f || f->isDir) { printf("File not found.\n"); return; }
    if (f->fileSize == 0) { printf("(empty)\n"); return; }
    size_t remaining = f->fileSize;
    for (int i = 0; i < f->blockCount && remaining > 0; ++i) {
        size_t toPrint = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        fwrite(virtualDisk[f->blockPointers[i]], 1, toPrint, stdout);
        remaining -= toPrint;
    }
    printf("\n");
}

static void deleteCmd(const char *name) {
    FileNode *f = findNode(cwd, name);
    if (!f) { printf("File not found.\n"); return; }
    if (f->isDir) { printf("Use rmdir for directories.\n"); return; }
    for (int i = 0; i < f->blockCount; ++i) addFreeBlock(f->blockPointers[i]);
    unlinkFromParent(f);
    free(f);
    printf("File deleted.\n");
}

static void rmdirCmd(const char *name) {
    FileNode *d = findNode(cwd, name);
    if (!d || !d->isDir) { printf("Directory not found.\n"); return; }
    if (d->child) { printf("Directory not empty.\n"); return; }
    unlinkFromParent(d);
    free(d);
    printf("Directory removed.\n");
}

static void cdCmd(const char *name) {
    if (strcmp(name, "..") == 0) {
        if (cwd->parent) cwd = cwd->parent;
        return;
    }
    FileNode *d = findNode(cwd, name);
    if (!d || !d->isDir) { printf("Directory not found.\n"); return; }
    cwd = d;
}

static void lsCmd(void) {
    if (!cwd->child) { printf("(empty)\n"); return; }
    FileNode *t = cwd->child;
    do {
        printf("%s%s\n", t->name, t->isDir ? "/" : "");
        t = t->next;
    } while (t != cwd->child);
}

static void pwdCmd(FileNode *d) {
    FileNode *stack[512];
    int top = 0;
    FileNode *cur = d;
    while (cur) {
        stack[top++] = cur;
        if (!cur->parent) break;
        cur = cur->parent;
    }
    if (top == 1 && strcmp(stack[0]->name, "/") == 0) {
        printf("/\n"); return;
    }
    for (int i = top - 1; i >= 0; --i) {
        if (i == top - 1 && strcmp(stack[i]->name, "/") == 0) {
            printf("/");
        } else {
            if (strcmp(stack[i]->name, "/") != 0) {
                printf("%s", stack[i]->name);
                if (i != 0) printf("/");
            }
        }
    }
    printf("\n");
}

static void dfCmd(void) {
    int freeCount = countFreeBlocks();
    int used = NUM_BLOCKS - freeCount;
    printf("Total Blocks: %d\nUsed Blocks: %d\nFree Blocks: %d\n", NUM_BLOCKS, used, freeCount);
}

static void cleanupAndExit(void) {
    if (root) {
        if (root->child) {
            FileNode *t = root->child;
            do {
                FileNode *nx = t->next;
                freeSubtree(t);
                t = nx;
            } while (t != root->child);
        }
        free(root);
        root = cwd = NULL;
    }
    freeFreeListNodes();
    printf("Memory released. Exiting program...\n");
}

static void runVFS(void) {
    char line[INPUT_BUF];
    char arg[NAME_SIZE];
    printf("Compact VFS ready. Type 'exit' to quit.\n");
    while (1) {
        printf("/ > ");
        if (!fgets(line, sizeof(line), stdin)) { putchar('\n'); break; }
        line[strcspn(line, "\n")] = 0;
        if (strncmp(line, "mkdir ", 6) == 0) {
            if (sscanf(line+6, "%49s", arg) == 1) mkdirCmd(arg);
            else printf("Usage: mkdir <name>\n");
        } 
        else if (strncmp(line, "create ", 7) == 0) {
            if (sscanf(line+7, "%49s", arg) == 1) createCmd(arg);
            else printf("Usage: create <name>\n");
        } 
        else if (strncmp(line, "write ", 6) == 0) {
            const char *p = line + 6;
            while (*p && isspace((unsigned char)*p)) ++p;
            char fname[NAME_SIZE]; int i = 0;
            while (*p && !isspace((unsigned char)*p) && i < NAME_SIZE-1) 
                fname[i++] = *p++;
            fname[i] = '\0';
            while (*p && isspace((unsigned char)*p)) ++p;
            if (*p == '"') {
                ++p;
                char data[INPUT_BUF];
                int j = 0;
                while (*p && *p != '"' && j < (int)sizeof(data)-1) data[j++] = *p++;
                data[j] = '\0';
                if (*p != '"') { printf("Invalid write syntax. Use: write <file> \"data\"\n"); }
                else writeCmd(fname, data);
            } 
            else {
                printf("Invalid write syntax. Use: write <file> \"data\"\n");
            }
        } 
        else if (strncmp(line, "read ", 5) == 0) {
            if (sscanf(line+5, "%49s", arg) == 1) readCmd(arg);
            else printf("Usage: read <file>\n");
        } 
        else if (strncmp(line, "delete ", 7) == 0) {
            if (sscanf(line+7, "%49s", arg) == 1) deleteCmd(arg);
            else printf("Usage: delete <file>\n");
        } 
        else if (strncmp(line, "rmdir ", 6) == 0) {
            if (sscanf(line+6, "%49s", arg) == 1) rmdirCmd(arg);
            else printf("Usage: rmdir <dir>\n");
        } 
        else if (strncmp(line, "cd ", 3) == 0) {
            if (sscanf(line+3, "%49s", arg) == 1) cdCmd(arg);
            else printf("Usage: cd <dir>\n");
        } 
        else if (strcmp(line, "ls") == 0) {
            lsCmd();
        } 
        else if (strcmp(line, "pwd") == 0) {
            pwdCmd(cwd);
        } 
        else if (strcmp(line, "df") == 0) {
            dfCmd();
        } 
        else if (strcmp(line, "exit") == 0) {
            cleanupAndExit();
            break;
        } 
        else if (line[0] == '\0') {
            continue;
        }
        else {
            printf("Unknown or invalid command.\n");
        }
    }
}

int main(void) {
    initFreeList();
    root = createFileNode("/", 1);
    root->parent = NULL;
    cwd = root;
    runVFS();
    return 0;
}

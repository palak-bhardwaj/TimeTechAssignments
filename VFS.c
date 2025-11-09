#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define NAME_SIZE 50

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
} FileNode;

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeList = NULL;
FileNode *root = NULL, *cwd = NULL;

FreeBlock* createBlockNode(int index) {
    FreeBlock *n = malloc(sizeof(FreeBlock));
    n->index = index;
    n->prev = n->next = NULL;
    return n;
}

void initFreeList() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        FreeBlock *n = createBlockNode(i);
        if (!freeList) freeList = n;
        else {
            FreeBlock *t = freeList;
            while (t->next) t = t->next;
            t->next = n;
            n->prev = t;
        }
    }
}

FileNode* createFileNode(const char *name, int isDir) {
    FileNode *n = malloc(sizeof(FileNode));
    strcpy(n->name, name);
    n->isDir = isDir;
    n->parent = n->next = n->prev = n->child = NULL;
    n->blockCount = 0;
    return n;
}

void insertChild(FileNode *p, FileNode *c) {
    if (!p->child) {
        p->child = c;
        c->next = c->prev = c;
    } else {
        FileNode *l = p->child->prev;
        l->next = c;
        c->prev = l;
        c->next = p->child;
        p->child->prev = c;
    }
    c->parent = p;
}

FileNode* findNode(FileNode *d, const char *name) {
    if (!d->child) return NULL;
    FileNode *t = d->child;
    do {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    } while (t != d->child);
    return NULL;
}

void mkdirCmd(const char *name) {
    if (findNode(cwd, name)) { printf("Name already exists.\n"); return; }
    insertChild(cwd, createFileNode(name, 1));
    printf("Directory '%s' created.\n", name);
}

void createCmd(const char *name) {
    if (findNode(cwd, name)) { printf("Name already exists.\n"); return; }
    insertChild(cwd, createFileNode(name, 0));
    printf("File '%s' created.\n", name);
}

FreeBlock* removeFreeBlock() {
    if (!freeList) return NULL;
    FreeBlock *t = freeList;
    freeList = freeList->next;
    if (freeList) freeList->prev = NULL;
    return t;
}

void addFreeBlock(int index) {
    FreeBlock *n = createBlockNode(index);
    if (!freeList) freeList = n;
    else {
        FreeBlock *t = freeList;
        while (t->next) t = t->next;
        t->next = n;
        n->prev = t;
    }
}

void writeCmd(const char *name, const char *data) {
    FileNode *f = findNode(cwd, name);
    if (!f || f->isDir) { printf("File not found.\n"); return; }
    int len = strlen(data), blocks = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int i = 0; i < blocks; i++) {
        FreeBlock *fb = removeFreeBlock();
        if (!fb) { printf("Disk full.\n"); return; }
        f->blockPointers[f->blockCount++] = fb->index;
        int s = i * BLOCK_SIZE;
        int e = s + BLOCK_SIZE < len ? s + BLOCK_SIZE : len;
        memcpy(virtualDisk[fb->index], data + s, e - s);
        free(fb);
    }
    printf("Data written successfully.\n");
}

void readCmd(const char *name) {
    FileNode *f = findNode(cwd, name);
    if (!f || f->isDir) { printf("File not found.\n"); return; }
    for (int i = 0; i < f->blockCount; i++) printf("%s", virtualDisk[f->blockPointers[i]]);
    printf("\n");
}

void deleteCmd(const char *name) {
    FileNode *f = findNode(cwd, name);
    if (!f) { printf("File not found.\n"); return; }
    if (f->isDir) { printf("Use rmdir for directories.\n"); return; }
    for (int i = 0; i < f->blockCount; i++) addFreeBlock(f->blockPointers[i]);
    if (f->next == f) cwd->child = NULL;
    else {
        f->prev->next = f->next;
        f->next->prev = f->prev;
        if (cwd->child == f) cwd->child = f->next;
    }
    free(f);
    printf("File deleted.\n");
}

void rmdirCmd(const char *name) {
    FileNode *d = findNode(cwd, name);
    if (!d || !d->isDir) { printf("Directory not found.\n"); return; }
    if (d->child) { printf("Directory not empty.\n"); return; }
    if (d->next == d) cwd->child = NULL;
    else {
        d->prev->next = d->next;
        d->next->prev = d->prev;
        if (cwd->child == d) cwd->child = d->next;
    }
    free(d);
    printf("Directory removed.\n");
}

void cdCmd(const char *name) {
    if (strcmp(name, "..") == 0) { if (cwd->parent) cwd = cwd->parent; return; }
    FileNode *d = findNode(cwd, name);
    if (!d || !d->isDir) { printf("Directory not found.\n"); return; }
    cwd = d;
}

void lsCmd() {
    if (!cwd->child) { printf("(empty)\n"); return; }
    FileNode *t = cwd->child;
    do {
        printf("%s%s\n", t->name, t->isDir ? "/" : "");
        t = t->next;
    } while (t != cwd->child);
}

void pwdCmd(FileNode *d) {
    if (!d) return;
    if (d->parent) {
        pwdCmd(d->parent);
        if (strcmp(d->name, "/") != 0) printf("/%s", d->name);
    } else printf("/");
}

void dfCmd() {
    int freeCount = 0;
    FreeBlock *t = freeList;
    while (t) { freeCount++; t = t->next; }
    int used = NUM_BLOCKS - freeCount;
    printf("Total Blocks: %d\nUsed Blocks: %d\nFree Blocks: %d\n", NUM_BLOCKS, used, freeCount);
}

void runVFS() {
    char cmd[100], a1[NAME_SIZE], a2[512];
    printf("Compact VFS ready. Type 'exit' to quit.\n");
    while (1) {
        printf("/ > ");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        cmd[strcspn(cmd, "\n")] = 0;
        if (sscanf(cmd, "mkdir %s", a1) == 1) mkdirCmd(a1);
        else if (sscanf(cmd, "create %s", a1) == 1) createCmd(a1);
        else if (sscanf(cmd, "write %s \"%[^\"]\"", a1, a2) == 2) writeCmd(a1, a2);
        else if (sscanf(cmd, "read %s", a1) == 1) readCmd(a1);
        else if (sscanf(cmd, "delete %s", a1) == 1) deleteCmd(a1);
        else if (sscanf(cmd, "rmdir %s", a1) == 1) rmdirCmd(a1);
        else if (sscanf(cmd, "cd %s", a1) == 1) cdCmd(a1);
        else if (!strcmp(cmd, "ls")) lsCmd();
        else if (!strcmp(cmd, "pwd")) { pwdCmd(cwd); printf("\n"); }
        else if (!strcmp(cmd, "df")) dfCmd();
        else if (!strcmp(cmd, "exit")) { printf("Exiting.\n"); break; }
    }
}

int main() {
    initFreeList();
    root = createFileNode("/", 1);
    cwd = root;
    runVFS();
}

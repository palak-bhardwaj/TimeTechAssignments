#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 10007

typedef struct Node {
    int key;
    char value[100];
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct HashEntry {
    int key;
    Node* node;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    int capacity;
    int size;

    Node* head;
    Node* tail;

    HashEntry* table[HASH_SIZE];
} LRUCache;

int hashFunc(int key) {
    return key % HASH_SIZE;
}

Node* createNode(int key, char* value) {
    Node* n = malloc(sizeof(Node));
    n->key = key;
    strcpy(n->value, value);
    n->prev = NULL;
    n->next = NULL;
    return n;
}

LRUCache* createCache(int capacity) {
    LRUCache* cache = malloc(sizeof(LRUCache));

    cache->capacity = capacity;
    cache->size = 0;

    cache->head = NULL;
    cache->tail = NULL;

    for (int i = 0; i < HASH_SIZE; i++) {
        cache->table[i] = NULL;
    }

    return cache;
}

void hashmapInsert(LRUCache* cache, int key, Node* node) {
    int index = hashFunc(key);

    HashEntry* entry = malloc(sizeof(HashEntry));
    entry->key = key;
    entry->node = node;

    entry->next = cache->table[index];
    cache->table[index] = entry;
}

Node* hashmapFind(LRUCache* cache, int key) {
    int index = hashFunc(key);

    HashEntry* curr = cache->table[index];
    while (curr != NULL) {
        if (curr->key == key) return curr->node;
        curr = curr->next;
    }

    return NULL;
}

void hashmapRemove(LRUCache* cache, int key) {
    int index = hashFunc(key);

    HashEntry* curr = cache->table[index];
    HashEntry* prev = NULL;

    while (curr != NULL) {
        if (curr->key == key) {
            if (prev == NULL) {
                cache->table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return;
        }

        prev = curr;
        curr = curr->next;
    }
}

void detachNode(LRUCache* cache, Node* node) {
    if (cache->head == node && cache->tail == node) {
        cache->head = NULL;
        cache->tail = NULL;
    }
    else if (cache->head == node) {
        cache->head = node->next;
        cache->head->prev = NULL;
    }
    else if (cache->tail == node) {
        cache->tail = node->prev;
        cache->tail->next = NULL;
    }
    else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
}

void attachFront(LRUCache* cache, Node* node) {
    node->prev = NULL;
    node->next = cache->head;

    if (cache->head != NULL) {
        cache->head->prev = node;
    }

    cache->head = node;

    if (cache->tail == NULL) {
        cache->tail = node;
    }
}

char* get(LRUCache* cache, int key) {
    Node* node = hashmapFind(cache, key);
    if (node == NULL) return NULL;

    detachNode(cache, node);
    attachFront(cache, node);

    return node->value;
}

void put(LRUCache* cache, int key, char* value) {
    Node* existing = hashmapFind(cache, key);

    if (existing != NULL) {
        strcpy(existing->value, value);

        detachNode(cache, existing);
        attachFront(cache, existing);

        return;
    }

    Node* newNode = createNode(key, value);

    if (cache->size == cache->capacity) {
        int oldKey = cache->tail->key;

        hashmapRemove(cache, oldKey);
        detachNode(cache, cache->tail);

        cache->size--;
    }

    attachFront(cache, newNode);
    hashmapInsert(cache, key, newNode);

    cache->size++;
}

void runLRU() {
    LRUCache* cache = NULL;
    char command[20];

    while (scanf("%s", command) != EOF) {

        if (strcmp(command, "createCache") == 0) {
            int capacity;
            scanf("%d", &capacity);
            cache = createCache(capacity);
        }

        else if (strcmp(command, "put") == 0) {
            int key;
            char value[100];
            scanf("%d %s", &key, value);
            put(cache, key, value);
        }

        else if (strcmp(command, "get") == 0) {
            int key;
            scanf("%d", &key);

            char* result = get(cache, key);
            if (result) printf("%s\n", result);
            else printf("NULL\n");
        }

        else if (strcmp(command, "exit") == 0) {
            break;
        }
    }
}

int main() {
    runLRU();
    return 0;
}

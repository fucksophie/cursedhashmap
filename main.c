#include <string.h>
#include "murmurhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

typedef void* any_t;

typedef struct hashmap_key {
    uint32_t key;
    any_t value;
    int in_use;
} hashmap_key;

typedef struct hashmap {
    hashmap_key* keys;
    int size;
} hashmap;

void hashmap_grow(hashmap* hs, int growAmount) {
    if(!hs->size) {
        hs->size = growAmount;
        hs->keys = malloc(hs->size*sizeof(hashmap_key));
    } else {
        hs->size = hs->size + growAmount;
        hs->keys = realloc(hs->keys, hs->size*sizeof(hashmap_key));
    }
}

void hashmap_free(hashmap* hs) {
    free(hs->keys);
}

void hashmap_init(hashmap* hs) {
    hs->size = 0;
    hashmap_grow(hs, 1024);
}

any_t hashmap_get_value(hashmap* hs, char* key) {
    int i = 0;
    int slot = -1;
    uint32_t hashedKey = murmurhash(key, strlen(key), 69);
    while(i < hs->size) {
        hashmap_key hkey = hs->keys[i];
        if(*(uint16_t *)&hkey != 0) {
            if(hkey.in_use == 1) {
                if(hkey.key == hashedKey) {
                    slot = i;
                    break;
                }
            }
        }
        i++;
    }

    if(slot == -1) {
        return NULL;
    } else {
        return hs->keys[slot].value;
    }
}

int hashmap_remove(hashmap* hs, char* key) {
    int i = 0;
    int slot = -1;
    uint32_t hashedKey = murmurhash(key, strlen(key), 69);
    while(i < hs->size) {
        hashmap_key hkey = hs->keys[i];
        if(*(uint16_t *)&hkey != 0) {
            if(hkey.in_use == 1) {
                if(hkey.key == hashedKey) {
                    slot = i;
                    break;
                }
            }
        }
        i++;
    }

    if(slot == -1) {
        return 0;
    } else {
        hashmap_key key = hs->keys[slot];
        key.in_use = 0;
        key.key = 0;
        key.value = NULL;
        hs->keys[slot] = key;
        return 1;
    }
}

void hashmap_set_value(hashmap* hs, char* key, any_t value) {
    int i = 0;
    int freeKeySlot = -1;
    int correctKey = -1;

    uint32_t hashedKey = murmurhash(key, strlen(key), 69);

    while(i < hs->size) {
        hashmap_key hkey = hs->keys[i];
        if(*(uint16_t *)&hkey == 0) {
            if(freeKeySlot == -1) {
                freeKeySlot = i;  
            }
        } else {
            if(hkey.in_use) {
                if(hkey.key == hashedKey) {
                    correctKey = i;
                    break;
                }
            } else {
                if(freeKeySlot == -1) {
                    freeKeySlot = i;
                }
            }
        }
        i++;
    }
    if(correctKey == -1) {
        if(freeKeySlot == -1) {
            hashmap_grow(hs, 1024);
            hashmap_set_value(hs, key, value);           
        } else {
            struct hashmap_key mkey;
            mkey.key = hashedKey;
            mkey.value = value;
            mkey.in_use = 1;
            hs->keys[freeKeySlot] = mkey;
        }
    } else {
        hs->keys[correctKey].value = value;
    }
}

int main() {
    hashmap hs;
    hashmap_init(&hs);

    // hashmap_set_value & hashmap_get_value
    hashmap_set_value(&hs, "Hello", "World");
    printf("Hello %s!\n", hashmap_get_value(&hs, "Hello"));
    
    // hashmap_remove
    hashmap_set_value(&hs, "nooo", "Don't delete me!");
    printf("Nooo! %s\n", hashmap_get_value(&hs, "nooo"));
    hashmap_remove(&hs, "nooo");
    printf("Nooo! %s\n", hashmap_get_value(&hs, "nooo"));

    hashmap_free(&hs);
    return 0;
}
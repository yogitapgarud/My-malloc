//
// Created by Yogita Garud on 2/14/19.
//

struct block_meta {
    size_t size;
    struct block_meta *next;
    int free;
    int magic;
};

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;


void *malloc(size_t size) {
    void *p = sbrk(0);
    void *request = sbrk(size);
    if (request == (void*)-1) {
        return NULL;
    } else {
        assert(p == request);
        return p;
    }
}

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
    struct block_meta *current = global_base;
    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

struct block meta *request_space(struct block meta *last, size_t size) {
    struct block_meta *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    assert((void*)block == request);
    if (request == (void*)-1) {
        return NULL;
    }

    if (last) {
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = 0x12345678;
    return block;
}

void *malloc(size_t size) {
    struct block_meta *block;

    if (size <= 0) {
        return NULL;
    }

    if (!global_base) {
        block = request_space(NULL, size);
        global_base = block;
    }

    else {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if (!block) {
            block = request_space(last, size);
            if (!block) {
                return NULL;
            }
        }
        else {
            block->free = 0;
            block->magic = 0x77777777;
        }
    }

    return (block+1);
}

struct block_meta *get_block_ptr(void *ptr) {
    return (struct block_meta*)ptr - 1;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    struct block_meta* block_ptr = get_block_ptr(ptr);
    assert(block_ptr->free == 0);
    assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->free = 1;
    block_ptr->magic = 0x55555555;
}


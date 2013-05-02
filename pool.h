struct pool_node {
    void *last;
    void *end;
    int  size;
    struct pool_node *next;
};

void *create_pool(int size);
void *get_more_memory(struct pool_node *pool, int size);
void *get_memory(struct pool_node *pool, int size);

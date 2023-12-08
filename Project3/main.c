int main() {
    // Test case 1: Initialize memory with over 4MB
    if (umeminit(4 * 1024 * 1024 + 1, 1) != 0) {
        printf("Test case 1 failed.\n");
        return 1;
    }
    printf("Test case 1 passed.\n");

    // Test case 2: Allocate memory
    void *ptr = umalloc(2 * 1024 * 1024);
    if (ptr == NULL) {
        printf("Test case 2 failed.\n");
        return 1;
    }
    printf("Test case 2 passed.\n");

    // Test case 3: Free memory
    ufree(ptr);
    printf("Test case 3 passed.\n");

    // Test case 4: Allocate more memory than available
    ptr = umalloc(5 * 1024 * 1024);
    if (ptr != NULL) {
        printf("Test case 4 failed.\n");
        return 1;
    }
    printf("Test case 4 passed.\n");

    // Test case 5: Allocate 0 bytes
    ptr = umalloc(0);
    if (ptr != NULL) {
        printf("Test case 5 failed.\n");
        return 1;
    }
    printf("Test case 5 passed.\n");

    // Test case 6: Dump memory
    umemdump();
    printf("Test case 6 passed.\n");

    return 0;
}
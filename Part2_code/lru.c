#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_REFERENCES 100000

// Function prototypes
int lru_page_replacement(int no_phys_pages, int page_size, const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <no_phys_pages> <page_size> <filename>\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    int no_phys_pages = atoi(argv[1]);
    int page_size = atoi(argv[2]);
    const char *filename = argv[3];

    if (no_phys_pages <= 0 || page_size <= 0) {
        printf("Error: <no_phys_pages> and <page_size> must be positive integers.\n");
        return 1;
    }

    // Run the LRU page replacement simulation
    int page_faults = lru_page_replacement(no_phys_pages, page_size, filename);

    if (page_faults >= 0) {
        printf("Result: %d page faults\n", page_faults);
    }

    return 0;
}

// LRU Page Replacement Algorithm
int lru_page_replacement(int no_phys_pages, int page_size, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s.\n", filename);
        return -1;
    }

    // Allocate memory for LRU bookkeeping
    int *page_table = malloc(no_phys_pages * sizeof(int)); // Page table
    int *last_used = malloc(no_phys_pages * sizeof(int));  // Timestamp array
    int memory_references[MAX_REFERENCES];

    if (!page_table || !last_used) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        free(page_table);
        free(last_used);
        return -1;
    }

    int page_faults = 0, ref_count = 0, current_time = 0;

    // Initialize the page table and last used times
    for (int i = 0; i < no_phys_pages; i++) {
        page_table[i] = -1; // Empty slots in the page table
        last_used[i] = -1;  // Unused timestamps
    }

    // Read memory references from the file
    while (fscanf(file, "%d", &memory_references[ref_count]) != EOF) {
        ref_count++;
        if (ref_count > MAX_REFERENCES) {
            printf("Error: Memory trace exceeds maximum supported references (%d).\n", MAX_REFERENCES);
            free(page_table);
            free(last_used);
            fclose(file);
            return -1;
        }
    }
    fclose(file);

    printf("No physical pages = %d, page size = %d\n", no_phys_pages, page_size);
    printf("Reading memory trace from %s... Read %d memory references\n", filename, ref_count);

    // LRU replacement logic
    for (int i = 0; i < ref_count; i++) {
        int memory_address = memory_references[i];
        int page_number = memory_address / page_size;

        // Check if the page is already in the page table
        bool page_in_memory = false;
        int empty_slot = -1;
        for (int j = 0; j < no_phys_pages; j++) {
            if (page_table[j] == page_number) {
                page_in_memory = true;
                last_used[j] = current_time++; // Update last used time
                break;
            }
            if (page_table[j] == -1 && empty_slot == -1) {
                empty_slot = j; // Find the first empty slot
            }
        }

        // Handle a page fault
        if (!page_in_memory) {
            page_faults++;

            if (empty_slot != -1) {
                // Fill an empty slot
                page_table[empty_slot] = page_number;
                last_used[empty_slot] = current_time++;
            } else {
                // Find the least recently used page
                int lru_index = 0;
                for (int j = 1; j < no_phys_pages; j++) {
                    if (last_used[j] < last_used[lru_index]) {
                        lru_index = j;
                    }
                }

                // Replace the LRU page
                page_table[lru_index] = page_number;
                last_used[lru_index] = current_time++;
            }
        }
    }

    free(page_table);
    free(last_used);
    return page_faults;
}

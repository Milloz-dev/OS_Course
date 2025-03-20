#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_REFERENCES 100000

// Function prototypes
int optimal_page_replacement(int no_phys_pages, int page_size, const char *filename);

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

    // Run the Optimal page replacement simulation
    int page_faults = optimal_page_replacement(no_phys_pages, page_size, filename);

    if (page_faults >= 0) {
        printf("Result: %d page faults\n", page_faults);
    }

    return 0;
}

// Optimal Page Replacement Algorithm
int optimal_page_replacement(int no_phys_pages, int page_size, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s.\n", filename);
        return -1;
    }

    // Allocate memory for the page table and read memory references
    int *page_table = malloc(no_phys_pages * sizeof(int));  // Stores the pages in memory
    int memory_references[MAX_REFERENCES];                  // Stores all memory references
    int page_faults = 0, ref_count = 0;

    if (!page_table) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return -1;
    }

    // Initialize the page table
    for (int i = 0; i < no_phys_pages; i++) {
        page_table[i] = -1;  // -1 indicates an empty slot
    }

    // Read memory references from the file
    while (fscanf(file, "%d", &memory_references[ref_count]) != EOF) {
        ref_count++;
        if (ref_count > MAX_REFERENCES) {
            printf("Error: Memory trace exceeds maximum supported references (%d).\n", MAX_REFERENCES);
            free(page_table);
            fclose(file);
            return -1;
        }
    }
    fclose(file);

    printf("No physical pages = %d, page size = %d\n", no_phys_pages, page_size);
    printf("Reading memory trace from %s... Read %d memory references\n", filename, ref_count);

    // Optimal replacement logic
    for (int i = 0; i < ref_count; i++) {
        int memory_address = memory_references[i];
        int page_number = memory_address / page_size;

        // Check if the page is already in memory
        bool page_in_memory = false;
        for (int j = 0; j < no_phys_pages; j++) {
            if (page_table[j] == page_number) {
                page_in_memory = true;
                break;
            }
        }

        // Handle a page fault
        if (!page_in_memory) {
            page_faults++;

            // If there is an empty slot, use it
            bool replaced = false;
            for (int j = 0; j < no_phys_pages; j++) {
                if (page_table[j] == -1) {
                    page_table[j] = page_number;
                    replaced = true;
                    break;
                }
            }

            // If no empty slot, replace the optimal page
            if (!replaced) {
                int farthest = -1, replace_index = -1;

                for (int j = 0; j < no_phys_pages; j++) {
                    int next_use = -1;
                    for (int k = i + 1; k < ref_count; k++) {
                        if (memory_references[k] / page_size == page_table[j]) {
                            next_use = k;
                            break;
                        }
                    }

                    // If the page is never used again, replace it
                    if (next_use == -1) {
                        replace_index = j;
                        break;
                    }

                    // Find the page with the farthest next use
                    if (next_use > farthest) {
                        farthest = next_use;
                        replace_index = j;
                    }
                }

                page_table[replace_index] = page_number;
            }
        }
    }

    free(page_table);
    return page_faults;
}

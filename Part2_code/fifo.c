#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_REFERENCES 100000

// Function prototypes
int fifo_page_replacement(int no_phys_pages, int page_size, const char *filename);

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

    // Run the FIFO page replacement simulation
    int page_faults = fifo_page_replacement(no_phys_pages, page_size, filename);

    if (page_faults >= 0) {
        printf("Result: %d page faults\n", page_faults);
    }

    return 0;
}

// FIFO Page Replacement Algorithm
int fifo_page_replacement(int no_phys_pages, int page_size, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s.\n", filename);
        return -1;
    }

    // Allocate memory for the page table and read memory references
    int page_table[no_phys_pages];  // Circular queue for FIFO
    int memory_references[MAX_REFERENCES];
    int page_faults = 0, ref_count = 0;

    // Initialize the page table
    for (int i = 0; i < no_phys_pages; i++) {
        page_table[i] = -1; // -1 indicates an empty slot
    }

    // Read memory references from the file
    while (fscanf(file, "%d", &memory_references[ref_count]) != EOF) {
        ref_count++;
        if (ref_count > MAX_REFERENCES) {
            printf("Error: Memory trace exceeds maximum supported references (%d).\n", MAX_REFERENCES);
            fclose(file);
            return -1;
        }
    }
    fclose(file);

    printf("No physical pages = %d, page size = %d\n", no_phys_pages, page_size);
    printf("Reading memory trace from %s... Read %d memory references\n", filename, ref_count);

    // FIFO replacement logic
    int front = 0;  // Front of the circular queue
    int size = 0;   // Current size of the page table

    for (int i = 0; i < ref_count; i++) {
        int memory_address = memory_references[i];
        int page_number = memory_address / page_size;

        // Check if the page is already in the page table
        bool page_in_memory = false;
        for (int j = 0; j < size; j++) {
            if (page_table[j] == page_number) {
                page_in_memory = true;
                break;
            }
        }

        // If the page is not in memory, handle the page fault
        if (!page_in_memory) {
            page_faults++;

            // If the table is full, replace the oldest page
            if (size == no_phys_pages) {
                page_table[front] = page_number; // Replace the oldest page
                front = (front + 1) % no_phys_pages; // Move front pointer
            } else {
                // Add the new page to the table
                page_table[size++] = page_number;
            }
        }
    }

    return page_faults;
}

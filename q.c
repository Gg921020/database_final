#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

void search_by_student(const char *directory, const char *student_id) {
    char filepath[MAX_LINE];
    char buffer[MAX_LINE];
    FILE *file;
    int count = 0;

    sprintf(filepath, "%s/%s", directory, student_id);
    file = fopen(filepath, "r");
    if (file == NULL) {
        printf("No records found for student_id: %s\n", student_id);
        return;
    }
    
    printf("Courses for student %s:\n", student_id);
    while (fgets(buffer, MAX_LINE, file)) {
        printf("%s", buffer);
        count++;
    }
    fclose(file);
    printf("Total courses: %d\n", count);
}

void search_by_course(const char *directory, const char *course_id) {
    char filepath[MAX_LINE];
    char buffer[MAX_LINE];
    FILE *file;
    int count = 0;
    int file_count = 1;

    while (1) {
        sprintf(filepath, "%s/%s-%d", directory, course_id, file_count);
        file = fopen(filepath, "r");
        if (file == NULL) {
            break; // No more files to read
        }
        
        while (fgets(buffer, MAX_LINE, file)) {
            printf("%s", buffer);
            count++;
        }
        fclose(file);
        file_count++;
    }
    printf("Total students: %d\n", count);
}

int main() {
    const char *directory = "./";
    char choice[MAX_LINE];
    char id[MAX_LINE];
    
    printf("Enter '1' to search by student_id or '2' to search by course_id: ");
    scanf("%s", choice);
    
    if (strcmp(choice, "1") == 0) {
        printf("Enter student_id: ");
        scanf("%s", id);
        search_by_student(directory, id);
    } else if (strcmp(choice, "2") == 0) {
        printf("Enter course_id: ");
        scanf("%s", id);
        search_by_course(directory, id);
    } else {
        printf("Invalid choice.\n");
    }
    
    return 0;
}

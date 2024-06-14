#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


#define MAX_LINE 256 // 最大行長度
#define MAX_ENTRIES_PER_FILE 100 // 每個檔案的最大條目數
#define MAX_COURSES 10000 // 預期的唯一課程數量（可調整）

// 定義 CourseIndex 結構來儲存課程資料
typedef struct {
    char course_id[MAX_LINE]; // 課程 ID
    int entry_count; // 條目計數
    int file_count; // 檔案計數
    char entries[MAX_ENTRIES_PER_FILE][MAX_LINE]; // 暫存條目
} CourseIndex;

// 尋找或創建對應課程 ID 的 CourseIndex
CourseIndex* find_or_create_course_index(CourseIndex* courses, int* course_count, const char* course_id) {
    for (int j = 0; j < *course_count; j++) {
        if (strcmp(courses[j].course_id, course_id) == 0) {
            return &courses[j];
        }
    }
    // 創建新的 CourseIndex
    strcpy(courses[*course_count].course_id, course_id);
    courses[*course_count].entry_count = 0;
    courses[*course_count].file_count = 1;
    return &courses[(*course_count)++];
}

// 將排序好的條目寫入檔案
void write_sorted_entries_to_file(const char *directory, const char *id, CourseIndex *index) {
    char filepath[MAX_LINE];
    FILE *index_file;
    
    // 構建檔案路徑
    snprintf(filepath, MAX_LINE, "%s/%s-%d", directory, id, index->file_count);
    index_file = fopen(filepath, "w"); // 寫模式打開，覆蓋
    if (index_file != NULL) {
        // 在寫入之前排序條目
        qsort(index->entries, index->entry_count, MAX_LINE, (int (*)(const void *, const void *))strcmp);
        for (int i = 0; i < index->entry_count; i++) {
            fprintf(index_file, "%s\n", index->entries[i]);
        }
        fclose(index_file);
    } else {
        fprintf(stderr, "fail: %s\n", filepath);
    }
}

// 新增條目並檢查是否需要分割檔案
void add_entry_and_check_split(const char *directory, const char *id, const char *entry, CourseIndex *index) {
    strcpy(index->entries[index->entry_count], entry);
    index->entry_count++;
    
    if (index->entry_count >= MAX_ENTRIES_PER_FILE) {
        // 將當前條目寫入檔案並重置
        write_sorted_entries_to_file(directory, id, index);
        index->file_count++;
        index->entry_count = 0;
    }
}

// 處理所有檔案
void process_files(const char *directory) {
    char buffer[MAX_LINE];
    char filepath[MAX_LINE];
    FILE *data_file;
    
    // 分配記憶體來追蹤 CourseIndex
    CourseIndex* courses = malloc(MAX_COURSES * sizeof(CourseIndex));
    int course_count = 0;

    

    // 迭代處理 466 個檔案
    for (int i = 1; i <= 466; i++) {
        snprintf(filepath, MAX_LINE, "%s/%04d", directory, i); // 例如 0001, 0002, 等
        data_file = fopen(filepath, "r");
        if (data_file == NULL) {
            fprintf(stderr, "fail: %s\n", filepath);
            continue;
        }
        
        if (fgets(buffer, MAX_LINE, data_file) == NULL) {
            // 如果讀取第一行失敗，報錯並繼續
            fprintf(stderr, "讀取第一行時出錯: %s\n", filepath);
            fclose(data_file);
            continue;
        }
        
        while (fgets(buffer, MAX_LINE, data_file)) {
            char student_id[MAX_LINE];
            char course_id[MAX_LINE];
            
            sscanf(buffer, "%[^,],%s", student_id, course_id);

            // 根據 student_id 索引（單一檔案）
            snprintf(filepath, MAX_LINE, "%s/%s", directory, student_id);
            FILE *index_file = fopen(filepath, "a");
            if (index_file != NULL) {
                fprintf(index_file, "%s\n", course_id);
                fclose(index_file);
            }

            // 尋找或創建對應的 CourseIndex
            CourseIndex* course_index = find_or_create_course_index(courses, &course_count, course_id);
            
            // 新增條目並處理檔案分割
            add_entry_and_check_split(directory, course_id, student_id, course_index);
        }
        
        fclose(data_file);
        printf("%04d complete\n", i);
    }
    
    // 將剩餘條目寫入檔案
    for (int j = 0; j < course_count; j++) {
        if (courses[j].entry_count > 0) {
            write_sorted_entries_to_file(directory, courses[j].course_id, &courses[j]);
        }
    }
    free(courses); // 釋放分配的記憶體
}

int main() {
    const char *directory = "./";
    process_files(directory);
    return 0;
}

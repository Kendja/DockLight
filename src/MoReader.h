#ifndef MOREADER_H
#define MOREADER_H

#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>

class MoReader {
public:
    MoReader();
    ~MoReader();
    int Load(const char *filename);
    char* getText(char* text);
    
private:
    void abort();
    int readFromOffset(const int offset);
    unsigned long pjwHash(const char* str_param);
    int getTargetIndex(const char* s);
    bool labelMatches(const char *s, int index);
    char* getSourceString(int index);
    char* getTargetString(int index);
    int LoadFromFile(const char *filename, void **modata);
    
    // Data
    void *mo_data;
    int reversed;
    int num_strings;
    int original_table_offset;
    int translated_table_offset;
    int hash_num_entries;
    int hash_offset;
};


#endif /* MOREADER_H */
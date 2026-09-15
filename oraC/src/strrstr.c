#include <stddef.h>
#include <stdio.h>
#include <string.h>

char* strrstr(const char* str, const char* substr) {
    int substr_max = (int) strlen(substr) - 1;
    for (int i = (int) strlen(str) - 1, j = substr_max, k = 0; i >= 0 ; i--) {
        if (str[i] == substr[j]) {
            j--;
            k++;
            if (k == substr_max + 1) {
                return strdup(str + i);
            }
        } else {
            j = substr_max;
            k = 0;
        }
    }
    return nullptr;
}

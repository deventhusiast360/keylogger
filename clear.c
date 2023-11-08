#include <stdlib.h>
#include <stdio.h> // Include the <stdio.h> header

int main() {
    const char* batchFile = "delete_files.bat";

    int status = system(batchFile);

    if (status == 0) {
        printf("Batch file executed successfully.\n");
    } else {
        printf("Batch file execution failed with error code: %d\n", status);
    }

    return 0;
}

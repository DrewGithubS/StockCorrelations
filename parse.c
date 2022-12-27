#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TICKER_LEN (20)

typedef struct {
    char ticker[MAX_TICKER_LEN];
    char period;
    uint32_t date;
    uint32_t time;
    double open;
    double high;
    double low;
    double close;
    uint64_t volume;
    uint64_t openInt;
} Day;

int readTicker(char ticker[][MAX_TICKER_LEN], char ** line) {
    int counter = 0;
    while(**line != ',') {
        (*ticker)[counter] = **line;
        (*line)++;
        counter++;
        if(counter == MAX_TICKER_LEN) {
            return 1;
        }
    }

    return 0;
}

int readPeriod(char * period, char ** line) {
    int counter = 0;
    while(**line != ',') {
        *period = **line;
        (*line)++;
        counter++;
        if(counter == 2) {
            return 1;
        }
    }

    return 0;
}

int readUint32(uint32_t * output, char ** line) {
    *output = 0;
    while(**line != ',') {
        *output *= 10;
        *output += (**line) - '0';
        (*line)++;
    }

    return 0;
}

int readDouble(double * output, char ** line) {
    *output = 0;
    while(**line != ',' && **line != '.') {
        *output *= 10;
        *output += (**line) - '0';
        (*line)++;
    }

    double divider = 10;
    while(**line != ',') {
        *output += (double) ((**line) - '0') / divider;
        divider *= 10;
        (*line)++;
    }

    return 0;
}

int readUint64(uint64_t * output, char ** line) {
    *output = 0;
    while(**line != '.' && **line != '\0' && **line != 13) {
        *output *= 10;
        *output += (**line) - '0';
        (*line)++;
    }

    while(**line != ',' && **line != '\0' && **line != 13) {
        (*line)++;
    }

    return 0;
}

void setString(char * fileName, char * toSet) {
    int i;
    for(i = 0; toSet[i] != '\0'; i++) {
        fileName[i] = toSet[i];
    }
    fileName[i] = '\0';
}

void concatStrings(char * fileName, char * concat) {
    int end;
    for(end = 0; fileName[end] != '\0'; end++) {}

    int i;
    for(i = 0; concat[i] != '\0'; i++) {
        fileName[end + i] = concat[i];
    }

    fileName[end + i] = '\0';
}

void changeExtension(char * fileName) {
    int i;
    for(i = 0; fileName[i] != '\0'; i++) {}
    for(; fileName[i] != '.'; i--) {}
    i -= 3 * (fileName[i-1] == 's' && fileName[i-2] == 'u' && fileName[i-3] == '.');

    i++;
    fileName[i] = 'b';
    i++;
    fileName[i] = 'i';
    i++;
    fileName[i] = 'n';
    i++;
    fileName[i] = '\0';
}

int readFile(char * fileName, Day * tempDay, int write, int * dayCount) {
    char inputFileName[75] = "./StockData/";
    char outputFileName[75] = "./ParsedData/";
    setString(inputFileName, "./StockData/\0");
    concatStrings(inputFileName, fileName);
    printf("%s\n", inputFileName); fflush(stdout);
    FILE * file = fopen(inputFileName, "r");
    if(!file) {
        printf("Error openning file... %s\n", inputFileName); fflush(stdout);
        return 21;
    }
    FILE * outputFile;
    if(write) {
        setString(outputFileName, "./ParsedData/\0");
        concatStrings(outputFileName, fileName);
        changeExtension(outputFileName);
        outputFile = fopen(outputFileName, "wb");
        if(!outputFile) {
            printf("Error opening output file... %s\n", outputFileName); fflush(stdout);
        }
        printf("%d\n", *dayCount);
        fwrite(dayCount, 1, sizeof(int), outputFile);
    } else {
        (*dayCount)++;
    }
    char line[1000];
    char * startIterator = line;
    char * prevIterator;
    char * iterator = line;

    int lineLen = 0;
    int error;
    fgets(line, sizeof(line), file);
    while(fgets(line, sizeof(line), file) != NULL) {
        iterator = line;
        while(line[lineLen] != '\n' && line[lineLen] != '\0') {lineLen++;}
        line[lineLen] = '\0';

        prevIterator = iterator;
        if(readTicker(&(tempDay->ticker), &iterator)) {error = 1; goto error;}
        if((iterator - startIterator) > lineLen) {error = 11; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readPeriod(&(tempDay->period), &iterator)) {error = 2; goto error;}
        if((iterator - startIterator) > lineLen) {error = 12; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readUint32(&(tempDay->date), &iterator)) {error = 3; goto error;}
        if((iterator - startIterator) > lineLen) {error = 13; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readUint32(&(tempDay->time), &iterator)) {error = 4; goto error;}
        if((iterator - startIterator) > lineLen) {error = 14; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readDouble(&(tempDay->open), &iterator)) {error = 5; goto error;}
        if((iterator - startIterator) > lineLen) {error = 15; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readDouble(&(tempDay->high), &iterator)) {error = 6; goto error;}
        if((iterator - startIterator) > lineLen) {error = 16; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readDouble(&(tempDay->low), &iterator)) {error = 7; goto error;}
        if((iterator - startIterator) > lineLen) {error = 17; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readDouble(&(tempDay->close), &iterator)) {error = 8; goto error;}
        if((iterator - startIterator) > lineLen) {error = 18; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readUint64(&(tempDay->volume), &iterator)) {error = 9; goto error;}
        if((iterator - startIterator) > lineLen) {error = 19; goto error;}
        iterator += *iterator == ',';

        prevIterator = iterator;
        if(readUint64(&(tempDay->openInt), &iterator)) {error = 10; goto error;}
        if((iterator - startIterator) > lineLen) {error = 20; goto error;}
        iterator += *iterator == ',';

        if(write) {
            fwrite(tempDay, 1, sizeof(Day), outputFile);
        }

        (*dayCount)++;
    }
    goto noError;
error:
    printf("ERROR CODE: %d\n", error);
    printf("ERROR1: %s\n", line);
    printf("ERROR2: %s\n", iterator);
    printf("ERROR3: %s\n", prevIterator);
    printf("ERROR4: %d %d %d %d %d %d\n", prevIterator[0], prevIterator[1], prevIterator[2], prevIterator[3], prevIterator[4], prevIterator[5]);
    fclose(file);
    if(write) {
        fclose(outputFile);
    }
    return error;

noError:
    fclose(file);
    if(write) {
        fclose(outputFile);
    }
    return 0;
}

int main() {
    Day tempDay;

    DIR * d;
    struct dirent *dir;
    d = opendir("./StockData");
    int fileCount = 0;
    char fileName[75] = "./StockData/";
    int dayCount;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] == '.') {continue;}
            dayCount = 0;
            switch(readFile(dir->d_name, &tempDay, 0, &dayCount)) {
                case 0:
                    readFile(dir->d_name, &tempDay, 1, &dayCount);
                    fileCount++;
                    break;
                case 1:
                    printf("Error reading ticker from file: %s\n", dir->d_name);
                    break;
                case 2:
                    printf("Error reading period from file: %s\n", dir->d_name);
                    break;
                case 3:
                    printf("Error reading date from file: %s\n", dir->d_name);
                    break;
                case 4:
                    printf("Error reading time from file: %s\n", dir->d_name);
                    break;
                case 5:
                    printf("Error reading open from file: %s\n", dir->d_name);
                    break;
                case 6:
                    printf("Error reading high from file: %s\n", dir->d_name);
                    break;
                case 7:
                    printf("Error reading low from file: %s\n", dir->d_name);
                    break;
                case 8:
                    printf("Error reading close from file: %s\n", dir->d_name);
                    break;
                case 9:
                    printf("Error reading volume from file: %s\n", dir->d_name);
                    break;
                case 10:
                    printf("Error reading openInt from file: %s\n", dir->d_name);
                    break;
                default:
                    printf("Error reading something from file: %s\n", dir->d_name);
                    break;
            }
        }
        closedir(d);
    }

    printf("File count: %d\n", fileCount);
    return 0;
}
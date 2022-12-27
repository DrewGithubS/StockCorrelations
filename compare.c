#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

    i++;
    fileName[i] = 'b';
    i++;
    fileName[i] = 'i';
    i++;
    fileName[i] = 'n';
    i++;
    fileName[i] = '\0';
}

int compareDays(Day * day1, Day * day2) {
    return  strcmp(day1->ticker, day2->ticker) == 0 &&
            day1->period  == day2->period &&
            day1->date    == day2->date &&
            day1->time    == day2->time &&
            day1->open    == day2->open &&
            day1->high    == day2->high &&
            day1->low     == day2->low &&
            day1->close   == day2->close &&
            day1->volume  == day2->volume &&
            day1->openInt == day2->openInt;
}

int compareFile(char * fileName, Day * tempDay, Day * tempDay2) {
    int dayCount;
    char inputFileName[75] = "./StockData/";
    char outputFileName[75] = "./ParsedData/";
    setString(inputFileName, "./StockData/\0");
    concatStrings(inputFileName, fileName);
    printf("%s\n", inputFileName); fflush(stdout);
    FILE * file = fopen(inputFileName, "r");
    if(!file) {
        printf("Error opening file... %s\n", inputFileName); fflush(stdout);
        return 21;
    }
    FILE * cmpFile;
    setString(outputFileName, "./ParsedData/\0");
    concatStrings(outputFileName, fileName);
    changeExtension(outputFileName);
    cmpFile = fopen(outputFileName, "wb");
    if(!cmpFile) {
        printf("Error opening output file... %s\n", outputFileName); fflush(stdout);
    }
    fread(&dayCount, 1, sizeof(int), cmpFile);
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

        fread(tempDay2, 1, sizeof(Day), cmpFile);

        if(compareDays(tempDay, tempDay2) != 0) {
            return 22;
        }
    }
    goto noError;
error:
    printf("ERROR CODE: %d\n", error);
    printf("ERROR1: %s\n", line);
    printf("ERROR2: %s\n", iterator);
    printf("ERROR3: %s\n", prevIterator);
    printf("ERROR4: %d %d %d %d %d %d\n", prevIterator[0], prevIterator[1], prevIterator[2], prevIterator[3], prevIterator[4], prevIterator[5]);
    fclose(file);
    fclose(cmpFile);
    return error;

noError:
    fclose(file);
    fclose(cmpFile);
    return 0;
}

int main() {
    FILE * spyFile = fopen("spy.us.txt", "r");

    Day tempDay;
    Day tempDay2;

    DIR *d;
    struct dirent * dir;
    d = opendir("./StockData");
    int correctCount = 0;
    int fileCount = 0;
    char fileName[75] = "./StockData/";
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] == '.') {continue;}
            
            FILE * file = fopen(fileName, "r");
            if(!file) {continue;}
            fileCount++;
            switch(compareFile(dir->d_name, &tempDay, &tempDay2)) {
                case 0:
                    correctCount++;
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
            fclose(file);
        }
        closedir(d);
    }

    printf("File count: %d\nCorrect count: %d\n", fileCount, correctCount);
    return 0;
}
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

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

typedef struct {
    char fileName[50];
    int dayCount;
    Day * days;
} Stock;

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

double getCorrelations(Stock stock1, Stock stock2) {
    double dayCount = 0;
    double correlatedDays = 0;
    int i1 = 0;
    int i2 = 0;

    // printf("Beginning while1...\n"); fflush(stdout);
    while(i1 < stock1.dayCount && i2 < stock2.dayCount) {
        // printf("Doing fread...\n"); fflush(stdout);
recheck:
        // printf("Beginning while2... %d %d %d %d %d %d\n", tempDay1.date < tempDay2.date, !(i1 < dayCount1 && i2 < dayCount2), i1, i2, dayCount1, dayCount2); fflush(stdout);
        while(stock1.days[i1].date < stock2.days[i2].date) {
            if(!(i1 < stock1.dayCount && i2 < stock2.dayCount)) {break;}
            i1++;
        }
        // printf("Beginning while3... %d %d %d %d %d %d\n", tempDay1.date < tempDay2.date, !(i1 < dayCount1 && i2 < dayCount2), i1, i2, dayCount1, dayCount2); fflush(stdout);
        while(stock2.days[i2].date < stock1.days[i1].date) {
            if(!(i1 < stock1.dayCount && i2 < stock2.dayCount)) {break;}
            i2++;
            goto recheck;
        }

        // printf("Doing if...\n"); fflush(stdout);
        if(stock1.days[i1].date == stock2.days[i2].date) {
            dayCount++;
            double dayChange1 = stock1.days[i1].close - stock1.days[i1].open;
            double dayChange2 = stock2.days[i2].close - stock2.days[i2].open;
            correlatedDays += fabs((dayChange1) * (dayChange2)) < 0.001 ? 0 :
                              ((dayChange1) * (dayChange2)) > 0 ? 1 : -1;

        }
        i1++;
        i2++;
    }

    if(dayCount < 100) {
        return -100;
    } else {
        return correlatedDays / dayCount;
    }
}

void removeBin(char * name) {
    int i;
    for(i = 0; name[i] != '\0'; i++) {
        name[i] -= ('a' - 'A') * (name[i] >= 'a' && name[i] <= 'z');
    }

    i -= 4 * (name[i-1] == 'N' && 
              name[i-2] == 'I' && 
              name[i-3] == 'B' && 
              name[i-4] == '.');

    name[i] = '\0';
}

void printResult(FILE * file, char * name1, char * name2, double correlation) {
    char binRemName1[20];
    char binRemName2[20];
    strcpy(binRemName1, name1);
    strcpy(binRemName2, name2);
    removeBin(binRemName1);
    removeBin(binRemName2);

    fprintf(file, "%s-%s: %lf\n", binRemName1, binRemName2, correlation);
}


int main() {
    Day tempDay;

    DIR * d;
    struct dirent * dir;

    d = opendir("./ParsedData");


    int counter1 = 0;

    if(!d) {
        printf("Error opening directory...\n");
        return 1;
    }

    FILE * outputFile = fopen("Correlations.txt", "w");

    double correlation;
    int stockCounter = 0;

    while ((dir = readdir(d)) != NULL) {
        if(dir->d_name[0] == '.') {continue;}
        stockCounter++;
    }

    Stock * stockList = (Stock *) malloc(stockCounter * sizeof(Stock));
    stockCounter = 0;
    char fileName[100];
    d = opendir("./ParsedData");
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_name[0] == '.') {continue;}
        setString(fileName, "./ParsedData/");
        concatStrings(fileName, dir->d_name);
        FILE * file = fopen(fileName, "rb");
        if(!file) {
            printf("FILE FAILED: %s\n", fileName);
            sleep(1);
            continue;
        }
        strcpy(stockList[stockCounter].fileName, dir->d_name);
        fread(&stockList[stockCounter].dayCount, 1, sizeof(int), file);
        stockList[stockCounter].days = 
            (Day *) malloc(stockList[stockCounter].dayCount * sizeof(Day));
        fread(stockList[stockCounter].days, 
              stockList[stockCounter].dayCount, sizeof(Day), file);
        stockCounter++;
        if(stockCounter % 250 == 0) {
            printf("Read %d stocks...\n", stockCounter);
        }
        fclose(file);
    }

    

    printf("Stock counter: %d\n", stockCounter);

    for(int i = 0; i < stockCounter; i++) {
        for(int j = i+1; j < stockCounter; j++) {
            if(j == stockCounter-1) {
                printf("Getting correlation for %d-%d...\n", i, j);
            }
            correlation = getCorrelations(stockList[i], stockList[j]);
            if(correlation == -100) {
                continue;
            }
            printResult(outputFile, 
                        stockList[i].fileName,
                        stockList[j].fileName, 
                        correlation);
        }
    }

    fclose(outputFile);
}
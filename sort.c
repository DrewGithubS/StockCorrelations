#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
	char ticker1[10];
	char ticker2[10];
	double correlation;
} Correlation;

int compare(const void* a, const void* b) {
     Correlation * correl_a = (Correlation *) a;
     Correlation * correl_b = (Correlation *) b;

     if (fabs(correl_a->correlation) == fabs(correl_b->correlation)) return 0;
     else if (fabs(correl_a->correlation) < fabs(correl_b->correlation)) return 1;
     else return -1;
}

void readLine(char * line, Correlation * correlation) {
	char * i;
	for(i = line; *i != '\n'; i++) {}
	*i = '\0';
	
	int iter = 0;
	for(; *line != '-'; line++) {
		correlation->ticker1[iter] = *line;
		iter++;
	}
	correlation->ticker1[iter] = '\0';

	line++;
	iter = 0;
	for(; *line != ':'; line++) {
		correlation->ticker2[iter] = *line;
		iter++;
	}
	correlation->ticker2[iter] = '\0';

	line++;
	line++;
	double multiplier = 1;
	if(*line == '-') {
		multiplier = -1;
		line++;
	}

	correlation->correlation = 0;
	for(; *line != '.'; line++) {
		correlation->correlation *= 10;
		correlation->correlation += (*line) - '0';
	}
	line++;

	double div = 10;
	for(; *line != '\0'; line++) {
		correlation->correlation += (double) (*(line) - '0') / div;
		div *= 10;
	}

	correlation->correlation *= multiplier;
}

int main() {
	char line[200];


	FILE * inputFile = fopen("Correlations.txt", "r");
	int lineCount = 0;
	printf("Counting lines...\n");
	while(fgets(line, sizeof(line), inputFile) != NULL) {
		lineCount++;
		if(lineCount % 1000 == 0) {
			printf("Read %d...\n", lineCount);
		}
	}

	fclose(inputFile);

	Correlation * correls = (Correlation *) 
							malloc(lineCount * sizeof(Correlation));


	inputFile = fopen("Correlations.txt", "r");
	int curlineCount = 0;
	while(fgets(line, sizeof(line), inputFile) != NULL) {
		readLine(line, correls + curlineCount);
		if(fabs(correls[curlineCount].correlation) < 0.5) {
			lineCount--;
		} else {
			curlineCount++;
		}
		if(curlineCount % 1000 == 0) {
			printf("Read %.2lf%%...\n", 100 * (double) curlineCount / (double) lineCount);
		}
	}

	lineCount = curlineCount;

	qsort(correls, lineCount, sizeof(Correlation), compare);

	FILE * negativeCorrel = fopen("NegativeCorrelation.txt", "w");
	FILE * positiveCorrel = fopen("PositiveCorrelation.txt", "w");

	for(int i = 0; i < lineCount; i++) {
		if(fabs(correls[i].correlation) >= 0.5) {
			if(correls[i].correlation < 0) {
				fprintf(negativeCorrel, "%s-%s: %lf\n",
					correls[i].ticker1,
					correls[i].ticker2,
					correls[i].correlation);
			} else {
				fprintf(positiveCorrel, "%s-%s: %lf\n",
					correls[i].ticker1,
					correls[i].ticker2,
					correls[i].correlation);
			}
		}
	}

	fclose(negativeCorrel);
	fclose(positiveCorrel);
}
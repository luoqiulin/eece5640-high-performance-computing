#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <time.h>
#define LOOP 10000

using namespace std;

long long int factorial(long long int n){
    if (n == 1) return 1;
    return n * factorial(n - 1);
}

float floatsSP(float x, int terms){
    float sinTotal = x;
    int coeff;
    int term;
    float each_loop;
    for(int i = 1; i < terms; ++i){
        if (i % 2 == 0){
            coeff = 1;
        }
        else{
            coeff = -1;
        }
        term = 2 * i + 1;
        each_loop = (coeff * pow(x, term) / factorial(term));
        sinTotal += each_loop;
    }

    return sinTotal;
}

double doublesDP(double x, int terms){
    double sinTotal = x;
    int coeff;
    int term;
    double each_loop;
    for(int i = 1; i < terms; ++i){
        if (i % 2 == 0){
            coeff = 1;
        }
        else{
            coeff = -1;
        }
        term = 2 * i + 1;
        each_loop = (coeff * pow(x, term) / factorial(term));
        sinTotal += each_loop;
    }

    return sinTotal;
}

int main(){
    float numbers[4] = {1.0, 2.0, 3.0, 4.0};
    int terms;
    cout << "number of terms: " << endl;
    cin >> terms;
    clock_t initial1, end1, initial2, end2;
    double total_time1, total_time2;

    initial1 = clock();
    for(int l = 0; l < LOOP; l++){
        for (float x : numbers){
            float result;
            result = floatsSP(x, terms);
            if(l == LOOP - 1){
                printf("The single precision floating point function results of %f is: %.25f\n", x,result);
                printf("The single precision floating point cmath results of %f is: %.25f\n", x, sin(x));
            }
        }
    }

    end1 = clock();
    total_time1 = (double)(end1 - initial1) / CLOCKS_PER_SEC;
    printf("Runtime of SP: %.20f\n", total_time1);

    printf("\n");

    initial2 = clock();
    for(int l = 0; l < LOOP; l++){
        for (double x : numbers){
            double result;
            result = doublesDP(x, terms);
            if(l == LOOP - 1){
                printf("The double precision floating point function results of %f is: %.25f\n", x, result);
                printf("The double precision floating point cmath results of %f is: %.25f\n", x, sin(x));
            }
        }
    }

    end2 = clock();
    total_time2 = (double)(end2 - initial2) / CLOCKS_PER_SEC;
    printf("Runtime of DP: %.20f\n", total_time2);
    return 0;
}
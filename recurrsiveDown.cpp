#include "bintree.hpp"
#include "differenciator.hpp"

#include "recurrsiveDown.hpp"

// const char* S = NULL;

// int p = 0;

int getG (const char* str) {//get end of str
    printf ("I'm in getG\n");

    S = str;
    p = 0;
    int val = getE();
    if (S[p] == '$') {
        printf ("I'm out getG\n");
        return val;
    }
}

int getE () {
    printf ("I'm in getE\n");
    int val = getT ();
    
    while (S[p] == '+' || S[p] == '-') {//sum, sub
        int op = S[p];
        p++;
        int val2 = getT ();
        if (op == '+') {
            val += val2;
        } else {
            val -= val2;
        }
    }
    
    printf ("I'm out getE\n");
    
    return val;
}

int getT () {
    printf ("I'm in getT\n");
    int val = getP ();
    
    while (S[p] == '*' || S[p] == '/') {// mul and div
        int op = S[p];
        p++;
        int val2 = getP ();
        if (op == '*') {
            val *= val2;
        } else {
            val /= val2;
        }
    }
    printf ("I'm out getT\n");
    return val;
}

int getP () {
    printf ("I'm in getP\n");
    if (S[p] == '(') {//scopes
        p++;
        int val = getE();
        if (S[p] == ')') {
            p++;
        } else {
            printf ("error getP\n");
            exit (0);
        }

        printf ("I'm out getP\n");
        
        return val;
    } else {
            printf ("I'm out getP go to getN\n");
        return getN ();
    }
}

int getN () {
    printf ("I'm in getN\n");       //get number

    int val = 0;
    int savP = 0;
    do {
        val = val * 10 + S[p] - '0';
        p++;
    } while ('0' <= S[p] && S[p] <= '9');

    savP = p;

    printf ("I'm out getN\n");

    return val;
}
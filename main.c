#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CLAUSES 1000
#define MAX_LITERALS 100

typedef struct {
    int literals[MAX_LITERALS];
    int size;
} Clause;

Clause clauses[MAX_CLAUSES];
int num_clauses = 0;

int contains_literal(Clause* c, int lit) {
    int i;
    for (i = 0; i < c->size; i++)
        if (c->literals[i] == lit)
            return 1;
    return 0;
}

int clause_equals(Clause* a, Clause* b) {
    int i, j, found;
    if (a->size != b->size) return 0;
    for (i = 0; i < a->size; i++) {
        found = 0;
        for (j = 0; j < b->size; j++) {
            if (a->literals[i] == b->literals[j]) {
                found = 1;
                break;
            }
        }
        if (!found) return 0;
    }
    return 1;
}

int add_clause(Clause* new_c) {
    int i;
    for (i = 0; i < num_clauses; i++) {
        if (clause_equals(&clauses[i], new_c)) return 0;
    }
    clauses[num_clauses++] = *new_c;
    return 1;
}

int resolve(Clause* a, Clause* b, Clause* result) {
    int i, j, k;
    result->size = 0;

    for (i = 0; i < a->size; i++) {
        int lit = a->literals[i];
        for (j = 0; j < b->size; j++) {
            if (lit == -b->literals[j]) {

                for (k = 0; k < a->size; k++)
                    if (a->literals[k] != lit)
                        result->literals[result->size++] = a->literals[k];


                for (k = 0; k < b->size; k++) {
                    int l = b->literals[k];
                    if (l != -lit && !contains_literal(result, l))
                        result->literals[result->size++] = l;
                }
                return 1;
            }
        }
    }
    return 0;
}

int main() {
    FILE* file;
    char line[1024];
    int lit;
    char* token;
    Clause c;
    Clause resolvent;
    int i, j, changed;

    file = fopen("mpi.txt", "r");
    if (!file) {
        printf("Eroare: fisierul mpi.txt nu a putut fi deschis.\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '%' || line[0] == '0') break;

        c.size = 0;
        token = strtok(line, " \n");
        while (token != NULL) {
            lit = atoi(token);
            if (lit == 0) break;
            c.literals[c.size++] = lit;
            token = strtok(NULL, " \n");
        }

        clauses[num_clauses++] = c;
    }
    fclose(file);


    printf("Clauze citite din fisier:\n");
    for (i = 0; i < num_clauses; i++) {
        for (j = 0; j < clauses[i].size; j++) {
            printf("%d ", clauses[i].literals[j]);
        }
        printf("0\n");
    }


    changed = 1;
    while (changed) {
        changed = 0;
        for (i = 0; i < num_clauses; i++) {
            for (j = i + 1; j < num_clauses; j++) {
                if (resolve(&clauses[i], &clauses[j], &resolvent)) {
                    if (resolvent.size == 0) {
                        printf("\nFormula este NESATISFACUTOARE (s-a generat clauza vida).\n");
                        return 0;
                    }
                    if (add_clause(&resolvent)) {
                        changed = 1;
                    }
                }
            }
        }
    }

    printf("\nFormula este POSIBIL satisfacatoare (nu s-a generat clauza vida).\n");
    return 0;
}

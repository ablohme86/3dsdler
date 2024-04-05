#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // access() function
#include <sys/stat.h> // stat() function
#include <signal.h> // signal() function

#define MAX_LINE_LENGTH 1000

void parse_arguments(int argc, char *argv[], char **search_term, char **region, char **ignore_term) {
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--wildcard=", 11) == 0) {
            *search_term = argv[i] + 11; // Henter verdi etter "="
        } else if (strncmp(argv[i], "--region=", 9) == 0) {
            *region = argv[i] + 9; // Henter verdi etter "="
        } else if (strncmp(argv[i], "--ignore=", 9) == 0) {
            *ignore_term = argv[i] + 9; // Henter verdi etter "="
        }
    }
}

void handle_sigint(int sig) {
    // Avslutt programmet
    exit(0);
}

int main(int argc, char *argv[]) {
    char *search_term = NULL;
    char *region = NULL;
    char *ignore_term = NULL;

    parse_arguments(argc, argv, &search_term, &region, &ignore_term);

    if (search_term == NULL || region == NULL) {
        fprintf(stderr, "Bruk: %s --wildcard=<nøkkelord> --region=<region> [--ignore=<nøkkelord>]\n", argv[0]);
        return 1;
    }

    // Registrer signalhåndterer for SIGINT (CTRL-C)
    signal(SIGINT, handle_sigint);

    char *prefix = "https://myrient.erista.me/files/No-Intro/Nintendo%20-%20Nintendo%203DS%20(Decrypted)";
    char line[MAX_LINE_LENGTH];
    FILE *file = fopen("dblist", "r");

    if (file == NULL) {
        fprintf(stderr, "Kunne ikke åpne filen.\n");
        return 1;
    }

    int files_found = 0; // Variabel for å telle antall filer funnet

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        char *ptr = strstr(line, search_term);
        if (ptr != NULL) {
            // Sjekk om ignore-termen er til stede
            if (ignore_term != NULL && strstr(line, ignore_term) != NULL) {
                continue; // Hopp over denne filen hvis ignore-termen er funnet
            }

            ptr = strstr(line, "href=\"");
            if (ptr != NULL) {
                ptr += strlen("href=\"");
                char *end = strstr(ptr, "\">");
                if (end != NULL) {
                    // Fjern trailing whitespace
                    while (end > ptr && (*(end-1) == ' ' || *(end-1) == '\t'))
                        end--;
                    // Sett null-terminator ved slutten av URLen
                    *end = '\0';
                    // Konstruer filnavnet
                    char filename[MAX_LINE_LENGTH];
                    sprintf(filename, "%s.zip", ptr);

                    // Sjekk om regionen er riktig
                    char *region_ptr = strstr(filename, region);
                    if (region_ptr != NULL) {
                        // Finn navnet inne i <a>-taggen
                        char *name_start = strstr(line, ">");
                        char *name_end = strstr(name_start, "</a>");
                        if (name_start != NULL && name_end != NULL) {
                            name_start = strstr(name_start, ">") + 1;
                            *name_end = '\0'; // Null-terminer navnet
                            printf("Laster ned: %s\n", name_start);
                        } else {
                            printf("Laster ned: %s\n", filename);
                        }
                        //printf("Fra: %s/%s\n", prefix, ptr);
                        char command[MAX_LINE_LENGTH + 100];
                        sprintf(command, "wget -q \"%s/%s.zip\"", prefix, ptr);
                        system(command);  // Kjører wget-kommandoen
                        files_found++; // Øk telleren for antall filer funnet
                    }
                }
            }
        }
    }

    fclose(file);

    // Hvis ingen filer ble funnet, skriv ut feilmelding
    if (files_found == 0) {
        fprintf(stderr, "Finner ingen filer ved oppgitte parametere.\n");
        return 1;
    }

    return 0;
}


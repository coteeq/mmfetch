#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

char **argv0;
#include "arg.h"

#define RDBUF_SIZE 1024

struct colors {
    char *key;
    char *value;
};

typedef void (*printer_t)(char *, char *, struct colors *data);

void die_usage (const char *prog_name);
void entry_printer (char *key, char *value, struct colors *cols);
void parse_entries (FILE *stream, struct colors *cols, printer_t printer);

int main (int argc, char **argv) {
    char *entries_path = NULL;
    struct colors cols = { NULL, NULL };

    ARGBEGIN {
        case 'c': entries_path = ARGF (); break;
        case 'k': cols.key = ARGF (); break;
        case 'v': cols.value = ARGF (); break;

        case 'h':
        default:
            die_usage (argv[0]); break;
    } ARGEND;

    if (!cols.key) cols.key = "32";
    if (!cols.value) cols.value = "31";

    FILE *stream = stdin;
    if (entries_path)
        stream = fopen (entries_path, "r");
    
    if (!stream) {
        perror ("fopen");
        exit (EXIT_FAILURE);
    }

    parse_entries (stream, &cols, entry_printer);

    return 0;
}

void die_usage (const char *prog_name) {
    fprintf (stderr,
        "mmfetch - the most minimal fetch\n"
        "Usage: %s [-k SEQ] [-v SEQ] [-c PATH] [-h]\n\n"
        
        "OPTIONS\n"
        "    -k SEQ        change key color sequence to SEQ\n"
        "    -v SEQ        change value color sequence to SEQ\n"
        "    -c PATH       read entries from PATH, instead of stdin\n\n"
        
        "ENTRIES FORMAT\n"
        "    key=value\n",
        prog_name);
    exit (EXIT_FAILURE);
}

void entry_printer (char *key, char *value, struct colors *cols) {
    printf ("\033[%sm%s:\033[0m \033[%sm%s\033[0m\n",
            cols->key, key, cols->value, value);
}

void parse_entries (FILE *stream, struct colors *cols, printer_t printer) {
    char buf[RDBUF_SIZE];
    while (fgets (buf, RDBUF_SIZE, stream)) {
        char *value = strchr (buf, '=');
        
        char *stop_at_newline = strchr (value ? value : buf, '\n');
        if (stop_at_newline) *stop_at_newline = 0;
        
        if (value) *value = 0;
        printer (buf, value ? value + 1 : value, cols);
    }
}

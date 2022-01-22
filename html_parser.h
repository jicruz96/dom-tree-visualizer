#ifndef HTML_H
#define HTML_H

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "./nary_trees/nary_trees.h"

#define BUFFER_SIZE 1000000
#define tolower(x) ((x) + 32)
#define toupper(x) ((x) - 32)
#define islowercase(x) ((x) >= 'a' && (x) <= 'z')
#define isuppercase(x) ((x) >= 'A' && (x) <= 'Z')
#define isalpha(x) (islowercase(x) || isuppercase(x))
#define iswhitespace(x) ((x) == ' ' || (x) == '\t')


typedef nary_tree_t dom_t;

typedef struct html_element_attr_s
{
    char *attr;
    char *val;
    struct html_element_attr_s *next;
} html_element_attr_t;

typedef struct html_element_s
{
    char *name;
    char **innerText;
    size_t nb_innerText;
    html_element_attr_t *attrs;
} html_element_t;


#endif /* HTML_H */

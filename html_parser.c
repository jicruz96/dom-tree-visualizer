#include "html_parser.h"


size_t _strlen(char *s)
{
    int len = 0;

    while (*s++)
        len++;
    
    return (len);
}

int same_string(char *s1, char *s2, int up_to)
{
    int i = 1;

    while (*s1 && *s2)
    {
        if (up_to && i == up_to)
            break;
        if (*s1 != *s2)
            return (0);
        
        s1 += 1;
        s2 += 1;
        i += 1;
    }
    return (*s1 == *s2);
}

void print_dom_node(dom_t const *node, size_t depth)
{
    html_element_t *element;
    html_element_attr_t *attr;

    if (!node)
        return;
    
    element = (html_element_t *)node->content;
    printf("%*s<%s", (int)depth * 2, "", element->name);
    for (attr = element->attrs; attr; attr = attr->next)
        printf(" %s=\"%s\"", attr->attr, attr->val);
    printf(">\n");
}

int same_character_case_insensitive(char a, char b)
{
    return (
        a == b
        ||
        (islowercase(a) && a == tolower(b))
        ||
        (isuppercase(a) && a == toupper(b))
    );
}

int same_string_case_insensitive(char *s1, char *s2, int up_to)
{
    int i = 1;
    while (*s1 && *s2)
    {
        if (up_to && i == up_to)
            break;
        if (!same_character_case_insensitive(*s1, *s2))
            return (0);
        s1 += 1;
        s2 += 1;
        i += 1;
    }

    return (same_character_case_insensitive(*s1, *s2));
}

void skip_whitespace_and_newlines(char **buffer)
{
    if (!buffer || !(*buffer))
        return;

    while (**buffer && (iswhitespace(**buffer) || **buffer == '\n'))
        *buffer += 1;
}

void skip_whitespace(char **buffer)
{
    if (!buffer || !(*buffer))
        return;

    while (**buffer && iswhitespace(**buffer))
        *buffer += 1;
}

int is_self_closing(char *name)
{
    char *self_closing_tags[] = {
        "area",
        "base",
        "br",
        "col",
        "command",
        "embed",
        "hr",
        "img",
        "input",
        "keygen",
        "link",
        "meta",
        "param",
        "source",
        "track",
        "wbr",
        NULL
    };


    for (int i = 0; self_closing_tags[i]; i++)
        if (same_string(self_closing_tags[i], name, 0))
            return (1);
    
    return (0);
}

int is_doctype_html(char **buffer)
{
    if (!buffer || !(*buffer))
        return (0);
    
    skip_whitespace_and_newlines(buffer);
    
    if (!same_string_case_insensitive(*buffer, "<!doctype", _strlen("<!doctype")))
    {
        printf("is_doctype_html: ABORTING: file does not start with <!doctype\n");
        return (0);
    }
    
    *buffer += _strlen("<!doctype");

    skip_whitespace_and_newlines(buffer);
    
    if (!same_string_case_insensitive(*buffer, "html", _strlen("html")))
        return (0);
    
    *buffer += _strlen("html");

    skip_whitespace_and_newlines(buffer);
    
    if (**buffer != '>')
        return (0);

    
    *buffer += 1;
    return (1);
}

void free_attrs(html_element_attr_t *attrs)
{
    if (attrs)
    {
        free_attrs(attrs->next);
        free(attrs);
    }
}

html_element_attr_t *attr_init(char *name, char *val)
{
    if (!name || !val)
        return (NULL);
    
    html_element_attr_t *attr = malloc(sizeof(html_element_attr_t));

    if (!attr)
        return (NULL);
    
    attr->attr = name;
    attr->val = val;
    attr->next = NULL;
    return (attr);
}

html_element_attr_t *make_attr(char **buffer)
{
    skip_whitespace_and_newlines(buffer);

    char *name = *buffer;
    int expecting_quote = 0;

    while (**buffer && (isalpha(**buffer) || **buffer == '-'))
        *buffer += 1;
    
    if (**buffer != '=')
    {
        printf("make_attr: ABORTING: Expected '='\n");
        return (NULL);
    }
    
    **buffer = '\0';
    *buffer += 1;

    if (**buffer == '"')
    {
        *buffer += 1;
        expecting_quote = 1;
    }


    char *val = *buffer;
    int escape = 0;

    while (**buffer && ((expecting_quote && **buffer != '"') || (!expecting_quote && !iswhitespace(**buffer) && **buffer != '\n' && **buffer != '>') || escape))
    {
        escape = (**buffer == '\\');
        *buffer += 1;
    }

    if (expecting_quote && **buffer != '"')
    {
        printf("make_attr: ABORTING: Expected '\"'\n");
        
        return (NULL);        
    }

    if (!expecting_quote && (!iswhitespace(**buffer) && **buffer != '\n' && **buffer != '>'))
    {
        printf("make_attr: ABORTING: buffer: [%c]\n", **buffer);
        return (NULL);
    }
    
    if (**buffer != '>')
    {
        **buffer = '\0';
        *buffer += 1;
    }
    return attr_init(name, val);
}

dom_t *dom_node_init(char *name, html_element_attr_t *attrs, dom_t *parent)
{
    if (!name)
        return (NULL);
    
    html_element_t *element = malloc(sizeof(html_element_t));

    if (!element)
        return (NULL);
    
    element->name = name;
    element->attrs = attrs;
    element->nb_innerText = 0;
    element->innerText = NULL;
    dom_t *node = nary_tree_init(parent, element);

    if (!node)
    {
        free(element);
        return (NULL);
    }
    return (node);
}

void free_dom_nodes(dom_t *nodes)
{
    if (nodes)
    {
        free_dom_nodes(nodes->next);
        free(nodes->content);
        free(nodes);
    }
}

int assign_innerText(char *text, html_element_t *element)
{
    printf("assign_innerText\n");
    if (!text || !element)
        return (0);
    printf("assigning [%s] to [%s]\n", text, element->name);
    
    element->innerText = realloc(element->innerText, sizeof(char *) * (element->nb_innerText + 1));
    if (!element->innerText)
        return (0);
    
    element->innerText[element->nb_innerText] = text;
    element->nb_innerText += 1;
    return (1);
}

void skip_to_end_of_quote(char **buffer)
{
    char quote = **buffer == '"' ? '"' : '\'';
    int escape = 0;

    *buffer += 1;

    while (**buffer && (**buffer != quote || escape))
    {
        escape = (**buffer == '\\');
        *buffer += 1;    
    }

    if (**buffer)
    {
        *buffer += 1;
    }
}

void skip_to_end_of_bracket(char **buffer)
{
    char opening_bracket = **buffer;
    char closing_bracket;
    int level = 1;

    if (**buffer == '{')
        closing_bracket = '}';
    else if (**buffer == '(')
        closing_bracket = ')';
    else
        closing_bracket = ']';
    
    *buffer += 1;

    while (**buffer)
    {
        if (**buffer == closing_bracket)
        {
            level -= 1;
            if (!level)
            {
                *buffer += 1;
                return;
            }
        }
        else if (**buffer == opening_bracket)
        {
            level += 1;
        }
        else if (**buffer == '\'' || **buffer == '"')
        {
            skip_to_end_of_quote(buffer);
            continue;
        }
        *buffer += 1;
    }
}

void parse_script_tag(char **buffer)
{
    int escape = 0;

    while (**buffer && (**buffer != '<' || escape))
    {
        if (**buffer == '{' || **buffer == '(' || **buffer == '[')
            skip_to_end_of_bracket(buffer);
        else if (**buffer == '\'' || **buffer == '"')
            skip_to_end_of_quote(buffer);
        else
        {
            escape = (**buffer == '\\');
            *buffer += 1;
        }
    }
}

dom_t *make_dom_node(char **buffer, dom_t *parent)
{
    char *orig;
    int escape = 0;

    skip_whitespace_and_newlines(buffer);

    orig = *buffer;

    if (parent && same_string("script", ((html_element_t *)parent->content)->name, 0))
    {
        parse_script_tag(buffer);
    }
    else
    {
        while (**buffer && (**buffer != '<' || escape))
        {
            escape = (**buffer == '\\');
            *buffer += 1;
        }
    }
    
    if (!(**buffer))
    {
        printf("Reached end of file without finding end of html\n");
        return (NULL);
    }
    
    **buffer = '\0';
    *buffer += 1;
    if (orig != (*buffer - 1) && (!parent || !assign_innerText(orig, (html_element_t *)parent->content)))
    {
        if (parent)
            printf("ABORTING: Found inner text but not tag to put it in!\n");
        else
            printf("ABORTING: assigning inner Text failed\n");
        return (NULL);
    }

    skip_whitespace_and_newlines(buffer);
    
    char *name = *buffer;

    while (**buffer && (isalpha(**buffer) || **buffer == '-' || (name == *buffer && **buffer == '/')))
        *buffer += 1;
    
    if (!(**buffer))
    {
        printf("Reached end of file without finding closing bracket for: [%s]\n", name);
        return (NULL);
    }
    
    if (name[0] == '/')
    {
        if (!parent)
        {
            printf("ABORTING: found closing bracket without an opening one: [%s]\n", name);
            return (NULL);
        }
        if (**buffer != '>')
        {
            if (!iswhitespace(**buffer) && **buffer != '\n')
                return (NULL);
            
            **buffer = '\0';
            *buffer += 1;
            
            skip_whitespace_and_newlines(buffer);

            if (**buffer != '>')
                return (NULL);
        }

        **buffer = '\0'; 
        *buffer += 1;

        char *parent_name = ((html_element_t *)parent->content)->name;
        if (!same_string(name + 1, parent_name, _strlen(parent_name)))
        {
            printf("ABORTING: found closing tag [%s] but expected [%s]\n", name + 1, parent_name);
            return (NULL);
        }
        return (parent);
    }

    char *end_of_name = *buffer;

    html_element_attr_t *attrs = NULL, *attrs_head = NULL, *attr;

    while (**buffer && **buffer != '>' && **buffer != '/')
    {
        skip_whitespace_and_newlines(buffer);
        attr = make_attr(buffer);
        if (!attr)
        {
            free_attrs(attrs);
            return (NULL);
        }

        if (attrs)
            attrs->next = attr;
        else
            attrs_head = attr;
        
        attrs = attr;
    }

    if (**buffer == '\0')
    {
        free_attrs(attrs);
        return (NULL);
    }

    if (**buffer == '/')
    {
        if (!is_self_closing(name))
        {
            free_attrs(attrs);
            return (NULL);
        }
        **buffer = '\0';
        *buffer += 1;
        skip_whitespace_and_newlines(buffer);
        if (**buffer != '>')
        {
            free_attrs(attrs);
            return (NULL);
        }
    }

    *end_of_name = '\0';
    if (**buffer == '>')
    {
        **buffer = '\0';
    }
    *buffer += 1;

    dom_t *node = dom_node_init(name, attrs_head, parent);

    if (!node)
    {
        free_attrs(attrs);
        return (NULL);
    }

    if (is_self_closing(name))
        return (node);

    dom_t *child = NULL;
    dom_t *children = NULL;
    dom_t *children_head = NULL;
    size_t nb_children = 0;
    while (1)
    {
        printf("getting %s's children\n", name);
        child = make_dom_node(buffer, node);

        if (!child)
        {
            printf("no child... freeing shit!\n");
            nary_tree_traverse(parent, print_dom_node);
            free_attrs(attrs);
            free_dom_nodes(children);
            free_dom_nodes(node);
            return (NULL);
        }
        if (child == node)
            break;
        nb_children += 1;
        if (children)
            children->next = child;
        else
            children_head = child;
        
        children = child;
    }

    node->children = children_head;
    node->nb_children = nb_children;
    printf("Returning node [%s]\n", name);
    return (node);
}


dom_t *make_dom_tree(char **buffer)
{
    if (!buffer || !(*buffer))
        return (NULL);

    /**
     * For now, let's only deal with files that follow this format:
     * <!DOCTYPE html>
     * <html>
     * ...
     * </html>
     */
    if (!is_doctype_html(buffer))
    {
        printf("make_dom_tree: ABORTING: Expected DOCTYPE\n");
        return (NULL);
    }
    
    return (make_dom_node(buffer, NULL));
}


char *read_html_file(int fd)
{
    char *buffer       = malloc(sizeof(char) * BUFFER_SIZE);
    ssize_t bytes_read = 0;
    size_t total_bytes = 0;
    size_t buffer_size = BUFFER_SIZE;

    while ((bytes_read = read(fd, buffer + total_bytes, BUFFER_SIZE)) > 0)
    {
        total_bytes += bytes_read;
        if (total_bytes > buffer_size)
        {
            buffer_size += BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer)
                return (NULL); 
        }
    }

    if (bytes_read == -1)
    {
        free(buffer);
        return (NULL);
    }

    buffer[total_bytes] = '\0';
    return (buffer);

}

int main(int ac, char **av)
{
    if (ac < 2)
    {
        printf("Usage: %s file-to-parse.html\n", av[0]);
        exit(1);
    }

    int fd = open(av[1], O_RDONLY);

    if (fd == -1)
    {
        printf("Error: file %s does not exist. Aborting\n", av[1]);
        exit(1);
    }

    char *html_file_buffer = read_html_file(fd);
    dom_t *dom = make_dom_tree(&html_file_buffer);
    nary_tree_traverse(dom, print_dom_node);
    return (0);
}

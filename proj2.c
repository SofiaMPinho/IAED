/*
 * File: proj2.c
 * Author: Sofia Pinho
 * Description: A programme that creates and manages a hierarchical storage system
 * similar to a file system.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHAR_INST 65535
/* comands descriptions */
#define HELP_DESC     "help: Imprime os comandos disponíveis."
#define QUIT_DESC     "quit: Termina o programa."
#define SET_DESC      "set: Adiciona ou modifica o valor a armazenar."
#define PRINT_DESC    "print: Imprime todos os caminhos e valores."
#define FIND_DESC     "find: Imprime o valor armazenado."
#define LIST_DESC     "list: Lista todos os componentes imediatos de um sub-caminho."
#define SEARCH_DESC   "search: Procura o caminho dado um valor."
#define DELETE_DESC   "delete: Apaga um caminho e todos os subcaminhos."
/* errors */
#define NOT_FOUND     "not found"
#define NO_DATA       "no data"
/* boolean values */
#define TRUE          1
#define FALSE         0
/* structs and prototypes */
/* a doubly linked list that stores chars */
typedef struct strnode {
    struct strnode *next, *previous;
    char c;
} str_node;

typedef struct {
    struct strnode *head, *last;
} string;

string* mk_string();
void free_string(string *s);
void add_last_string(string *l, char c);
void remove_last_string(string *l);
void remove_first_string(string *l);
int stringcmp(string *s1, string *s2);
void stringcopy(string *s1, string *s2);
void print_str(string *s);
int len(string *s);
string* remove_directory(string *s1, string *s2);

/* returns a new empty string */
string* mk_string()
{
    string *new_string = malloc(sizeof(string));

    new_string->head = NULL;
    new_string->last = NULL;

    return new_string;
}

/* frees all memory associated with the string s */
void free_string(string *s)
{
    if(s == NULL) {
        free(s);
        return;
    }
    while(s->head != NULL) {
        str_node *next = s->head->next;
        free(s->head);
        s->head = next;
    }
    free(s);
}

/* adds the char c to the string s */
void add_last_string(string *s, char c)
{
    str_node *new_node = malloc(sizeof(str_node));

    new_node->c = c;
    new_node->previous = s->last;
    new_node->next = NULL;
    
    if(s->last != NULL) 
        s->last->next = new_node;
  
    else
        s->head = new_node;

    s->last = new_node;
}

/* removes the last char from the string s */
void remove_last_string(string *s)
{
    str_node *last = s->last;

    if(s->head->next != NULL) {
        s->last = last->previous;
        s->last->next = NULL;
    }

    else {
        s->head = NULL;
        s->last = NULL;
    }

    free(last);
}

/* removes the first char from the string s */
void remove_first_string(string *s)
{
    str_node *first = s->head;

    if(s->head->next != NULL) {
        s->head = first->next;
        s->head->previous = NULL;
    }

    else {
        s->head = NULL;
        s->last = NULL;
    }

    free(first);
}

/* compares the strings s1 and s2, returns 0 if they're equal, a negative
value if s2 comes after alphabetically and a positive value if the
opposite is true */
int stringcmp(string *s1, string *s2)
{
    str_node *c1 = s1->head;
    str_node *c2 = s2->head;
    int res;
    
    while(c1->c == c2->c) {
        if(c1 == s1->last && c2 != s2->last) {
            res = '\0' - c2->next->c;
            return res;
        }
        if(c2 == s2->last && c1 != s1->last) {
            res = c1->next->c - '\0';
            return res;
        }
        if(c1 == s1->last && c2 == s2->last)
            return 0;

        c1 = c1->next;
        c2 = c2->next;
    } 
    res = c1->c - c2->c;
    
    return res;
}

/* copies the content of the string s1 to the string s2 */
void stringcopy(string *s1, string *s2)
{
    str_node *c = s1->head;

    for(;c != NULL; c = c->next)
        add_last_string(s2, c->c);
}

/* prints the string s */
void print_str(string *s)
{
    str_node *c = s->head;

    while(c != NULL) {
        putchar(c->c);
        c = c->next;
    }
}

/* returns the length of the string s */
int len(string *s)
{
    str_node *c;
    int count = 1;

    if(s == NULL)
        return 0;
    
    c = s->head;

    for(; c != NULL; c = c->next, count++);

    return count;
}

/* returns the string equivalent to s2 without s1 in the beginning,
returns NULL if s1 and s2 are equal or if s2 doesn't start with s1*/
string* remove_directory(string *s1, string *s2)
{
    string *new;
    str_node *c1;
    str_node *c2 = s2->head;
    
    if(s1 == NULL) {
        new = mk_string();
        stringcopy(s2, new);
        return(new);
    }
    c1 = s1->head;

    if(stringcmp(s1, s2) == 0)
        return NULL;
    
    for(;c1 != NULL; c1 = c1->next, c2 = c2->next) {
        if(c1->c != c2->c) 
            return NULL;
    }
    new = mk_string();

    for(;c2 != NULL; c2 = c2->next)
        add_last_string(new, c2->c);
    
    return new;
}

/* a struct which stores the pointers to the strings that represent a path, desc,
and its value, value */
typedef struct {
    string *desc;
    string *value;
} Path;

Path* mk_path(string *desc, string *value);
string* read_path_desc();
string* read_path_value();
string* mother_path(string *desc);
string* n_dir(string *desc, int n);
string* sub_dir(string *desc, string *desc_remove);
int number_subpaths(string *desc);
void free_path(Path *path);

/* makes a new path */
Path* mk_path(string *desc, string *value)
{
    Path *new_path = malloc(sizeof(Path));

    new_path->desc = desc;
    new_path->value = value;
    
    return new_path;
}

/* reads input and creates a new string that follows path descriptions rules */
string* read_path_desc()
{
    char c;
    string *desc = mk_string();
    add_last_string(desc, '/');

    while((c = getchar()) != ' ' && c != '\t' && c != '\n') {
        if(c == '/' && desc->last->c == '/')
            continue;

        add_last_string(desc, c);
    }

    if(desc->last->c == '/') 
        remove_last_string(desc);

    return desc;
}

/* reads input and creates a new string that follows value rules */
string* read_path_value()
{
    char c;
    string *value = mk_string();

    while((c = getchar()) != '\n')
        add_last_string(value, c);

    return value;
}

/* returns the string equivalent to the description of the path that has the 
path with description desc as a direct subpath */
string* mother_path(string *desc)
{
    string *new_path_desc = mk_string();
    str_node *c;
    stringcopy(desc, new_path_desc);
    
    c = new_path_desc->last;

    while(c->c != '/') {
        c = c->previous;
        remove_last_string(new_path_desc);
    }
    remove_last_string(new_path_desc);

    return new_path_desc;
}

/* returns the subdirectory number n of the directory desc, if
desc doesn't have enough subdirs or n = 0 returns NULL */
string* n_dir(string *desc, int n)
{
    string *new;
    str_node *c = desc->head;

    if(n == 0)
        return NULL;
    
    if(number_subpaths(desc) < n)
        return NULL;

    new = mk_string();

    for(; n != 0; n--) {
        add_last_string(new, c->c);
        c = c->next;

        for(; c != NULL && c->c != '/'; c = c->next) 
            add_last_string(new, c->c);   
    }
    return new;
}

/* returns the string equivalent to the name of the direct subdirectory 
of the directory desc_remove in the path desc */
string* sub_dir(string *desc, string *desc_remove) {
    string *aux = remove_directory(desc_remove, desc);
    string *new;
    str_node *c;
    
    if(aux == NULL) {
        free(aux);
        return NULL;
    }
    
    new = mk_string();
    c = aux->head->next;

    for(; c != NULL && c->c != '/'; c = c->next)
        add_last_string(new, c->c);

    free_string(aux);
    
    return new;
}

/* returns the number of subpaths in the description desc */
int number_subpaths(string *desc)
{
    string *new_path_desc = mk_string();
    string *mother_path_desc;
    int count = 0;

    stringcopy(desc, new_path_desc);

    while(new_path_desc->head != NULL) {
        count++;
        mother_path_desc = mother_path(new_path_desc);
        free_string(new_path_desc);
        new_path_desc = mother_path_desc;
    }
    free_string(mother_path_desc);

    return count;
}

/* frees all memory associated with the Path path */
void free_path(Path *path)
{
    free_string(path->desc);

    if(path->value != NULL)
        free_string(path->value);
    else
        free(path->value);
    
    free(path);
}

/* an AVL tree that stores pointers to all existing paths in alphabetical order */
typedef struct treenode {
    Path *path;
    struct treenode *left;
    struct treenode *right;
    int height;
} *tree;

int less(string *desc1, string *desc2);
int equal(string *desc1, string *desc2);
tree new_h(Path *path, tree left, tree right);
tree search_tree(tree h, string *desc);
tree insert(tree h, Path *path);
tree max(tree h);
tree min(tree h);
tree delete_tree(tree h, string *desc);
void free_tree(tree h);

/* returns TRUE if desc1 comes after than desc2 alphabetically
and FALSE otherwise */
int less(string *desc1, string *desc2)
{
    return stringcmp(desc1, desc2) > 0;
}

/* returns TRUE if the strings desc1 and desc2 are equal
and FALSE otherwise */
int equal(string *desc1, string *desc2)
{
    if(desc1 == NULL || desc2 == NULL)
        return FALSE;
    if(desc1->head == NULL)
        return FALSE;
    if(desc2->head == NULL)
        return FALSE;
    
    return stringcmp(desc1, desc2) == 0;
}

/* creates and returns a new tree with the trees left and 
right in their respective position */
tree new_h(Path *path, tree left, tree right)
{
    tree new = malloc(sizeof(struct treenode));

    new->path = path;
    new->left = left;
    new->right = right;
    new->height = 1;

    return new;
}

/* returns the height of the tree h */
int height(tree h)
{
    if(h == NULL)
        return 0;
    return h->height;
}

/* rotation of tree h to the left */
tree rotL(tree h)
{
    int hleft, hright, xleft, xright;
    tree x = h->right;
    h->right = x->left;
    x->left = h;

    hleft = height(h->left);
    hright = height(h->right);
    h->height = hleft > hright ? hleft + 1: hright + 1;

    xleft = height(x->left);
    xright = height(x->right);
    x->height = xleft > xright ? xleft + 1: xright + 1;

    return x;
}

/* rotation of tree h to the right */
tree rotR(tree h)
{
    int hleft, hright, xleft, xright;
    tree x = h->left;
    h->left = x->right;
    x->right = h;

    hleft = height(h->left);
    hright = height(h->right);
    h->height = hleft > hright ? hleft + 1: hright + 1;

    xleft = height(x->left);
    xright = height(x->right);
    x->height = xleft > xright ? xleft + 1: xright + 1;

    return x;
}

/* double rotation to the left and right */
tree rotLR(tree h)
{
    if(h == NULL)
        return h;
    h->left = rotL(h->left);
    return rotR(h);
}

/* double rotation to the right and left */
tree rotRL(tree h)
{
    if(h == NULL)
        return h;
    h->right = rotR(h->right);
    return rotL(h);
}

/* balance factor */
int balance(tree h)
{
    if(h == NULL)
        return 0;
    return height(h->left) - height(h->right);
}

/* balances the tree */
tree AVLbalance(tree h)
{
    int balance_factor, hleft, hright;

    if(h == NULL)
        return h;
    balance_factor = balance(h);

    if(balance_factor > 1) {
        if(balance(h->left) >= 0)
            h = rotR(h);
        else
            h = rotLR(h);
    }
    else if(balance_factor < -1) {
        if(balance(h->right) <= 0)
            h = rotL(h);
        else
            h = rotRL(h);
    }
    else {
        hleft = height(h->left);
        hright = height(h->right);
        h->height = hleft > hright ? hleft + 1: hright + 1;
    }
    return h;
}

/* searchs for the Path with description desc in the tree with head
h, returns NULL if the Path isn't in the tree */
tree search_tree(tree h, string *desc)
{
    if(h == NULL)
        return NULL;

    if(equal(h->path->desc, desc))
        return h;
    
    if(less(desc, h->path->desc))
        return search_tree(h->left, desc);
    
    else
        return search_tree(h->right, desc);
}

/* inserts a new Path in the tree with head h */
tree insert(tree h, Path *path)
{
    if(h == NULL)
        return new_h(path, NULL, NULL);

    if(less(path->desc, h->path->desc))
        h->left = insert(h->left, path);
    
    else
        h->right = insert(h->right, path);
    h = AVLbalance(h);
    return h;
}

/* returns the subtree with max value from the tree with head h */
tree max(tree h)
{
    while(h != NULL && h->right != NULL)
        h = h->right;

    return h;
}

/* returns the subtree with minimum value from the tree with head h */
tree min(tree h)
{
    while(h != NULL && h->left != NULL)
        h = h->left;

    return h;
}

/* removes and deletes the Path with description desc from the
tree with head h */
tree delete_tree(tree h, string *desc)
{
    tree aux;

    if(h == NULL) return h;
    if(less(desc, h->path->desc))  h->left = delete_tree(h->left, desc);
    else if(less(h->path->desc, desc))  h->right = delete_tree(h->right, desc);   
    else {
        if(h->left != NULL && h->right != NULL) {
            Path *x = h->path;
            aux = max(h->left);
            h->path = aux->path;
            aux->path = x;
            h->left = delete_tree(h->left, aux->path->desc); }
        else {
            aux = h;
            if(h->left == NULL && h->right == NULL) h = NULL;
            else if(h->left == NULL) h = h->right;
            else h = h->left;            
            free(aux); }
    }
    h = AVLbalance(h);
    return h;
}

/* frees all memory associated with the tree with head h */
void free_tree(tree h)
{   
    if(h == NULL)
        return;

    free_path(h->path);
    free_tree(h->left);
    free_tree(h->right);
    free(h);
}

/* a doubly linked list that store pointers to all existing paths in order of creation */
typedef struct pathnode {
    struct pathnode *next;
    struct pathnode *previous;
    Path *path;
} path_node;

typedef struct {
    struct pathnode *first, *last;
} path_list;

path_list *mk_pathlist();
void free_pathlist(path_list *list);
void add_to_pathlist(path_list *list, path_node *next, Path *path);
path_node* where_to_add_pathlist(path_list *list, Path *path);
path_node* find_item_pathlist(path_list *list, string *desc);
path_node* remove_item_path_list(path_list *list, path_node *node);

/* creates and returns a new empty path_list */
path_list *mk_pathlist()
{
    path_list *new_list = malloc(sizeof(path_list));

    new_list->first = NULL;
    new_list->last = NULL;

    return new_list;
}

/* frees the path_list list but doesn't free the memory associated
with its paths */
void free_pathlist(path_list *list)
{
    while(list->first != NULL) {
        path_node *next = list->first->next;
        free(list->first);
        list->first = next;
    }
    free(list);
}

/* adds the Path path to the path_list list before the path_node next*/
void add_to_pathlist(path_list *list, path_node *next, Path *path)
{
    path_node *new_node = malloc(sizeof(struct pathnode));

    new_node->path = path;
    new_node->next = next;
    if(next == NULL && list->first == NULL) {
        list->first = new_node;
        list->last = new_node;
        new_node->previous = NULL;
        return; }
    if(next == NULL) {
        new_node->previous = list->last;
        list->last->next = new_node;
        list->last = new_node;
        return; }
    if(next == list->first) {
        list->first = new_node;
        next->previous = new_node;
        new_node->previous = NULL;
        return; }
    new_node->previous = next->previous;
    next->previous->next = new_node;
    next->previous = new_node;
}

/* returns the path_node next to where the new path should be added */
path_node* where_to_add_pathlist(path_list *list, Path *path)
{
    path_node *current;
    string *mother;
    int num;
    
    if(list->first == NULL) 
        return NULL;
    if(number_subpaths(path->desc) == 1) 
        return NULL;
    current = list->first;
    mother = mother_path(path->desc);
    num = number_subpaths(mother);
    for(; current != NULL; current = current->next) {
        if(equal(current->path->desc, mother))
            break; }
    if(current == NULL) {
        free_string(mother);
        return NULL; }
    for(current = current->next; current != NULL; current = current->next) {
        if(number_subpaths(current->path->desc) <= num) 
            break; }
    free_string(mother);
    return current;
}

/* finds the path with description desc in the list, returns 
its node if the path is found and NULL if it doesn't exist */
path_node* find_item_pathlist(path_list *list, string *desc)
{
    path_node *current = list->first;

    for(; current != NULL; current = current->next) {
        if(equal(desc, current->path->desc)) {
            return current;
        }
    }
    return NULL;
}

/* frees the path_node node and returns the next node */
path_node* remove_item_path_list(path_list *list, path_node *node)
{
    path_node *next = node->next;

    if(node != list->first) 
        node->previous->next = node->next;
    else 
        list->first = node->next;

    if(node != list->last) 
        node->next->previous = node->previous;
    else
        list->last = node->previous;
            
    free_path(node->path);
    free(node);

    return next;
}

/* prints all available comands and their descriptions */
void help()
{
    printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", HELP_DESC, QUIT_DESC, SET_DESC,
    PRINT_DESC, FIND_DESC, LIST_DESC, SEARCH_DESC, DELETE_DESC);
}

/* adds a new path and all its mother paths that don't already exist
to the tree head and to the path_list plist */
tree add_new_path(tree alph, path_list *list, string *desc, string *value)
{
    string *dir = desc;
    path_node *next = NULL;
    Path *new_path;
    int i, num_dir = number_subpaths(desc), found = 0;

    for(i = 1; i < num_dir; i++) {
        dir = n_dir(desc, i);
        if(alph == NULL) {
            new_path = mk_path(dir, NULL);
            alph = new_h(new_path, NULL, NULL);
            add_to_pathlist(list, NULL, new_path);
            found = 1; }
        else if(search_tree(alph, dir) == NULL) {
            new_path = mk_path(dir, NULL);
            if(found == 0) {
                next = where_to_add_pathlist(list, new_path);
                found = 1; }
            add_to_pathlist(list, next, new_path);
            alph = insert(alph, new_path); }
        else free_string(dir); }
    if(alph == NULL) {
        new_path = mk_path(desc, value);
        alph = new_h(new_path, NULL, NULL);
        add_to_pathlist(list, NULL, new_path);
        return alph; }
    new_path = mk_path(desc, value);
    if(found == 0) next = where_to_add_pathlist(list, new_path);
    add_to_pathlist(list, next, new_path);
    alph = insert(alph, new_path);
    return alph;
}

/* adds or modifies a value */
tree set(tree alph, path_list *list)
{
    string *desc = read_path_desc();
    string *new_value = read_path_value();
    string *old_value;
    tree head;

    if(alph == NULL) {
        alph = add_new_path(alph, list, desc, new_value);
        return alph;
    }
    if((head = search_tree(alph, desc)) != NULL) {
        old_value = head->path->value;
        head->path->value = new_value;
        free_string(old_value);
        free_string(desc);
        return alph;
    }
    alph = add_new_path(alph, list, desc, new_value);
    return alph;
}

/* prints all paths and values */
void print(path_list *list)
{
    path_node *current = list->first;

    for(; current != NULL; current = current->next) {
        if(current->path->value != NULL) {
            print_str(current->path->desc);
            printf(" ");
            print_str(current->path->value);
            printf("\n");
        }
    }
}

/* prints the value stored in a path */
void find(tree alph)
{
    string *desc = read_path_desc();
    tree h;

    h = search_tree(alph, desc);

    free_string(desc);

    if(h == NULL) {
        printf("%s\n", NOT_FOUND);
        return;
    }
    if(h->path->value == NULL) {
        printf("%s\n", NO_DATA);
        return;
    }

    print_str(h->path->value);
    printf("\n");
}

/* lists all directory dir components */
void list(tree alph, string *dir)
{   
    string *aux, *subdir;

    if(alph == NULL)
        return;
    
    list(alph->right, dir);

    if(len(alph->path->desc) > len(dir)) {
        if((subdir = sub_dir(alph->path->desc, dir)) != NULL) {
            aux = remove_directory(dir, alph->path->desc);
            remove_first_string(aux);   

            if(equal(subdir, aux)) {
                print_str(subdir);
                printf("\n");
            }
            free_string(aux);
            free_string(subdir);
        }
    }

    list(alph->left, dir);
}

/* searchs for a path through its value */
void search(path_list *list)
{
    string *value = read_path_value();
    path_node *current = list->first;

    for(; current != NULL; current = current->next) {
        if(equal(current->path->value, value)) {
            free_string(value);
            print_str(current->path->desc);
            printf("\n");

            return;
        }
    }

    printf("%s\n", NOT_FOUND);
    free_string(value);
}

/* deletes a path and all its subpaths */
tree delete(tree alph, path_list *plist)
{
    string *dir, *desc = read_path_desc();
    path_node *node;
    int n = number_subpaths(desc);

    node = find_item_pathlist(plist, desc);

    if(node == NULL) {
        printf("%s\n", NOT_FOUND);
        free_string(desc);
        return alph;
    }
    while(node != NULL && equal(desc, (dir = n_dir(node->path->desc, n)))) {
        free_string(dir);
        alph = delete_tree(alph, node->path->desc);
        node = remove_item_path_list(plist, node);
    }
    if(node != NULL) 
        free_string(dir);
    free_string(desc);
    return alph;
}

int main()
{
    char command[MAX_CHAR_INST];
    tree alph = NULL;
    path_list *plist = mk_pathlist();

    scanf("%s", command);
    for(;strcmp(command, "quit") != 0; scanf("%s", command)) {
        if(strcmp(command, "help") == 0)
            help();        
        if(strcmp(command, "set") == 0) {
            getchar(); /* space */
            alph = set(alph, plist); }
        if(strcmp(command, "print") == 0)  print(plist);
        if(strcmp(command, "find") == 0) {
            getchar(); /* space */
            find(alph); }
        if(strcmp(command, "list") == 0) {
            string *dir;
            if(getchar() == '\n') {
                dir = NULL;
                if(plist->first != NULL) list(alph, dir);
                else printf("%s\n", NOT_FOUND); }
            else {
                dir = read_path_desc();
                if(search_tree(alph, dir) != NULL) list(alph, dir);
                else printf("%s\n", NOT_FOUND); }
            if(dir != NULL) free_string(dir);
            else free(dir); }
        if(strcmp(command, "search") == 0) {
            getchar(); /* space */
            search(plist); }
        if(strcmp(command, "delete") == 0) {
            if(getchar() == '\n') {
                if(alph != NULL) {
                    free_pathlist(plist);
                    free_tree(alph);
                    plist = mk_pathlist();
                    alph = NULL; } }
            else alph = delete(alph, plist); }  
    }
    free_pathlist(plist);
    free_tree(alph);
    return 0;
}
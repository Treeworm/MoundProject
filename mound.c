#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define THRESHOLD 20
typedef struct LNode List;
typedef struct MNode Mound;

// TODO: make type T? dirty implementation? count?
struct LNode
{
    int value;
    struct LNode *next;
};

struct MNode
{
    struct LNode *list;
    bool dirty;
    int c;
};

int val(Mound *node)
{
    if (!node->list)
        return INT32_MAX;
    return node->list->value;
}

int get_random(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

Mound *newMNode()
{
    Mound *node = malloc(sizeof(struct MNode));
    node->list = NULL;
    node->dirty = false;
    node->c = 0;
    return node;
}

Mound **expand(Mound **tree, int depth)
{
    Mound **re = realloc(tree, sizeof(Mound *) * (1 << (depth + 1)) - 1);
    if (!re)
    {
        printf("Out of memory");
        exit(1);
    }
    for (int i = (1 << depth) - 1; i < (1 << depth + 1) - 1; i++)
    {
        re[i] = newMNode();
    }
    return re;
}

void insert_in_list(Mound *m, int v)
{
    List *list = malloc(sizeof(struct LNode));
    list->value = v;
    list->next = NULL;
    if (m->list)
        list->next = m->list;
    m->list = list;
}

// TODO : change leafs to internal / internal_nodes
Mound **insert(Mound **tree, int *depth, int v)
{
    // Tree node range = [0, 2^(h+1)-2]
    int n = (1 << (*depth + 1)) - 1;
    int leafs = 1 << *depth; // Index range of internal nodes = [0, 2^h-2];
    for (int i = 0; i < THRESHOLD; i++)
    {
        int l = get_random(n - leafs, n - 1); // Random from [2^h-1, 2^(h+1)-2] = Leaf index range
        if (v <= val(tree[l]))
        {
            if (*depth == 0 || val(tree[(l + 1) / 2 - 1]) < v)
            {
                insert_in_list(tree[l], v);
            }
            else
            {
                int lo = 0, hi = *depth - 1, ld;
                while (lo <= hi)
                {
                    int mid = (lo + hi) / 2;
                    ld = ((l + 1) >> (*depth - mid)) - 1;
                    if (val(tree[ld]) < v)
                    {
                        lo = mid + 1;
                    }
                    else
                    {
                        hi = mid - 1;
                    }
                }
                insert_in_list(tree[ld], v);
            }
            return tree;
        }
    }
    tree = expand(tree, ++(*depth));
    tree = insert(tree, depth, v);
    return tree;
}

Mound **new_tree()
{
    Mound **tree = malloc(sizeof(Mound *));
    tree[0] = newMNode();
    return tree;
}

bool isDirty(Mound *moundNode)
{
    return moundNode->dirty;
}

void swapMound(int l, int r, Mound **tree)
{
    Mound *temp = tree[l];
    tree[l] = tree[r];
    tree[r] = temp;
}

void moundify(Mound **tree, int index, int *depth)
{
    if (!isDirty(tree[index]))
    {
        return;
    }
    // is lead
    if (index >= (1 << *depth) - 1)
    {
        tree[index]->dirty = false;
        return;
    }
    else
    {
        // left and right child
        int l = 2 * index + 1, r = 2 * index + 2;
        if (isDirty(tree[l]))
        {
            moundify(tree, l, depth);
        }
        if (isDirty(tree[r]))
        {
            moundify(tree, r, depth);
        }
        int minIndex = val(tree[index]) <= val(tree[l]) ? index : val(tree[l]) <= val(tree[r]) ? l
                                                                                               : r;
        if (minIndex != index)
        {
            swapMound(index, minIndex, tree);
            // Original node would still be dirty.
            moundify(tree, minIndex, depth);
        }
        else
        {
            tree[index]->dirty = false;
        }
    }
}

int extractMin(Mound **tree, int *depth)
{
    int min_val = val(tree[0]);
    if (tree[0]->list)
    {
        struct LNode *temp = tree[0]->list;
        tree[0]->list = temp->next;
        free(temp);
    }
    tree[0]->dirty = true;
    moundify(tree, 0, depth);
}
// TODO : Read nodes from file
int main()
{
    srand(time(NULL));
    int depth = 0;
    Mound **tree = new_tree();
    tree = insert(tree, &depth, 2);
    tree = insert(tree, &depth, 5);
    tree = insert(tree, &depth, 6);
    tree = insert(tree, &depth, 11);
    tree = insert(tree, &depth, 12);
    tree = insert(tree, &depth, 13);
    tree = insert(tree, &depth, 14);
    tree = insert(tree, &depth, 1);
    tree = insert(tree, &depth, 0);
    tree = insert(tree, &depth, 3);

    for (int i = 0; i < (1 << (depth + 1)) - 1; i++)
    {
        printf("%d ", val(tree[i]));
    }
    extractMin(tree, &depth);
    printf("\n");
    for (int i = 0; i < (1 << (depth + 1)) - 1; i++)
    {
        printf("%d ", val(tree[i]));
    }
}
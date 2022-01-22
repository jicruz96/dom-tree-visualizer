#include "nary_trees.h"

/**
 * nary_tree_insert - inserts a node into an N-ary tree
 * @parent: pointer to parent node of new node
 * @content: content to be stored in new node
 * Return: pointer to the created node
 */
nary_tree_t *nary_tree_insert(nary_tree_t *parent, void *content)
{
	nary_tree_t *new = nary_tree_init(parent, content);

	if (new && parent)
	{
		parent->nb_children++;
		if (parent->children)
			new->next = parent->children;
		parent->children = new;
	}

	return (new);
}

/**
 * nary_tree_init - creates a new N-ary tree node
 *
 * @parent: pointer to parent for new node
 * @content: content to place in node (duplicate)
 * Return: pointer to new node or NULL if malloc fails
 */
nary_tree_t *nary_tree_init(nary_tree_t *parent, void *content)
{
	nary_tree_t *new = malloc(sizeof(nary_tree_t));

	if (new)
	{
		new->content = content;
		new->parent = parent;
		new->children = NULL;
		new->nb_children = 0;
		new->next = NULL;
	}

	return (new);
}

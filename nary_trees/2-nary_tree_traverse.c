#include "nary_trees.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

static size_t helper(
	nary_tree_t const *root,
	void (*action)(nary_tree_t const *node, size_t depth),
	size_t depth
);

/**
 * nary_tree_traverse - traverses an N-ary tree, executing the function
 *                      `action` at each node
 *
 * @root: pointer to root of tree
 * @action: action to perform on each node of tree
 * Return: max depth of tree
 */
size_t nary_tree_traverse(
	nary_tree_t const *root,
	void (*action)(nary_tree_t const *node, size_t depth)
)
{
	return (helper(root, action, 0));
}

static size_t helper(
	nary_tree_t const *root,
	void (*action)(nary_tree_t const *node, size_t depth),
	size_t depth
)
{
	size_t tmp_depth, max_depth = 0;
	nary_tree_t *tmp;

	if (!root)
		return (depth - 1);

	action(root, depth);

	for (tmp = root->children; tmp; tmp = tmp->next)
	{
		tmp_depth = helper(tmp, action, depth + 1);
		max_depth = max(tmp_depth, max_depth);
	}

	return (max(max_depth, depth));
}

/* Source file for the RING tree class */



/* Include files */

#include "RNAdts.h"



/* Public functions */

int 
RNInitTree()
{
    /* Return success */
    return TRUE;
}



void 
RNStopTree()
{
}


 
RNVTree::
RNVTree(int datasize, int headersize)
    : datasize(datasize),
      headersize(headersize),
      root(NULL),
      nnodes(0)
{
    // Check header size
    assert(headersize >= sizeof(RNTreeNodeHeader));
}


 
RNVTree::
RNVTree(const RNVTree& tree)
   : datasize(tree.datasize),
     headersize(tree.headersize),
     root(NULL),
     nnodes(0)
{
    // Copy nodes ???
    RNAbort("Not implemented yet");
}



RNVTree::
~RNVTree(void)
{
    // Remove all nodes
    Collapse();
}



const RNBoolean RNVTree::
IsAncestor(const RNTreeNode *node, const RNTreeNode *decendent) const
{
    // Return whether node is ancestor of candidate decendent
    const RNTreeNode *ancestor;
    for (ancestor = decendent; ancestor != NULL; ancestor = ParentNode(ancestor)) 
	if (ancestor == node) return TRUE;
    return FALSE;
}



const int RNVTree::
NChildren(const RNTreeNode *node) const
{
    // Return number of immediate children
    return (node->children) ? node->children->NEntries() : 0;
}



const int RNVTree::
NSiblingsBefore(const RNTreeNode *node) const
{
    // Return number of siblings to left of node in parent's child vector
    if (IsRoot(node)) return 0;
    RNTreeNode *parentnode = ParentNode(node);
    assert(parentnode);
    assert(parentnode->children);
    RNVectorEntry *entry = parentnode->children->FindEntryFromData(&node);
    assert(entry);
    int index = parentnode->children->EntryIndex(entry);
    assert((index >= 0) && (index < parentnode->children->NEntries()));
    return index;
}



const int RNVTree::
NDecendents(const RNTreeNode *node) const
{
    // Count this node
    int nnodes = 1;

    // Count decendent nodes
    if (IsInterior(node)) {
	RNTreeNode *child;
	RNIterator iterator;
	RN_FOR_EACH_TREE_NODE_CHILD(*this, node, child, iterator) {
	    nnodes += NDecendents(child);
	}
    }

    // Return total number of nodes under node (counting this one)
    return nnodes;
}



const int RNVTree::
NLeaves(const RNTreeNode *node) const
{
    // Check if node is a leaf
    if (IsLeaf(node)) return 1;

    // Count decendent leaves
    int nnodes = 0;
    RNTreeNode *child;
    RNIterator iterator;
    RN_FOR_EACH_TREE_NODE_CHILD(*this, node, child, iterator) 
	nnodes += NLeaves(child);
    return nnodes;
}



const int RNVTree::
MaxDepth(const RNTreeNode *node) const
{
    // Count decendent levels
    int maxdepth = 1;
    RNTreeNode *child;
    RNIterator iterator;
    RN_FOR_EACH_TREE_NODE_CHILD(*this, node, child, iterator) {
	int child_maxdepth = MaxDepth(child);
	if (maxdepth < (child_maxdepth+1)) maxdepth = child_maxdepth + 1;
    }
    return maxdepth;
}



const int RNVTree::
NAncestors(const RNTreeNode *node) const
{
    // Return depth of node in tree
    if (IsRoot(node)) return 0;
    else return (1 + NAncestors(ParentNode(node)));
}



RNTreeNode *RNVTree::
SiblingNode(const RNTreeNode *node, int k) const
{
    // Return sibling with index k in parent's children list
    if (IsRoot(node)) return NULL;
    else return ChildNode(ParentNode(node), k);
}



RNTreeNode *RNVTree::
ChildNode(const RNTreeNode *node, int k) const
{
    // Return pointer to Kth child node
    assert(node->children);
    return node->children->Kth(k);
}



RNTreeNode *RNVTree::
LeastCommonAncestor(const RNTreeNode *node1, const RNTreeNode *node2) const
{
    // Check all ancestors of node1 to see if they are ancestor of node2
    RNTreeNode *ancestor = (RNTreeNode *) node1;
    while (ancestor) {
	if (IsAncestor(ancestor, node2)) return ancestor;
        ancestor = ParentNode(ancestor);
    }

    // Should have at least found root
    RNAbort("Should never reach here");
    return NULL;
}



RNTreeNode *RNVTree::
FindNodeFromData(const void *data, RNTreeNode *node, RNCompareFunction *compare, void *appl) const
{
    // Check if node matches data
    if (compare) { if (!(*compare)(NodeData(node), data, appl)) return node; }
    else { if (!RNCompare(NodeData(node), data, DataSize())) return node; }

    // Otherwise, search children
    RNTreeNode *child;
    RNTreeNode *result;
    RNIterator iterator;
    RN_FOR_EACH_TREE_NODE_CHILD(*this, node, child, iterator) {
	if (result = FindNodeFromData(data, child, compare, appl)) return result;
    }       

    // Node *was not found
    return NULL;
}



RNTreeNode *RNVTree::
InternalInsert(const void *data, const void *header, RNTreeNode *parent)
{
    // Allocate memory for tree node 
    RNTreeNode *node = (RNTreeNode *) malloc(NodeSize());

    // Copy data into node
    if (DataSize() > 0) {
	if (data) RNCopy(data, NodeData(node), DataSize());
	else RNZero(NodeData(node), DataSize());
    }

    // Copy header into node
    if (HeaderSize() > 0) {
	if (header) RNCopy(header, node, HeaderSize());
	else {
	    RNZero(node, HeaderSize());
	    RNTreeNodeHeader default_header(parent);
	    RNCopy(&default_header, node, sizeof(RNTreeNodeHeader));
	}
    }

    // Check if first entry in tree
    if (nnodes == 0) {
        // Assign root
        assert(root == NULL);
        assert(parent == NULL);
        root = node;
    }
    else {
        // Just checking
        assert(root != NULL);

        // Tree already has entries
        if (parent == NULL) {
            // Make root a child of the inserted node
	    assert(root != NULL);
	    assert(root->parent == NULL);
            root->parent = node;
            node->children = new RNVector<RNTreeNode *>();
            node->children->Insert(root);
            root = node;
        }
        else {
   	    // Add node to parent's children
  	    assert(parent != NULL);
	    if (!(parent->children)) 
		parent->children = new RNVector<RNTreeNode *>();
            parent->children->Insert(node);
        }
    }

    // Increment number of nodes
    nnodes++;

    // Return node
    return node;
}



void RNVTree::
InternalRemove(RNTreeNode *node)
{
    // Check parameters
    assert(node);
    assert(nnodes > 0);

    // Remove all children 
    CollapseNode(node);

    // Check if node is root
    if (node != root) {
	// Remove node from parent's list
        RNTreeNode *parent = ParentNode(node);
	assert(parent);
	assert(parent->children);
        parent->children->Remove(node);
	if (parent->children->NEntries() == 0) {
	    delete parent->children;
            parent->children = NULL;
        }
    }
    else {
	root = NULL;
    }

    // Decrement the tree nnodes
    nnodes--;

    // Delete the node
    free(node);
}



void RNVTree::
Collapse(void)
{
    // Remove all nodes
    if (root) RemoveNode(root);
}



void RNVTree::
CollapseNode(RNTreeNode *node)
{
    // Remove all children nodes
    while (IsInterior(node)) {
	RemoveNode(ChildNode(node, 0));
    }
}



RNTreeNode *RNVTree::
SearchDepthFirst(RNTreeNode *start, RNCheckFunction *check, RNVisitFunction *visit, void *appl) 
{
    RNStack<RNTreeNode *> stack;
    RNTreeNode *node;
    RNTreeNode *child;
    RNIterator iterator;

    /* Check if start is OK */
    if ((check) && (!(check(NodeData(start), appl)))) return NULL;

    /* Add start node to stack */
    stack.Push(start);

    /* Repeatedly pop nodes from stack */
    while (stack.Pop(node)) {
 	/* Visit node */
	if ((visit) && (!(visit(NodeData(node), appl)))) break;

	/* Add node's children to stack */
	RN_FOR_EACH_TREE_NODE_CHILD(*this, node, child, iterator) {
	    /* Check if child is OK */
	    if ((check) && (!(check(NodeData(child), appl)))) continue;
	    
	    /* Add child to stack */
	    stack.Push(child);
	}
    }

    /* Return result node */
    return node;
}



RNTreeNode *RNVTree::
SearchBreadthFirst(RNTreeNode *start, RNCheckFunction *check, RNVisitFunction *visit, void *appl)
{
    RNQueue<RNTreeNode *> queue;
    RNTreeNode *node;
    RNTreeNode *child;
    RNIterator iterator;

    /* Check if start is OK */
    if ((check) && (!(check(NodeData(start), appl)))) return NULL;

    /* Add start node to queue */
    queue.Push(start);

    /* Repeatedly pop nodes from queue */
    while (queue.Pop(node)) {
 	/* Visit node */
	if ((visit) && (!(visit(NodeData(node), appl)))) break;

	/* Add node's children to queue */
    	RN_FOR_EACH_TREE_NODE_CHILD(*this, node, child, iterator) {
	    /* Check if child is OK */
	    if ((check) && (!(check(NodeData(child), appl)))) continue;
	    
	    /* Add child to queue */
	    queue.Push(child);
	}
    }

    /* Return result node */
    return node;
}



typedef struct RNCompareTreeNodesAppl {
    RNCompareFunction *compare;
    void *appl;
} RNCompareTreeNodesAppl;

static int RNCompareTreeNodes(const void *data1, const void *data2, void *voidappl)
{
    RNCompareTreeNodesAppl *appl = (RNCompareTreeNodesAppl *) voidappl;
    return (*(appl->compare))(*((void * const *) data1), *((void * const *) data2), appl->appl);
}



RNTreeNode *RNVTree::
SearchBestFirst(RNTreeNode *start, RNCheckFunction *check, RNVisitFunction *visit, RNCompareFunction *compare, void *appl)
{
    RNCompareTreeNodesAppl compareappl;
    compareappl.compare = compare;
    compareappl.appl = appl;
    RNHeap<RNTreeNode *> heap(RNCompareTreeNodes, &compareappl);
    RNTreeNode *node;
    RNTreeNode *child;
    RNIterator iterator;

    /* Check if start is OK */
    if ((check) && (!(check(NodeData(start), appl)))) return NULL;

    /* Add start node to heap */
    heap.Push(start);

    /* Repeatedly pop nodes from heap */
    while (heap.Pop(node)) {
 	/* Visit node */
	if ((visit) && (!(visit(NodeData(node), appl)))) break;

	/* Add node's children to heap */
    	RN_FOR_EACH_TREE_NODE_CHILD(*this, node, child, iterator) {
	    /* Check if child is OK */
	    if ((check) && (!(check(NodeData(child), appl)))) continue;
	    
	    /* Add child to heap */
	    heap.Push(child);
	}
    }

    /* Return result node */
    return node;
}



RNBoolean RNVTree::
IsValid(void) const
{
    // Check invariants
    assert(datasize > 0);
    assert(headersize >= sizeof(RNTreeNodeHeader));
    assert(nnodes >= 0);

    // Check tree
    if (nnodes == 0) {
	assert(root == NULL);
    }
    else {
	assert(root != NULL);
	assert(ParentNode(root) == NULL);
    }

    // Return success
    return TRUE;
}




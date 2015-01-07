/* Inline functions for RING tree class */



inline const RNBoolean RNVTree::
IsEmpty(void) const
{
    // Return whether tree is empty
    return (nnodes == 0);
}



inline const int RNVTree::
DataSize(void) const
{
    // Return size of data inside nodes in tree
    return datasize;
}



inline const int RNVTree::
HeaderSize(void) const
{
    // Return size of data inside nodes in tree
    return headersize;
}



inline const int RNVTree::
NodeSize(void) const
{
    // Return size of nodes in tree
    return (DataSize() + HeaderSize());
}



inline const int RNVTree::
NAllocated(void) const
{
    // Return number of nodes allocated memory for
    return nnodes;
}



inline const int RNVTree::
NNodes(void) const
{
    // Return number of nodes in tree
    return nnodes;
}



inline const int RNVTree::
NLeaves(void) const
{
    // Return number of leaf nodes in tree
    return ((root) ? NLeaves(root) : 0);
}



inline const int RNVTree::
NInterior(void) const
{
    // Return number of interior nodes in tree
    return (NNodes() - NLeaves());
}



inline const int RNVTree::
MaxDepth(void) const
{
    // Return number of leaf nodes in tree
    return ((root) ? MaxDepth(root) : 0);
}



inline const RNTreeNodeHeader *RNVTree::
NodeHeader(const RNTreeNode *node) const
{
    // Return pointer to header part of node
    return (const RNTreeNodeHeader *) node;
}



inline RNTreeNodeHeader *RNVTree::
NodeHeader(RNTreeNode *node) const
{
    // Return pointer to header part of node
    return (RNTreeNodeHeader *) node;
}



inline void *RNVTree::
NodeData(RNTreeNode *node) const
{
    // Return pointer to data part of node
    return (void *) ((char *) node + HeaderSize());
}



inline const int RNVTree::
NSiblings(const RNTreeNode *node) const
{
    // Return number of sibling nodes
    return (node->parent) ? (NChildren(node->parent) - 1) : 0;
}



inline const int RNVTree::
NSiblingsAfter(const RNTreeNode *node) const
{
    // Return number of siblings to right of node
    return (node->parent) ? (NSiblings(node) - NSiblingsBefore(node)) : 0;
}



inline const RNBoolean RNVTree::
IsRoot(const RNTreeNode *node) const
{
    // Return whether node is root of tree
    return ((node) && (node == root));
}



inline const RNBoolean RNVTree::
IsInterior(const RNTreeNode *node) const
{
    // Return whether node has decendents
    return (NChildren(node) > 0);
}



inline const RNBoolean RNVTree::
IsLeaf(const RNTreeNode *node) const
{
    // Return whether node is leaf
    return (!IsInterior(node));
}



inline const RNBoolean RNVTree::
IsDecendent(const RNTreeNode *node, const RNTreeNode *ancestor) const
{
    // Return whether node is decendent of candidate ancestor
    return IsAncestor(ancestor, node);
}



inline const int RNVTree::
Depth(const RNTreeNode *node) const
{
    // Return number of ancestors
    return NAncestors(node);
}



inline RNTreeNode *RNVTree::
RootNode(void) const
{
    // Return pointer to root node
    return root;
}



inline RNTreeNode *RNVTree::
ParentNode(const RNTreeNode *node) const
{
    // Return pointer to parent node
    return node->parent;
}



inline RNTreeNode *RNVTree::
FindNodeFromData(const void *data, RNCompareFunction *compare, void *appl) const
{
    // Search for node recursively starting at root
    return (root) ? FindNodeFromData(data, root, compare, appl) : NULL;
}



inline RNTreeNode *RNVTree::
InsertData(const void *data, RNTreeNode *parent)
{
    // Insert data into tree
    return InternalInsert(data, NULL, parent);
}



inline void RNVTree::
RemoveNode(RNTreeNode *node)
{
    // Remove node from tree
    InternalRemove(node);
}



inline void RNVTree::
RemoveData(const void *data)
{
    // Remove data from tree
    RemoveNode(FindNodeFromData(data));
}




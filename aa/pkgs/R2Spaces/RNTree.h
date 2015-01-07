/* Include file for the RING tree class */

#ifndef __RN__TREE__H__
#define __RN__TREE__H__



/* Library initialization functions */

int RNInitTree();
void RNStopTree();



/* Node *definition */

typedef class RNTreeNodeHeader {
    friend class RNVTree;
    protected:
        RNTreeNodeHeader(RNTreeNodeHeader *parent = NULL) 
	    : parent(parent), children(NULL) {};
        RNTreeNodeHeader *parent;
        RNArray<RNTreeNodeHeader *> *children;
} RNTreeNode;



/* Class definition */

class RNVTree {
    public:
        // Constructor functions
        RNVTree(int datasize, int headersize = sizeof(RNTreeNodeHeader));
        RNVTree(const RNVTree& tree);
	~RNVTree(void);

        // Tree property functions/operators
	const RNBoolean IsEmpty(void) const;
        const int DataSize(void) const;
        const int HeaderSize(void) const;
        const int NodeSize(void) const;
	const int NAllocated(void) const;
	const int NNodes(void) const;
	const int NLeaves(void) const;
	const int NInterior(void) const;
	const int MaxDepth(void) const;

        // Node *property functions/operators
	const RNBoolean IsRoot(const RNTreeNode *node) const;
	const RNBoolean IsLeaf(const RNTreeNode *node) const;
	const RNBoolean IsInterior(const RNTreeNode *node) const;
	const RNBoolean IsAncestor(const RNTreeNode *node, const RNTreeNode *decendent) const;
	const RNBoolean IsDecendent(const RNTreeNode *node, const RNTreeNode *ancestor) const;
	const int NChildren(const RNTreeNode *node) const;
	const int NSiblings(const RNTreeNode *node) const;
	const int NSiblingsBefore(const RNTreeNode *node) const;
	const int NSiblingsAfter(const RNTreeNode *node) const;
	const int NDecendents(const RNTreeNode *node) const;
	const int NAncestors(const RNTreeNode *node) const;
	const int NLeaves(const RNTreeNode *node) const;
	const int Depth(const RNTreeNode *node) const;
	const int MaxDepth(const RNTreeNode *node) const;
	const RNTreeNodeHeader *NodeHeader(const RNTreeNode *node) const;
	RNTreeNodeHeader *NodeHeader(RNTreeNode *node) const;
	void *NodeData(RNTreeNode *node) const;

        // Node *access functions/operators
	RNTreeNode *RootNode(void) const;
	RNTreeNode *ParentNode(const RNTreeNode *node) const;
	RNTreeNode *SiblingNode(const RNTreeNode *node, int k) const;
	RNTreeNode *ChildNode(const RNTreeNode *node, int k) const;
	RNTreeNode *LeastCommonAncestor(const RNTreeNode *node1, const RNTreeNode *node2) const;
	RNTreeNode *FindNodeFromData(const void *data, RNCompareFunction *compare = NULL, void *appl = NULL) const;
	RNTreeNode *FindNodeFromData(const void *data, RNTreeNode *node, RNCompareFunction *compare = NULL, void *appl = NULL) const;

	// Insertion/removal functions/operators
	RNTreeNode *InsertData(const void *data, RNTreeNode *parent);
	void RemoveNode(RNTreeNode *node);
	void RemoveData(const void *data);

        // Manipulation functions/operators
	virtual void Collapse(void);
	virtual void CollapseNode(RNTreeNode *node);

	// Traversal functions/operators
        RNTreeNode *SearchDepthFirst(RNTreeNode *start, RNCheckFunction *check, RNVisitFunction *visit, void *appl = NULL);
        RNTreeNode *SearchBreadthFirst(RNTreeNode *start, RNCheckFunction *check, RNVisitFunction *visit, void *appl = NULL);
        RNTreeNode *SearchBestFirst(RNTreeNode *start, RNCheckFunction *check, RNVisitFunction *visit, RNCompareFunction *compare, void *appl = NULL);

	// Debug function
        RNBoolean IsValid(void) const;

    public:
	// Do not use these
	virtual RNTreeNode *InternalInsert(const void *data, const void *header, RNTreeNode *parent);
	virtual void InternalRemove(RNTreeNode *node);

    private:
        int datasize;
        int headersize;
	RNTreeNode *root;
	int nnodes;
};



/* Inline functions */

#include "RNTree.I"





/* Template definition */

template <class DataClass>
class RNTree : public RNVTree {
    public:
        // Constructor functions
        RNTree(void) : RNVTree(sizeof(DataClass)) {};
        RNTree(const RNTree<DataClass>& src) : RNVTree(src) {};
	RN_TREE_DATA_CLASS_MEMBER_DEFINITIONS(RNVTree, RNTree, RNTreeNode *, RNTreeNode *, DataClass);
};



/* Utility macros */

#define RN_FOR_EACH_TREE_LEAF_NODE(__tree, __node, __iterator) \
    (__iterator).i = 0; \
    RNStack<RNTreeNode *> RNtree_search_stack; \
    RNtree_search_stack.Push((__tree).RootNode()); \
    while (RNtree_search_stack.Pop(__node)) \
	if ((__tree).IsInterior(__node)) { \
            for (int i = 0; i < (__tree).NChildren(__node); i++) \
                RNtree_search_stack.Push((__tree).ChildNode((__node), i)); \
	} \
	else 

#define RN_FOR_EACH_TREE_NODE_CHILD(__tree, __node, __child, __iterator) \
    for ((__iterator).i = 0; (__iterator).i < (__tree).NChildren(__node); (__iterator).i++) \
        if ((__child) = (__tree).ChildNode((__node), (__iterator).i))



#endif



/* Include file for the R2 tree class */
/* Same as RNVTree, except "node" -> "cell" */



/* Library initialization functions */

int R2InitTree();
void R2StopTree();



/* Node *definition */

typedef RNTreeNodeHeader R2TreeCellHeader;
typedef RNTreeNode R2TreeCell;



/* Class definition */

class R2VTree : private RNVTree {
    public:
        // Constructor functions
        R2VTree(int datasize, int headersize = sizeof(RNTreeNodeHeader)) : RNVTree(datasize, headersize) {};
        R2VTree(const RNVTree& tree) : RNVTree(tree) {};

        // Tree property functions/operators
	RNVTree::IsEmpty;
        const int CellDataSize(void) const { return RNVTree::DataSize(); };
        const int CellHeaderSize(void) const { return RNVTree::HeaderSize(); };
        const int CellSize(void) const { return RNVTree::NodeSize(); };
	RNVTree::NAllocated;
	const int NCells(void) const { return RNVTree::NNodes(); };
	// RNVTree::NLeaves;  // SGI compiler can't handle access declaration for overloaded
	const int NLeaves(void) const { return RNVTree::NLeaves(); };
	RNVTree::NInterior;

        // Node *property functions/operators
	RNVTree::IsRoot;
	RNVTree::IsLeaf;
	RNVTree::IsInterior;
	RNVTree::IsAncestor;
	RNVTree::IsDecendent;
	RNVTree::NChildren;
	RNVTree::NSiblings;
	RNVTree::NSiblingsBefore;
	RNVTree::NSiblingsAfter;
	RNVTree::NDecendents;
	RNVTree::NAncestors;
	// RNVTree::NLeaves;  // SGI compiler can't handle access declaration for overloaded
        const int NLeaves(const R2TreeCell *cell) const { return RNVTree::NLeaves(cell); };
	RNVTree::Depth;
	const R2TreeCellHeader *CellHeader(const R2TreeCell *cell) const { return RNVTree::NodeHeader(cell); };
	R2TreeCellHeader *CellHeader(R2TreeCell *cell) const { return RNVTree::NodeHeader(cell); };
	void *CellData(R2TreeCell *cell) const { return RNVTree::NodeData(cell); };

        // Node *access functions/operators
	R2TreeCell *RootCell(void) const { return RNVTree::RootNode(); };
	R2TreeCell *ParentCell(const R2TreeCell *cell) const { return RNVTree::ParentNode(cell); };
	R2TreeCell *SiblingCell(const R2TreeCell *cell, int k) const { return RNVTree::SiblingNode(cell, k); };
	R2TreeCell *ChildCell(const R2TreeCell *cell, int k) const { return RNVTree::ChildNode(cell, k); };
	RNVTree::LeastCommonAncestor;
	R2TreeCell *FindCellFromData(const void *data, RNCompareFunction *compare = NULL, void *appl = NULL) const 
	    { return RNVTree::FindNodeFromData(data, compare, appl); };
	R2TreeCell *FindCellFromData(const void *data, R2TreeCell *cell, RNCompareFunction *compare = NULL, void *appl = NULL) const 
	    { return RNVTree::FindNodeFromData(data, cell, compare, appl); };

	// Insertion/removal functions/operators
	RNVTree::InsertData;
	void RemoveCell(R2TreeCell *cell) { RNVTree::RemoveNode(cell); };
	RNVTree::RemoveData;

        // Manipulation functions/operators
	RNVTree::Collapse;
	void CollapseCell(R2TreeCell *cell) { RNVTree::CollapseNode(cell); };

	// Traversal functions/operators
        RNVTree::SearchDepthFirst;
        RNVTree::SearchBreadthFirst;
        RNVTree::SearchBestFirst;

	// Debug function
        RNVTree::IsValid;

    public:
	// Do not use these
	RNVTree::InternalInsert;
	RNVTree::InternalRemove;
};



/* Utility macros */

#define R2_FOR_EACH_TREE_LEAF_CELL(__tree, __cell, __iterator) \
    (__iterator).i = 0; \
    RNStack<R2TreeCell *> R2tree_search_stack; \
    R2tree_search_stack.Push((__tree).RootCell()); \
    while (R2tree_search_stack.Pop(__cell)) \
	if ((__tree).IsInterior(__cell)) { \
            for (int i = 0; i < (__tree).NChildren(__cell); i++) \
                R2tree_search_stack.Push((__tree).ChildCell((__cell), i)); \
	} \
	else 

#define R2_FOR_EACH_TREE_CELL_CHILD(__tree, __cell, __child, __iterator) \
    for ((__iterator).i = 0; (__iterator).i < (__tree).NChildren(__cell); (__iterator).i++) \
        if ((__child) = (__tree).ChildCell((__cell), (__iterator).i))



/* Template macros */

#define R2_TREE_DATA_CLASS_MEMBER_DEFINITIONS(BaseTreeClass, TreeClass, BaseCellClass, CellClass, DataClass) \
    CellClass FindCell(const DataClass& data, RNCompareFunction *compare = NULL, void *appl = NULL) const \
	{ return (CellClass) FindCellFromData((const void *) &data, compare, appl); }; \
    CellClass FindCell(const DataClass& data, BaseCellClass cell, RNCompareFunction *compare = NULL, void *appl = NULL) const \
	{ return (CellClass) FindCellFromData((const void *) &data, cell, compare, appl); }; \
    DataClass *CellData(BaseCellClass cell) const \
        { return (DataClass *) BaseTreeClass::CellData(cell); }; \
    DataClass *RootData(void) const \
        { return CellData(RootCell()); }; \
    DataClass *ParentData(const BaseCellClass cell) const \
        { return CellData(ParentCell(cell)); }; \
    DataClass *ChildData(const BaseCellClass cell, int k) const \
        { return CellData(ChildCell(cell, k)); }; \
    DataClass& CellContents(BaseCellClass cell) const \
        { return *((DataClass *) CellData(cell)); }; \
    DataClass& Root(void) const \
        { return CellContents(RootCell()); }; \
    DataClass& Parent(const BaseCellClass cell) const \
        { return CellContents(ParentCell(cell)); }; \
    DataClass& Child(const BaseCellClass cell, int k) const \
        { return CellContents(ChildCell(cell, k)); }; \
    CellClass Insert(const DataClass& data, BaseCellClass parent) \
        { return InsertData((const void *) &data, parent); }; \
    void Remove(const DataClass& data) \
        { RemoveData((const void *) &data); }


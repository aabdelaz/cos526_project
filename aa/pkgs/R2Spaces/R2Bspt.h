/* Include file for the R2 bsptree class */



/* Initialization functions */

int R2InitBspTree();
void R2StopBspTree();



/* Cell *definition */

typedef R2TreeCell R2BspTreeCell;
class R2BspTreeCellHeader : public R2TreeCellHeader {
    friend class R2VBspTree;
    protected:
        R2BspTreeCellHeader(R2BspTreeCell *parent, const R2Line& split)
	    : R2TreeCellHeader(parent), split(split) {};
        R2Line split;
};




/* Class definition */

class R2VBspTree : public R2VTree {
    public:
        // Constructor functions
        R2VBspTree(int datasize, int headersize = sizeof(R2BspTreeCellHeader));

        // Cell *property functions/operators
	const R2Line& CellSplitLine(const R2BspTreeCell *cell) const;
	const R2BspTreeCellHeader *CellHeader(const R2BspTreeCell *cell) const;
	R2BspTreeCellHeader *CellHeader(R2BspTreeCell *cell) const;

        // Cell *access functions/operators
        R2BspTreeCell *FindCell(const R2Point& point) const;
        R2BspTreeCell *FindCell(R2BspTreeCell *cell, const R2Point& point) const;

	// Insertion/removal functions/operations
	R2BspTreeCell *InsertData(const void *data, R2BspTreeCell *parent);
	R2BspTreeCell *InsertData(const void *data, R2BspTreeCell *parent, const R2Line& line);

	// Manipulation functions/operations
        virtual void Split(const R2Span& span);
        virtual void SplitCell(R2BspTreeCell *cell, const R2Span& span);
        virtual void SplitCell(R2BspTreeCell *cell, const R2Line& split);

	// Draw functions/operations
	void Outline(void) const;
	void OutlineCell(const R2BspTreeCell *cell) const;

    public:
	// Do not use these
	virtual R2BspTreeCell *InternalInsert(const void *data, const void *header, R2BspTreeCell *parent);
	virtual R2BspTreeCell *InternalInsert(const void *data, const void *header, R2BspTreeCell *parent, const R2Line& split);
};



/* Public functions */

RNBoolean R2Contains(const R2VBspTree& bsptree, const R2BspTreeCell *cell, const R2Shape& shape);
RNClassID R2Intersects(const R2VBspTree& bsptree, const R2BspTreeCell *cell, const R2Line& line, R2Span *result = NULL);



/* Inline functions */

#include "R2Spaces/R2Bspt.I"



template <class DataClass>
class R2BspTree : public R2VBspTree {
    public:
        // Constructor functions
        R2BspTree(void) : R2VBspTree(sizeof(DataClass)) {};
	R2_TREE_DATA_CLASS_MEMBER_DEFINITIONS(R2VTree, R2BspTree, R2TreeCell *, R2BspTreeCell *, DataClass);

	// Cell *access functions/operators
        R2BspTreeCell *FindCell(const R2Point& point) const
	    { return R2VBspTree::FindCell(point); };
        R2BspTreeCell *FindCell(R2BspTreeCell *cell, const R2Point& point) const
	    { return R2VBspTree::FindCell(cell, point); };
};



/* Utility macros */

#define R2_FOR_EACH_BSPTREE_LEAF_CELL(__bsptree, __cell, __iterator) \
    R2_FOR_EACH_TREE_LEAF_CELL(__bsptree, __cell, __iterator) 

#define R2_FOR_EACH_BSPTREE_LEAF_CELL_INTERSECTING_SHAPE(__bsptree, __cell, __shape, __iterator) \
    (__iterator).i = 0; \
    RNStack<R2BspTreeCell *> R2bsptree_search_stack; \
    R2bsptree_search_stack.Push((__bsptree).RootCell()); \
    while (R2bsptree_search_stack.Pop(__cell)) \
	if ((__bsptree).IsInterior(__cell)) { \
            const R2Line *split = &((__bsptree).CellSplitLine(__cell)); \
            const R2Line backsplit = -(*split); \
	    if (R2Contains(*split, (__shape)) || R2Contains(backsplit, (__shape))) { \
		R2bsptree_search_stack.Push((__bsptree).ChildCell((__cell), 0)); \
		R2bsptree_search_stack.Push((__bsptree).ChildCell((__cell), 1)); \
	    } \
            else { \
	        if (!R2Contains(R2Halfspace(*split, 0), (__shape))) \
		    R2bsptree_search_stack.Insert((__bsptree).ChildCell((__cell), 0)); \
	        if (!R2Contains(R2Halfspace(backsplit, 0), (__shape))) \
		    R2bsptree_search_stack.Insert((__bsptree).ChildCell((__cell), 1)); \
	    } \
	} \
	else 

#define R2_FOR_EACH_BSPTREE_LEAF_CELL_INTERSECTING_BOX(__bsptree, __root, __cell, __box) \
    R2_FOR_EACH_BSPTREE_LEAF_CELL_INTERSECTING_SHAPE(__bsptree, __root, __cell, __box)

#define R2_FOR_EACH_BSPTREE_CELL_CHILD(__bsptree, __cell, __child, __iterator) \
    R2_FOR_EACH_TREE_CELL_CHILD(__bsptree, __cell, __child, __iterator)




/* Source file for the R2 bsptree class */



/* Include files */

#include "R2Spaces/R2Spaces.h"



/* Public functions */

int 
R2InitBspTree()
{
    /* Return success */
    return TRUE;
}



void 
R2StopBspTree()
{
}



R2VBspTree::
R2VBspTree(int datasize, int headersize)
    : R2VTree(datasize, headersize)
{
    // Create root cell
    // If headersize is different, assume another constructor will insert root
    if (IsEmpty() && (headersize == sizeof(R2BspTreeCellHeader)))
	InsertData(NULL, NULL);
}



R2BspTreeCell *R2VBspTree::
FindCell(R2BspTreeCell *cell, const R2Point& point) const
{
    // Return bsptree cell containing point
    if (IsInterior(cell)) {
	// Recurse to appropriate child cell
	int k = (R2SignedDistance(CellSplitLine(cell), point) < 0.0) ? 0 : 1;
	return FindCell(ChildCell(cell, k), point);
    }
    else {
	// Return leaf cell
	return cell;
    }
}



R2BspTreeCell *R2VBspTree::
InternalInsert(const void *data, const void *header, R2BspTreeCell *parent)
{
    // Insert cell
    return InternalInsert(data, header, parent, R2null_line);
}



R2BspTreeCell *R2VBspTree::
InternalInsert(const void *data, const void *header, R2BspTreeCell *parent, const R2Line& split)
{
    // Insert cell with proper header
    if (!header) {
	assert(CellHeaderSize() == sizeof(R2BspTreeCellHeader));
	R2BspTreeCellHeader cellheader(parent, split);
	header = (void *) &cellheader;

	// Insert cell
	return R2VTree::InternalInsert(data, header, parent);
    }
    else {
	// Insert cell
	return R2VTree::InternalInsert(data, header, parent);
    }
}



void R2VBspTree::
Split(const R2Span& span)
{
    // Split tree by span
    SplitCell(RootCell(), span);
}



void R2VBspTree::
SplitCell(R2BspTreeCell *cell, const R2Span& span)
{
    // Split all leaf cells intersecting span 
    if (IsInterior(cell)) {
	// Compute relationship between split line and span
	int split_result = R2Splits(CellSplitLine(cell), span);

	// Split appropriate child cells recursively
	if ((split_result == RN_BELOW) || (split_result == RN_CROSSING)) 
	    SplitCell(ChildCell(cell, 0), span);
	if ((split_result == RN_ABOVE) || (split_result == RN_CROSSING)) 
	    SplitCell(ChildCell(cell, 1), span);
    }
    else {
	// Split leaf cell on line of span
	SplitCell(cell, span.Line());
    }
}



void R2VBspTree::
SplitCell(R2BspTreeCell *cell, const R2Line& split)
{
    // Check if the cell is a leaf
    assert(IsLeaf(cell));

    // Set cell split
    CellHeader(cell)->split = split;

    // Create two children below split cell
    InsertData(NULL, cell);
    InsertData(NULL, cell);
}



void R2VBspTree::
OutlineCell(const R2BspTreeCell *cell) const
{
    // Outline leaf cells
    if (IsInterior(cell)) {
	// Recurse to both children cells
	OutlineCell(ChildCell(cell, 0));
	OutlineCell(ChildCell(cell, 1));
    }
    else {
	const R2BspTreeCell *ancestor = cell;
	while (ancestor = ParentCell(ancestor)) {
	    // Create span along split of ancestor
	    R2Span boundary;
	    if (!R2Intersects(*this, cell, CellSplitLine(ancestor), &boundary)) 
	        continue;

	    // Draw boundary span ??? Temporary ???
	    if (boundary.Length() < (0.5 * RN_INFINITY)) 
		boundary.Draw();
	}
    }
}



RNBoolean 
R2Contains(const R2VBspTree& bsptree, const R2BspTreeCell *cell, const R2Shape& shape)
{
    // Check whether cell is root
    if (bsptree.IsRoot(cell)) return TRUE;

    // Check whether shape is inside halfspaces defining cell
    R2BspTreeCell *parent = bsptree.ParentCell(cell);
    R2Halfspace halfspace(bsptree.CellSplitLine(parent), 0);
    if (bsptree.NSiblingsBefore(cell) == 0) halfspace.Flip();
    if (!R2Contains(halfspace, shape)) return FALSE;

    // Recursively check all ancestor cells
    return R2Contains(bsptree, parent, shape);
}



RNClassID
R2Intersects(const R2VBspTree& bsptree, const R2BspTreeCell *cell, const R2Line& line, R2Span *result) 
{
#if FALSE
    // Create infinite span in line
    R2Span span = R2InfiniteSpan(line);

    // Start clipping span at split line of parent cell
    const R2BspTreeCell *child = cell;
    const R2BspTreeCell *parent = bsptree.ParentCell(cell);

    // Chop span along split lines of ancestors
    while ((parent) && (span.NVertices() > 0)) {
	// Check if split line can possibly clip span
        // ??? Look at this ???
	const R2Line *split = &(bsptree.CellSplitLine(parent));
	if (!R2Contains(*split, line)) {
	    int k = bsptree.NSiblingsBefore(child);
	    if (k == 0) span.Clip(-(*split));
	    else span.Clip(*split);
	}

	// Move up to next ancestor entry in bsptree
	child = parent;
	parent = bsptree.ParentCell(parent);
    }

    // Garbage collection of points ???
    // ??? There is a memory leak here as points are created
    // ??? during clip, and then not deleted when they are
    // ??? subsequently clipped

    // Check whether line cut through cell 
    if (span.NVertices() == 0) return RN_NULL_CLASS_ID;

    // Copy result and return success
    if (result) *result = span;
    return R2_SPAN_CLASS_ID;
#else
    RNAbort("Not implemented");
    return RN_NULL_CLASS_ID;
#endif
}









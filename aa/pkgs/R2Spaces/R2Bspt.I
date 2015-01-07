/* Inline functions for R2 bsptree class */



inline const R2BspTreeCellHeader *R2VBspTree::
CellHeader(const R2BspTreeCell *cell) const
{
    // Return header part of bsptree cell
    return (const R2BspTreeCellHeader *) R2VTree::CellHeader(cell);
}



inline R2BspTreeCellHeader *R2VBspTree::
CellHeader(R2BspTreeCell *cell) const
{
    // Return header part of bsptree cell
    return (R2BspTreeCellHeader *) R2VTree::CellHeader(cell);
}



inline const R2Line& R2VBspTree::
CellSplitLine(const R2BspTreeCell *cell) const
{
    // Return line of split at interior cell
    return CellHeader(cell)->split;
}



inline R2BspTreeCell *R2VBspTree::
FindCell(const R2Point& point) const
{
    // Return bsptree cell containing point
    assert(RootCell());
    return FindCell(RootCell(), point);
}



inline R2BspTreeCell *R2VBspTree::
InsertData(const void *data, R2BspTreeCell *parent)
{
    // Insert data into tree
    return InternalInsert(data, NULL, parent, R2null_line);
}



inline R2BspTreeCell *R2VBspTree::
InsertData(const void *data, R2BspTreeCell *parent, const R2Line& split)
{
    // Insert cell into tree
    return InternalInsert(data, NULL, parent, split);
}



inline void R2VBspTree::
Outline(void) const
{
    // Outline bsptree bsp hierarchy
    assert(RootCell());
    OutlineCell(RootCell());
}




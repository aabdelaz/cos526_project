/* Source file for the RING list class */



/* Include files */

#include "RNAdts.h"



/* Public functions */

int 
RNInitList()
{
    /* Return success */
    return TRUE;
}



void 
RNStopList()
{
}


 
RNVList::
RNVList(int datasize, int headersize)
    : datasize(datasize),
      headersize(headersize),
      head(NULL),
      tail(NULL),
      nentries(0)
{
    // Check arguments
    assert(datasize > 0);
    assert(headersize >= sizeof(RNListEntryHeader));
}



RNVList::
RNVList(const RNVList& list)
    : datasize(list.datasize),
      headersize(list.headersize),
      head(NULL),
      tail(NULL),
      nentries(0)
{
    // Copy list
    *this = list;
}



RNVList::
~RNVList(void)
{
    // Remove all entries
    Empty();
}



RNListEntry *RNVList::
KthEntry(int k) const
{
    // Search for entry in list
    int i = 0;
    RNListEntry *entry;
    RNIterator iterator;
    RN_FOR_EACH_LIST_ENTRY(*this, entry, iterator) {
	if ((i++) == k) return entry;
    }

    // Entry *was not found
    return NULL;
}




RNListEntry *RNVList::
FindEntryFromData(const void *data, RNCompareFunction *compare, void *appl) const
{
    // Search for entry in list
    RNListEntry *entry;
    RNIterator iterator;
    RN_FOR_EACH_LIST_ENTRY(*this, entry, iterator) {
        if (compare) { if (!((*compare)(EntryData(entry), data, appl))) return entry; }
	else { if (!RNCompare(EntryData(entry), data, DataSize())) return entry; }
    }

    // Entry *was not found
    return NULL;
}



const int RNVList::
EntryIndex(const RNListEntry *entry) const
{
    // Search for entry in list
    int i = 0;
    RNListEntry *tmp;
    RNIterator iterator;
    RN_FOR_EACH_LIST_ENTRY(*this, tmp, iterator) {
	if (entry == tmp) return i;
	i++;
    }

    // Entry *was not found
    return -1;
}



RNListEntry *RNVList::
InternalInsert(const void *data, const void *header, RNListEntry *before)
{
    // Allocate memory for list entry 
    RNListEntry *entry = (RNListEntry *) malloc(EntrySize());

    // Copy data into entry
    if (DataSize() > 0) {
	if (data) RNCopy(data, EntryData(entry), DataSize());
	else RNZero(EntryData(entry), DataSize());
    }

    // Copy header into entry
    if (HeaderSize() > 0) {
	if (header) RNCopy(header, entry, HeaderSize());
	else {
	    RNZero(entry, HeaderSize());
	    RNListEntryHeader default_header;
	    RNCopy(&default_header, entry, sizeof(RNListEntryHeader));
	}
    }

    // Check if first entry in list
    if (nentries == 0) {
        // Just checking
        assert(head == NULL);
        assert(tail == NULL);
        assert(before == NULL);

        // Create one entry list
        head = tail = entry;
        entry->prev = entry->next = NULL;
    }
    else {
        // Just checking
        assert(head != NULL);
        assert(tail != NULL);

        // List already has entries
        if (before == NULL) {
            // Inserting at tail
	    assert(tail != NULL);
	    assert(tail->next == NULL);
            entry->next = NULL;
            entry->prev = tail;
            tail->next = entry;
            tail = entry;
        }
        else {
            // Check if inserting before head 
	    if (before == head) {
                // Inserting before head
                assert(before->prev == NULL);
	    	entry->prev = NULL;
            	entry->next = head;
            	before->prev = entry;
            	head = entry;            
            }
            else {
                // Inserting in middle
                assert(before->prev != NULL);
                entry->next = before;
                entry->prev = before->prev;
                before->prev->next = entry;
                before->prev = entry;
            }
        }
    }

    // Increment number of entries
    nentries++;

    // Return entry
    return entry;
}



void RNVList::
InternalRemove(RNListEntry *entry)
{
    // Check parameters
    assert(entry);
    assert(nentries > 0);

    // Check list nentries
    if (nentries == 1) {
        // Just checking
	assert(entry == head);
        assert(head == tail);
        head = tail = NULL;
    }
    else {
        // Just checking
        assert(head != tail);
	
        // Check where in list we are removing entry
	if (entry == head) {
            // Just checking 
            assert(entry->prev == NULL);
            assert(entry->next != NULL);

            // Remove head entry
            head = entry->next;
            head->prev = NULL;
        }
	else if (entry == tail) {
            // Just checking 
            assert(entry->next == NULL);
            assert(entry->prev != NULL);

            // Remove tail entry
            tail = entry->prev;
            tail->next = NULL;
        }
	else {
            // Just checking 
            assert(entry->next);
            assert(entry->next->prev == entry);
            assert(entry->prev);
            assert(entry->prev->next == entry);

            // Remove middle entry
            entry->prev->next = entry->next;
            entry->next->prev = entry->prev;
        }
    }

    // Free memory for list entry 
    free(entry);

    // Decrement the list nentries
    nentries--;
}



void *RNVList::
PopData(void *data)
{
    // Check if there are any entries 
    if (IsEmpty()) {
	return NULL;
    }
    else {
        // Remove and return data from head of list
        RNCopy(EntryData(HeadEntry()), data, DataSize());
        RemoveHead();
        return data;
    }
}



void RNVList::
Empty(void)
{
    // Remove all entries
    RNListEntry *entry = head;
    while (entry != NULL) {
	RNListEntry *next = entry->next;
        RemoveEntry(entry);
        entry = next;
    }
}



void RNVList::
Shift(int delta)
{
    // Shift all entries by delta
    Shift(0, 0, delta);
}



void RNVList::
Shift(int start, int length, int delta)
{
    /* Compute number of entries to shift */
    if ((delta < 0) && (start < -delta)) start = -delta;
    int nshift = nentries - start;
    if (delta > 0) nshift -= delta;
    if (nshift <= 0) return;
    if ((length > 0) && (length < nshift)) nshift = length;

    // Shift list entries ???
    RNAbort("Not Implemented");
}



void RNVList::
Reverse(void)
{
    // Reverse order of all entries
    Reverse(0, 0);
}



void RNVList::
Reverse(int start, int nentries)
{
    /* Compute number of entries to reverse */
    int nreverse = nentries - start;
    if (nreverse <= 0) return;
    if ((nentries > 0) && (nentries < nreverse)) nreverse = nentries;

    // Reverse list entries ???
    RNAbort("Not implemented");
}



RNVList& RNVList::
operator=(const RNVList& list)
{
    // Empty list 
    Empty();

    // Copy entries 
    RNListEntry *listentry;
    RNIterator iterator;
    RN_FOR_EACH_LIST_ENTRY(list, listentry, iterator) {
	RNListEntry *entry = InsertData(NULL);
	RNCopy(listentry, entry, EntrySize());
    }

    // Return list
    return *this;
}



RNBoolean RNVList::
IsValid(void) const
{
    // Check invariants
    assert(datasize > 0);
    assert(headersize >= sizeof(RNTreeNodeHeader));
    assert(nentries >= 0);

    // Check list
    if (nentries == 0) {
	assert(head == NULL);
	assert(tail == NULL);
    }
    else {
	assert(head != NULL);
	assert(tail != NULL);
	assert(PrevEntry(head) == NULL);
	assert(NextEntry(tail) == NULL);
    }

    // Check entries
    RNListEntry *entry;
    RNIterator iterator;
    int entrycount = 0;
    RN_FOR_EACH_LIST_ENTRY(*this, entry, iterator) {
	assert(entry);
	int index = EntryIndex(entry);
	assert((0 <= index) && (index < nentries));
	if (PrevEntry(entry) == NULL) assert(entry == head);
	else assert(NextEntry(PrevEntry(entry)) == entry);
	if (NextEntry(entry) == NULL) assert(entry == tail);
	else assert(PrevEntry(NextEntry(entry)) == entry);
	entrycount++;
    }
    assert(entrycount == nentries);

    // Return success
    return TRUE;
}




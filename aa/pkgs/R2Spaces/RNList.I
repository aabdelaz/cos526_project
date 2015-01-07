/* Inline functions for RING list class */



inline const RNBoolean RNVList::
IsEmpty(void) const
{
    // Return whether list is empty
    return (nentries == 0);
}



inline const int RNVList::
DataSize(void) const
{
    // Return size of data inside entries in list
    return datasize;
}



inline const int RNVList::
HeaderSize(void) const
{
    // Return size of data inside entries in list
    return headersize;
}



inline const int RNVList::
EntrySize(void) const
{
    // Return size of entries in list
    return (DataSize() + HeaderSize());
}



inline const int RNVList::
NAllocated(void) const
{
    // Return number of entries allocated memory for
    return nentries;
}



inline const int RNVList::
NEntries(void) const
{
    // Return number of entries in list
    return nentries;
}



inline const RNListEntryHeader *RNVList::
EntryHeader(const RNListEntry *entry) const
{
    // Return pointer to header part of entry
    return (const RNListEntryHeader *) entry;
}



inline RNListEntryHeader *RNVList::
EntryHeader(RNListEntry *entry) const
{
    // Return pointer to header part of entry
    return (RNListEntryHeader *) entry;
}



inline void *RNVList::
EntryData(RNListEntry *entry) const
{
    // Return pointer to data part of entry
    return (void *) ((char *) entry + HeaderSize());
}



inline RNListEntry *RNVList::
HeadEntry(void) const
{
    // Return pointer to head entry
    return head;
}



inline RNListEntry *RNVList::
TailEntry(void) const
{
    // Return pointer to tail entry
    return tail;
}



inline RNListEntry *RNVList::
PrevEntry(const RNListEntry *entry) const
{
    // Return pointer to previous entry
    return entry->prev;
}



inline RNListEntry *RNVList::
NextEntry(const RNListEntry *entry) const
{
    // Return pointer to next entry
    return entry->next;
}



inline RNListEntry *RNVList::
InsertHeadData(const void *data)
{
    // Insert data into list at head
    return InternalInsert(data, NULL, HeadEntry());
}



inline RNListEntry *RNVList::
InsertTailData(const void *data)
{
    // Insert data into list at tail
    return InternalInsert(data, NULL, NULL);
}



inline RNListEntry *RNVList::
InsertKthData(const void *data, int k)
{
    // Insert data into list at Kth position
    return InternalInsert(data, NULL, KthEntry(k));
}



inline RNListEntry *RNVList::
InsertBeforeData(const void *data, RNListEntry *before)
{
    // Insert data into list after entry
    return InternalInsert(data, NULL, before);
}



inline RNListEntry *RNVList::
InsertAfterData(const void *data, RNListEntry *after)
{
    // Insert data into list after entry
    return InternalInsert(data, NULL, NextEntry(after));
}



inline RNListEntry *RNVList::
InsertData(const void *data)
{
    // Insert data into list at default position (tail)
    return InsertTailData(data);
}



inline void RNVList::
RemoveHead(void)
{
    // Remove head entry from list
    InternalRemove(HeadEntry());
}



inline void RNVList::
RemoveTail(void)
{
    // Remove tail entry from list
    InternalRemove(TailEntry());
}



inline void RNVList::
RemoveKth(int k)
{
    // Remove kth entry from list
    InternalRemove(KthEntry(k));
}



inline void RNVList::
RemoveEntry(RNListEntry *entry)
{
    // Remove entry from list
    InternalRemove(entry);
}



inline void RNVList::
RemoveData(const void *data)
{
    // Remove data from list
    RemoveEntry(FindEntryFromData(data));
}



inline void RNVList::
PushData(const void *data)
{
    // Add data to tail of list
    InsertData(data);
}



inline void *RNVList::
PeekData(void)
{
    // Return data from head of list
    return (IsEmpty()) ? NULL : EntryData(HeadEntry());
}




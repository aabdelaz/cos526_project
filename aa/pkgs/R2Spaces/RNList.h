/* Include file for the RING list class */

#ifndef __RN__LIST__H__
#define __RN__LIST__H__



/* Library initialization functions */

int RNInitList();
void RNStopList();



/* Entry *definition */

typedef class RNListEntryHeader {
    friend class RNVList;
    protected:
        RNListEntryHeader(RNListEntryHeader *prev = NULL, RNListEntryHeader *next = NULL) 
	    : prev(prev), next(next) {};
        RNListEntryHeader *prev;
        RNListEntryHeader *next;
} RNListEntry;



/* Class definition */

class RNVList {
    public:
        // Constructor functions
        RNVList(int datasize, int headersize = sizeof(RNListEntryHeader));
        RNVList(const RNVList& list);
	~RNVList(void);

        // List property functions/operators
	const RNBoolean IsEmpty(void) const;
        const int DataSize(void) const;
        const int HeaderSize(void) const;
        const int EntrySize(void) const;
	const int NAllocated(void) const;
	const int NEntries(void) const;

	// Entry *property functions/operators
	const int EntryIndex(const RNListEntry *entry) const;
	const RNListEntryHeader *EntryHeader(const RNListEntry *entry) const;
	RNListEntryHeader *EntryHeader(RNListEntry *entry) const;
	void *EntryData(RNListEntry *entry) const;

        // Entry *access functions/operators
	RNListEntry *HeadEntry(void) const;
	RNListEntry *TailEntry(void) const;
	RNListEntry *KthEntry(int k) const;
	RNListEntry *PrevEntry(const RNListEntry *entry) const;
	RNListEntry *NextEntry(const RNListEntry *entry) const;
	RNListEntry *FindEntryFromData(const void *data, RNCompareFunction *compare = NULL, void *appl = NULL) const;

        // Insertion functions/operators
	RNListEntry *InsertHeadData(const void *data);
	RNListEntry *InsertTailData(const void *data);
	RNListEntry *InsertKthData(const void *data, int k);
	RNListEntry *InsertBeforeData(const void *data, RNListEntry *before);
	RNListEntry *InsertAfterData(const void *data, RNListEntry *after);
	RNListEntry *InsertData(const void *data);

        // Removal functions/operators
	void RemoveHead(void);
	void RemoveTail(void);
	void RemoveKth(int k);
	void RemoveEntry(RNListEntry *entry);
	void RemoveData(const void *data);

        // Insertion/removal convenience functions
	void PushData(const void *data); 
	void *PopData(void *data);
	void *PeekData(void);

        // Manipulation functions/operators
	virtual void Empty(void);
	virtual void Shift(int delta);
	virtual void Shift(int start, int length, int delta);
	virtual void Reverse(void);
	virtual void Reverse(int start, int length);
        virtual RNVList& operator=(const RNVList& list);

	// Debug function
        RNBoolean IsValid(void) const;

    public:
	// Do not use these
	virtual RNListEntry *InternalInsert(const void *data, const void *header, RNListEntry *before);
	virtual void InternalRemove(RNListEntry *entry);

    private:
	int datasize;
	int headersize;
        RNListEntry *head;
	RNListEntry *tail;
	int nentries;
};



/* Inline functions */

#include "RNList.I"



/* Template definition */

template <class DataClass>
class RNList : public RNVList {
    public:
        RNList(void) : RNVList(sizeof(DataClass)) {};
        RNList(const RNList<DataClass>& src) : RNVList(src) {};
	RN_ORDERED_ADT_DATA_CLASS_MEMBER_DEFINITIONS(RNVList, RNList, RNListEntry *, RNListEntry *, DataClass);
};



/* Utility macros */

#define RN_FOR_EACH_LIST_ENTRY(__list, __entry, __iterator) \
    (__iterator).i = 0; \
    for ((__entry) = (__list).HeadEntry(); \
	 (__entry) != NULL; \
	 (__entry) = (__list).NextEntry(__entry))

#define RN_FOR_EACH_LIST_DATA(__list, __type, __data, __iterator) \
    RN_FOR_EACH_LIST_ENTRY(__list, *((RNListEntry **) &((__iterator).entry)), __iterator) \
        if ((__data) = (__type) (__list).EntryData((RNListEntry *) (__iterator).entry)) 

#define RN_FOR_EACH_LIST(__list, __data, __iterator) \
    RN_FOR_EACH_LIST_ENTRY(__list, *((RNListEntry **) &((__iterator).entry)), __iterator) \
        if ((__data) = (__list).EntryContents((RNListEntry *) (__iterator).entry)) 



#endif





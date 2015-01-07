/* Include file for abstract adt type */

#ifndef __RN__ADT__H__
#define __RN__ADT__H__



/* Library initialization functions */

int RNInitAdt();
void RNStopAdt();



/* Iterator definition */

typedef struct RNIterator {
    void *entry;
    void *data;
    int i, j, k;
} RNIterator;



/* Callback type definitions */

typedef int RNCompareFunction(const void *data1, const void *data2, void *appl); 
typedef int RNHashFunction(const void *data, void *appl);
typedef int RNVisitFunction(const void *data, void *appl);
typedef int RNCheckFunction(const void *data, void *appl);



/* Template derivation macros */

#define RN_ORDERED_ADT_DATA_CLASS_MEMBER_DEFINITIONS(BaseAdtClass, AdtClass, BaseEntryClass, EntryClass, DataClass) \
    EntryClass FindEntry(const DataClass& data, RNCompareFunction *compare = NULL, void *appl = NULL) const \
        { return (EntryClass) FindEntryFromData((const void *) (const DataClass *) &(data), compare, appl); }; \
    DataClass *EntryData(BaseEntryClass entry) const \
        { return (DataClass *) BaseAdtClass::EntryData(entry); }; \
    DataClass *HeadData(void) const \
        { return EntryData(HeadEntry()); }; \
    DataClass *TailData(void) const \
        { return EntryData(TailEntry()); }; \
    DataClass *KthData(int k) const \
        { return EntryData(KthEntry(k)); }; \
    DataClass& EntryContents(BaseEntryClass entry) const \
        { return *(EntryData(entry)); }; \
    DataClass& Head(void) const \
        { return EntryContents(HeadEntry()); }; \
    DataClass& Tail(void) const \
        { return EntryContents(TailEntry()); }; \
    DataClass& Kth(int k) const \
        { return EntryContents(KthEntry(k)); }; \
    DataClass& operator[](int k) const \
        { return Kth(k); }; \
    EntryClass InsertHead(const DataClass& data) \
        { return (EntryClass) InsertHeadData((const void *) (const DataClass *) &(data)); }; \
    EntryClass InsertTail(const DataClass& data) \
        { return (EntryClass) InsertTailData((const void *) (const DataClass *) &(data)); }; \
    EntryClass InsertBefore(const DataClass& data, BaseEntryClass entry) \
        { return (EntryClass) InsertBeforeData((const void *) (const DataClass *) &(data), entry); }; \
    EntryClass InsertAfter(const DataClass& data, BaseEntryClass entry) \
        { return (EntryClass) InsertAfterData((const void *) (const DataClass *) &(data), entry); }; \
    EntryClass InsertKth(const DataClass& data, int k) \
        { return (EntryClass) InsertKthData((const void *) (const DataClass *) &(data), k); }; \
    EntryClass Insert(const DataClass& data) \
        { return (EntryClass) InsertData((const void *) (const DataClass *) &(data)); }; \
    void Remove(const DataClass& data) \
        { RemoveData((const void *) (const DataClass *) &(data)); }; \
    void Push(const DataClass& data) \
        { PushData((const void *) (const DataClass *) &(data)); }; \
    DataClass *Pop(DataClass& data) \
        { return (DataClass *) PopData((void *) (DataClass *) &(data)); }; \
    DataClass *Peek(void) \
        { return (DataClass *) PeekData(); }

#define RN_UNORDERED_ADT_DATA_CLASS_MEMBER_DEFINITIONS(BaseAdtClass, AdtClass, BaseEntryClass, EntryClass, DataClass) \
    EntryClass FindEntry(const DataClass& data, RNCompareFunction *compare = NULL, void *appl = NULL) const \
        { return (EntryClass) FindEntryFromData((const void *) (const DataClass *) &(data), compare, appl); }; \
    DataClass *EntryData(BaseEntryClass entry) const \
        { return (DataClass *) BaseAdtClass::EntryData(entry); }; \
    DataClass& EntryContents(BaseEntryClass entry) const \
        { return *(EntryData(entry)); }; \
    EntryClass Insert(const DataClass& data) \
        { return (EntryClass) InsertData((const void *) (const DataClass *) &(data)); }; \
    void Remove(const DataClass& data) \
        { RemoveData((const void *) (const DataClass *) &(data)); }

#define RN_TREE_DATA_CLASS_MEMBER_DEFINITIONS(BaseTreeClass, TreeClass, BaseNodeClass, NodeClass, DataClass) \
    NodeClass FindNode(const DataClass& data, RNCompareFunction *compare = NULL, void *appl = NULL) const \
	{ return (NodeClass) FindNodeFromData((const void *) (const DataClass *) &(data), compare, appl); }; \
    NodeClass FindNode(const DataClass& data, BaseNodeClass node, RNCompareFunction *compare = NULL, void *appl = NULL) const \
	{ return (NodeClass) FindNodeFromData((const void *) (const DataClass *) &(data), node, compare, appl); }; \
    DataClass *NodeData(BaseNodeClass node) const \
        { return (DataClass *) BaseTreeClass::NodeData(node); }; \
    DataClass *RootData(void) const \
        { return NodeData(RootNode()); }; \
    DataClass *ParentData(const BaseNodeClass node) const \
        { return NodeData(ParentNode(node)); }; \
    DataClass *ChildData(const BaseNodeClass node, int k) const \
        { return NodeData(ChildNode(node, k)); }; \
    DataClass& NodeContents(BaseNodeClass node) const \
        { return *((DataClass *) NodeData(node)); }; \
    DataClass& Root(void) const \
        { return NodeContents(RootNode()); }; \
    DataClass& Parent(const BaseNodeClass node) const \
        { return NodeContents(ParentNode(node)); }; \
    DataClass& Child(const BaseNodeClass node, int k) const \
        { return NodeContents(ChildNode(node, k)); }; \
    NodeClass Insert(const DataClass& data, BaseNodeClass parent) \
        { return InsertData((const void *) (const DataClass *) &(data), parent); }; \
    void Remove(const DataClass& data) \
        { RemoveData((const void *) (const DataClass *) &(data)); }

#define RN_GRAPH_DATA_CLASS_MEMBER_DEFINITIONS(BaseGraphClass, GraphClass, BaseNodeClass, NodeClass, NodeDataClass, BaseLinkClass, LinkClass, LinkDataClass) \
    NodeDataClass *NodeData(BaseNodeClass node) const \
        { return (NodeDataClass *) BaseGraphClass::NodeData(node); }; \
    LinkDataClass *LinkData(BaseLinkClass link) const \
        { return (LinkDataClass *) BaseGraphClass::LinkData(link); }; \
    NodeDataClass& NodeContents(BaseNodeClass node) const \
        { return *((NodeDataClass *) NodeData(node)); }; \
    LinkDataClass& LinkContents(BaseLinkClass link) const \
        { return *((LinkDataClass *) LinkData(link)); }; \
    NodeClass FindNode(const NodeDataClass& data, RNCompareFunction *compare = NULL, void *appl = NULL) const \
        { return FindNodeFromData((const void *) (const NodeDataClass *) &(data), compare, appl); }; \
    LinkClass FindLink(const LinkDataClass& data, RNCompareFunction *compare = NULL, void *appl = NULL) const \
        { return FindLinkFromData((const void *) (const LinkDataClass *) &(data), compare, appl); }; \
    NodeClass InsertNode(const NodeDataClass& data)  \
        { return InsertNodeData((const void *) (const NodeDataClass *) &(data)); }; \
    LinkClass InsertLink(const LinkDataClass& data, BaseNodeClass src, BaseNodeClass dst, int directed = 0)  \
        { return InsertLinkData((const void *) (const LinkDataClass *) &(data), src, dst, directed); }

#endif




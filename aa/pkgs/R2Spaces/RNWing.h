// Include file for the RN wing class 

#ifndef __RN__WING__H__
#define __RN__WING__H__





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Library initialization functions
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int RNInitWing();
void RNStopWing();





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Constant defintions
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#define RN_CW 0
#define RN_CCW 1





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Base vertex, edge, and face templates
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

template <class VertexType, class EdgeType, class FaceType>
class RNBaseWingVertex {
    public:
        RNBaseWingVertex(void);
    public:
	EdgeType *edge;             // One edge connected to vertex
        RNFlags flags;              // Bit-encoded attributes
	RNMark mark;                // Mark used for traversals
	void *data;                 // User data
};

template <class VertexType, class EdgeType, class FaceType>
class RNBaseWingEdge {
    public:
        RNBaseWingEdge(void);
    public:
	VertexType *vertex[2];      // Vertices: 0 is start, 1 is dir
        FaceType *face[2];          // Faces: 0 on left, 1 on right
        EdgeType *edge[2][2];       // 0 is face 0, 1 is face 1; 0 is prev, 1 is next
        RNFlags flags;              // Bit-encoded attributes
	RNMark mark;                // Mark used for traversals
	void *data;                 // User data
};

template <class VertexType, class EdgeType, class FaceType>
class RNBaseWingFace {
    public:
        RNBaseWingFace(void);
    public:
	EdgeType *edge;             // One edge connected to face
        RNFlags flags;              // Bit-encoded attributes
	RNMark mark;                // Mark used for traversals
	void *data;                 // User data
};





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Base winged-edge class templates
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

template <class VertexType, class EdgeType, class FaceType>
class RNBaseWing {
    public:
        // Constructor functions
        RNBaseWing(void);
	  // Constructs an empty data structure
        ~RNBaseWing(void);
	  // Destructs the data structure

        // Vertex retrieval functions/operators
	int NVertices(void) const
	  // Returns number of vertices
          { return vertices.NEntries(); };
	VertexType *Vertex(int k) const
	  // Returns kth vertex
          { return vertices.Kth(k); };

        // Edge retrieval functions/operators
	int NEdges(void) const
	  // Returns number of edges
          { return edges.NEntries(); };
	EdgeType *Edge(int k) const
	  // Returns kth edge
          { return edges.Kth(k); };

        // Face retrieval functions/operators
	int NFaces(void) const
	  // Returns number of faces
          { return faces.NEntries(); };
	FaceType *Face(int k) const
	  // Returns kth face
          { return faces.Kth(k); };

	// Vertex property functions/operators
	RNFlags VertexFlags(const VertexType *vertex) const
	  // Returns the flags stored with a vertex
          { return vertex->flags; };
	RNMark VertexMark(const VertexType *vertex) const
	  // Returns the mark stored with a vertex
          { return vertex->mark; };
	void *VertexData(const VertexType *vertex) const
	  // Returns the data stored with a vertex
          { return vertex->data; };
	void *UserData(const VertexType *vertex) const
          { return VertexData(vertex); };

	// Edge property functions/operators
	RNFlags EdgeFlags(const EdgeType *edge) const
	  // Returns the flags stored with a edge
          { return edge->flags; };
	RNMark EdgeMark(const EdgeType *edge) const
	  // Returns the mark stored with a edge
          { return edge->mark; };
	void *EdgeData(const EdgeType *edge) const
	  // Returns the data stored with a edge
          { return edge->data; };
	void *UserData(const EdgeType *edge) const
          { return EdgeData(edge); };

	// Face property functions/operators
	RNFlags FaceFlags(const FaceType *face) const
	  // Returns the flags stored with a face
          { return face->flags; };
	RNMark FaceMark(const FaceType *face) const
	  // Returns the mark stored with a face
          { return face->mark; };
	void *FaceData(const FaceType *face) const
	  // Returns the data stored with a face
          { return face->data; };
	void *UserData(const FaceType *face) const
          { return FaceData(face); };

	// Vertex manipulation functions
	void SetVertexMark(VertexType *vertex, RNMark mark)
	  // Set the mark stored with a vertex
          { vertex->mark = mark; };
	void SetVertexData(VertexType *vertex, void *data)
	  // Set the data stored with a vertex
          { vertex->data = data; };

	// Edge manipulation functions
	void SetEdgeMark(EdgeType *edge, RNMark mark)
	  // Set the mark stored with a edge
          { edge->mark = mark; };
	void SetEdgeData(EdgeType *edge, void *data)
	  // Set the data stored with a edge
          { edge->data = data; };

	// Face manipulation functions
	void SetFaceMark(FaceType *face, RNMark mark)
	  // Set the mark stored with a face
          { face->mark = mark; };
	void SetFaceData(FaceType *face, void *data)
	  // Set the data stored with a face
          { face->data = data; };

	// Vertex traversal functions/operators
	EdgeType *EdgeOnVertex(const VertexType *vertex) const
	  // Returns any edge connected to vertex
          { return vertex->edge; };
	EdgeType *EdgeBetweenVertices(const VertexType *vertex1, const VertexType *vertex2) const;
	  // Returns edge spanning between two vertices (or NULL if none)
	EdgeType *EdgeOnVertex(const VertexType *vertex, const EdgeType *edge, RNDirection dir = RN_CCW) const
	  // Returns edge on vertex in dir with respect to edge
	  {
	    assert((vertex == edge->vertex[0]) || (vertex == edge->vertex[1]));
	    int vk = (vertex == edge->vertex[0]) ? 0 : 1;
	    return edge->edge[1 - (vk ^ dir)][1-dir];
          };
	EdgeType *EdgeOnVertex(const VertexType *vertex, const FaceType *face, RNDirection dir = RN_CCW) const;
	  // Returns edge on vertex in dir with respect to face
	FaceType *FaceOnVertex(const VertexType *vertex) const
	  // Returns any face connected to vertex
	  { return FaceOnEdge(EdgeOnVertex(vertex)); };
	FaceType *FaceBetweenVertices(const VertexType *vertex1, const VertexType *vertex2, RNDirection dir = RN_CCW) const;
	  // Returns face between two vertices (in dir with respect to v1) or NULL if there is none
	FaceType *FaceOnVertex(const VertexType *vertex, const EdgeType *edge, RNDirection dir = RN_CCW) const
	  // Returns face on vertex in dir with respect to edge
	  { return FaceOnEdge(edge, vertex, dir); };
	FaceType *FaceOnVertex(const VertexType *vertex, const FaceType *face, RNDirection dir = RN_CCW) const
	  // Returns face on vertex in dir with respect to face
	  { return FaceOnVertex(vertex, EdgeOnVertex(vertex, face, dir), dir); };

	// Edge traversal functions/operators
	VertexType *VertexOnEdge(const EdgeType *edge) const
	  // Returns any vertex on edge 
	  { return edge->vertex[1]; };
	VertexType *VertexOnEdge(const EdgeType *edge, int k) const
	  // Returns kth vertex on edge 
	  { 
	      assert((k >= 0) && (k <= 1));
	      return edge->vertex[k]; 
	  };
	VertexType *VertexAcrossEdge(const EdgeType *edge, const VertexType *vertex) const
	  // Returns vertex across edge 
	  {
	    assert((vertex == edge->vertex[0]) || (vertex == edge->vertex[1]));
	    return (vertex == edge->vertex[0]) ? edge->vertex[1] : edge->vertex[0];
	  };
	VertexType *VertexBetweenEdges(const EdgeType *edge1, const EdgeType *edge2) const;
	  // Returns vertex between two edges (or NULL if there is none)
	VertexType *VertexOnEdge(const EdgeType *edge, const FaceType *face, RNDirection dir = RN_CCW) const
	  // Returns vertex of edge in dir with respect to face
	  {
	    assert((face == edge->face[0]) || (face == edge->face[1]));
	    return (face == edge->face[0]) ? edge->vertex[dir] : edge->vertex[1-dir];
	  };
	FaceType *FaceOnEdge(const EdgeType *edge) const
	  // Returns any face on edge 
	  { return (edge->face[1]) ? edge->face[1] : edge->face[0]; };
	FaceType *FaceOnEdge(const EdgeType *edge, int k) const
	  // Returns kth face on edge 
	  { 
	      assert((k >= 0) && (k <= 1));
	      return edge->face[k]; 
	  };
	FaceType *FaceOnEdge(const EdgeType *edge, const VertexType *vertex, RNDirection dir = RN_CCW) const
	  // Returns face on edge in dir with respect to vertex
	  {
	    assert((vertex == edge->vertex[0]) || (vertex == edge->vertex[1]));
	    return (vertex == edge->vertex[0]) ? edge->face[1-dir] : edge->face[dir];
	  };
	FaceType *FaceBetweenEdges(const EdgeType *edge1, const EdgeType *edge2) const;
	  // Returns the face between two edges (or NULL if there is none)
	FaceType *FaceAcrossEdge(const EdgeType *edge, const FaceType *face) const
	  // Returns face across edge 
	  {
	    assert((face == edge->face[0]) || (face == edge->face[1]));
	    return (face == edge->face[0]) ? edge->face[1] : edge->face[0];
	  };

	// Face traversal functions/operators
	VertexType *VertexOnFace(const FaceType *face) const
	  // Returns any vertex on face
	  { return VertexOnEdge(EdgeOnFace(face)); };
	VertexType *VertexOnFace(const FaceType *face, const VertexType *vertex, RNDirection dir = RN_CCW) const
	  // Returns vertex on face in dir with respect to vertex
	  { return VertexOnFace(face, EdgeOnFace(face, vertex, dir), dir); };
	VertexType *VertexOnFace(const FaceType *face, const EdgeType *edge, RNDirection dir = RN_CCW) const
	  // Returns vertex on face in dir with respect to edge
	  { return VertexOnEdge(edge, face, dir); };
	VertexType *VertexBetweenFaces(const FaceType *face1, const FaceType *face2, RNDirection dir = RN_CCW) const;
	  // Returns the vertex between two faces (in dir with respect to face1) or NULL if there is none
	EdgeType *EdgeOnFace(const FaceType *face) const
	  // Returns any edge connected to face
	  { return face->edge; };
	EdgeType *EdgeOnFace(const FaceType *face, const VertexType *vertex, RNDirection dir = RN_CCW) const
	  // Returns edge on face in dir with respect to vertex
	  { return EdgeOnVertex(vertex, face, 1-dir); };
	EdgeType *EdgeOnFace(const FaceType *face, const EdgeType *edge, RNDirection dir = RN_CCW) const
	  // Return edge on face in dir with respect to edge
	  {
	    assert((face == edge->face[0]) || (face == edge->face[1]));
	    return (face == edge->face[0]) ? edge->edge[0][dir] : edge->edge[1][dir];
	  };
	EdgeType *EdgeBetweenFaces(const FaceType *face1, const FaceType *face2) const;
	  // Returns the edge between two faces (or NULL if there is none)

	// Topology query functions
	RNBoolean IsVertexOnEdge(const VertexType *vertex, const EdgeType *edge) const
	  // Returns whether vertex lies on edge
	  { return ((vertex == edge->vertex[0]) || (vertex == edge->vertex[1])); };
	RNBoolean IsVertexOnFace(const VertexType *vertex, const FaceType *face) const;
	  // Returns whether vertex lies on face
	RNBoolean IsEdgeOnFace(const EdgeType *edge, const FaceType *face) const
	  // Returns whether edge lies on face
	  { return ((face == edge->face[0]) || (face == edge->face[1])); };

	// High-level topology manipulation functions
	virtual VertexType *CollapseEdge(EdgeType *edge);
	  // Collapses an edge into a vertex
	virtual VertexType *CollapseFace(FaceType *face);
	  // Collapses a face into a vertex
	virtual VertexType *SplitEdge(EdgeType *edge);
	  // Creates a vertex, and inserts it into an edge
	virtual EdgeType *CreateEdge(VertexType *v1, VertexType *v2, FaceType *face);
	  // Creates an edge between two vertices lying on the same face
	virtual EdgeType *SplitFace(FaceType *face, VertexType *v1, VertexType *v2);
	  // Creates an edge between two vertices lying on the same face (same as CreateEdge)
	virtual VertexType *MergeVertices(VertexType *vertex1, VertexType *vertex2);
	  // Merges two vertices into a new vertex

	// Medium-level topology manipulation functions
	virtual FaceType *CreateQuad(void);
	  // Creates a new face with four new edges and four new vertices
	virtual EdgeType *CreateEdge(VertexType *vertex1, VertexType *vertex2, 
          EdgeType *ccw1 = NULL, EdgeType *ccw2 = NULL, EdgeType *cw1 = NULL, EdgeType *cw2 = NULL);
	  // Creates a new edge between two vertices (edge ccw1 lies on vertex1 counterclockwise from the new edge)
	virtual FaceType *CreateFace(EdgeType *edge, VertexType *vertex, RNDirection dir = RN_CCW);
	  // Creates a new face on the dir side of edge with respect to vertex (edge loop must already exist)
	FaceType *CreateFace(VertexType *vertex, EdgeType *edge, RNDirection dir = RN_CCW)
	  // Creates a new face on the dir side of vertex with respect to edge (edge loop must already exist)
	  { return CreateFace(edge, vertex, dir); };
	// virtual FaceType *CreateFace(const RNArray<VertexType *>& vertices, RNDirection dir = RN_CCW);

	// Low-level topolopgy manipulation functions
	virtual VertexType *CreateVertex(void);
	  // Creates a vertex
	virtual EdgeType *CreateEdge(void);
	  // Creates an edge (connected to nothing)
	virtual FaceType *CreateFace(void);
	  // Creates a face (connected to nothing)
	virtual void DeleteVertex(VertexType *vertex);
	  // Deletes a vertex
	virtual void DeleteEdge(EdgeType *edge);
	  // Deletes an edge
	virtual void DeleteFace(FaceType *face);
	  // Deletes a face

	// Debug functions
	virtual RNBoolean IsValid(void) const;
	  // Returns whether data structure is valid

    private:
	// Other manipulation functions
	EdgeType *MergeEdges(EdgeType *edge1, EdgeType *edge2);
	  // Merges two edges into a new edge
	FaceType *MergeFaces(FaceType *face1, FaceType *face2);
	  // Merges two faces into a new face
	virtual VertexType *ReplaceVertex(VertexType *vertex1, VertexType *vertex2);
	  // Replaces vertex1 with vertex2 in all topologicial relationships
	virtual EdgeType *ReplaceEdge(EdgeType *edge1, EdgeType *edge2);
	  // Replaces edge1 with edge2 in all topologicial relationships
	virtual FaceType *ReplaceFace(FaceType *face1, FaceType *face2);
	  // Replaces face1 with face2 in all topologicial relationships

    protected:
	RNArray<VertexType *> vertices;
	RNArray<EdgeType *> edges;
	RNArray<FaceType *> faces;
};





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Winged-edge vertex, edge, face and class
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class RNWingVertex;
class RNWingEdge;
class RNWingFace;
class RNWingVertex : public RNBaseWingVertex<RNWingVertex, RNWingEdge, RNWingFace> {};
class RNWingEdge   : public RNBaseWingEdge  <RNWingVertex, RNWingEdge, RNWingFace> {};
class RNWingFace   : public RNBaseWingFace  <RNWingVertex, RNWingEdge, RNWingFace> {};
class RNWing       : public RNBaseWing<RNWingVertex, RNWingEdge, RNWingFace> {};





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Public variables
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

extern RNMark RNwing_mark;





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Useful macros
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#define RN_FOR_EACH_WING_VERTEX(__wing, __vertex, __iterator) \
    for ((__iterator).j = 0; (__iterator).j < (__wing).NVertices(); (__iterator).j++) \
        if ((__vertex) = (__wing).Vertex((__iterator).j))

#define RN_FOR_EACH_WING_EDGE(__wing, __edge, __iterator) \
    for ((__iterator).j = 0; (__iterator).j < (__wing).NEdges(); (__iterator).j++) \
        if ((__edge) = (__wing).Edge((__iterator).j))

#define RN_FOR_EACH_WING_FACE(__wing, __face, __iterator) \
    for ((__iterator).j = 0; (__iterator).j < (__wing).NFaces(); (__iterator).j++) \
        if ((__face) = (__wing).Face((__iterator).j))





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// More useful macros
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#define RN_FOR_EACH_WING_VERTEX_EDGE(__wing, __vertex, __edge, __iterator) \
    for ((__edge) = (__wing).EdgeOnVertex(__vertex), (__iterator.j) = 0; \
	 (__edge) && (((__edge) != (__wing).EdgeOnVertex(__vertex)) || ((__iterator.j) == 0)); \
	 (__edge) = (__wing).EdgeOnVertex((__vertex), (__edge), RN_CCW), (__iterator.j)++)

#define RN_FOR_EACH_WING_VERTEX_VERTEX(__wing, __vertex1, __edge, __vertex2, __iterator) \
    RN_FOR_EACH_WING_VERTEX_EDGE(__wing, __vertex1, __edge, __iterator) \
        if ((__vertex2) = (__wing).VertexAcrossEdge((__edge), (__vertex1)))

#define RN_FOR_EACH_WING_VERTEX_FACE(__wing, __vertex, __edge, __face, __iterator) \
    RN_FOR_EACH_WING_VERTEX_EDGE(__wing, __vertex, __edge, __iterator) \
        if ((__face) = (__wing).FaceOnEdge((__edge), (__vertex)))

#define RN_FOR_EACH_WING_EDGE_VERTEX(__wing,  __edge, __vertex, __iterator) \
    for ((__iterator.j) = 0; (__iterator.j) < 2; (__iterator.j)++) \
        if ((__vertex) = (__wing).VertexOnEdge((__edge), (__iterator.j)))

#define RN_FOR_EACH_WING_EDGE_FACE(__wing, __edge, __face, __iterator) \
    for ((__iterator.j) = 0; (__iterator.j) < 2; (__iterator.j)++) \
        if ((__face) = (__wing).FaceOnEdge((__edge), (__iterator.j)))

#define RN_FOR_EACH_WING_FACE_EDGE(__wing, __face, __edge, __iterator) \
    for ((__edge) = (__wing).EdgeOnFace(__face), (__iterator.j) = 0; \
	 ((__edge) != (__wing).EdgeOnFace(__face)) || ((__iterator.j) == 0); \
	 (__edge) = (__wing).EdgeOnFace((__face), (__edge), RN_CCW), (__iterator.j)++)

#define RN_FOR_EACH_WING_FACE_VERTEX(__wing, __face, __edge, __vertex, __iterator) \
    RN_FOR_EACH_WING_FACE_EDGE(__wing, __face, __edge, __iterator) \
        if ((__vertex) = (__wing).VertexOnEdge((__edge), (__face)))

#define RN_FOR_EACH_WING_FACE_FACE(__wing, __face1, __edge, __face2, __iterator) \
    RN_FOR_EACH_WING_FACE_EDGE(__wing, __face1, __edge, __iterator) \
        if ((__face2) = (__wing).FaceAcrossEdge((__edge), (__face1)))





////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Template functions
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#include "RNAdts/RNWing.I"





#endif










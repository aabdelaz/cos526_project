// Include file for the R2 wing class 

#ifndef __R2__WING__H__
#define __R2__WING__H__



// Initialization functions 

int R2InitWing();
void R2StopWing();



// Vertex, edge, and face type definitions

template <class VertexType, class EdgeType, class FaceType>
class R2BaseWingVertex : public RNBaseWingVertex<VertexType,EdgeType,FaceType> {
    public:
        R2BaseWingVertex(void);
    public:
	R2Point point;
};

template <class VertexType, class EdgeType, class FaceType>
class R2BaseWingEdge : public RNBaseWingEdge<VertexType,EdgeType,FaceType> {
    public:
        R2BaseWingEdge(void);
    public:
	R2Span span;
};

template <class VertexType, class EdgeType, class FaceType>
class R2BaseWingFace : public RNBaseWingFace<VertexType,EdgeType,FaceType> {
    public:
        R2BaseWingFace(void);
};



// Class definition 

template <class VertexType, class EdgeType, class FaceType>
class R2BaseWing : public RNBaseWing<VertexType, EdgeType, FaceType> {
    public:
        // Constructor functions
        R2BaseWing(void);
	  // Constructs an empty data structure
        R2BaseWing(const R2Box& box);
	  // Constructs a box

        // Wing property functions/operators
	const R2Box& BBox(void) const
	  // Returns the bounding box of all vertices
	  { return bbox; };

	// Vertex property functions/operators
        const R2Point& VertexPosition(const VertexType *vertex) const
	  // Returns the position of the vertex
	  { return vertex->point; };
	const R2Box VertexBBox(const VertexType *vertex) const
	  // Returns the bounding box of the vertex
	  { return R2Box(vertex->point, vertex->point); };

	// Edge property functions/operators
        const RNLength EdgeLength(const EdgeType *edge) const
	  // Returns the length of the edge
	  { return edge->span.Length(); };
        const R2Point EdgeMidpoint(const EdgeType *edge) const
	  // Returns the midpoint of the edge
	  { return edge->span.Midpoint(); };
        const R2Span& EdgeSpan(const EdgeType *edge) const
	  // Returns the span covering the edge
	  { return edge->span; };
        const R2Ray& EdgeRay(const EdgeType *edge) const
	  // Returns the ray supporting the edge
	  { return edge->span.Ray(); };
        const R2Line& EdgeLine(const EdgeType *edge) const
	  // Returns the line supporting the edge
	  { return edge->span.Line(); };
	const R2Box EdgeBBox(const EdgeType *edge) const
	  // Returns the bounding box of the edge
	  { return edge->span.BBox(); };
	const RNBoolean IsEdgeOpaque(const EdgeType *edge) const;
	  // Returns whether the edge is opaque

	// Face property functions/operators
	const R2Point FaceCentroid(const FaceType *face) const;
	  // Returns the centroid of the face
	const RNArea FaceArea(const FaceType *face) const;
	  // Returns the area of the face
	const R2Box FaceBBox(const FaceType *face) const;
	  // Returns the bounding box of the face

	// Vertex manipulation functions
	void SetVertexPosition(VertexType *vertex, const R2Point& position);

	// Edge manipulation functions
	void SetEdgeOpacity(EdgeType *edge, RNBoolean opaque);

	// Overloaded topology manipulation functions
	virtual VertexType *CreateVertex(void);
	virtual EdgeType *CreateEdge(void);
	virtual EdgeType *CreateEdge(VertexType *v1, VertexType *v2, FaceType *face);
	virtual EdgeType *CreateEdge(VertexType *vertex1, VertexType *vertex2, 
          EdgeType *ccw1 = NULL, EdgeType *ccw2 = NULL, EdgeType *cw1 = NULL, EdgeType *cw2 = NULL);
	virtual VertexType *CollapseEdge(EdgeType *edge);
	virtual VertexType *CollapseFace(FaceType *face);
	virtual VertexType *SplitEdge(EdgeType *edge);
	virtual VertexType *MergeVertices(VertexType *v1, VertexType *v2);
	virtual EdgeType *SplitFace(FaceType *face, VertexType *v1, VertexType *v2);

	// Topology manipulation functions
	virtual VertexType *CreateVertex(const R2Point& point);
	virtual VertexType *CollapseEdge(EdgeType *edge, const R2Point& point);
	virtual VertexType *CollapseFace(FaceType *face, const R2Point& point);
	virtual VertexType *SplitEdge(EdgeType *edge, const R2Point& point);
	virtual VertexType *SplitEdge(EdgeType *edge, const R2Line& line);
	virtual VertexType *MergeVertices(VertexType *v1, VertexType *v2, const R2Point& point);
	virtual EdgeType *SplitFace(FaceType *face, const R2Line& line);

	// Construction functions
	virtual FaceType *CreateBox(const R2Box& box);
	virtual FaceType *CreatePolygon(const R2Polygon& polygon);
	virtual void SplitEdge(EdgeType *edge, const R2Span& span, void *user_data = NULL);
	virtual void SplitFace(FaceType *face, const R2Span& span, void *user_data = NULL);
	virtual void Split(const R2Span& span, void *user_data = NULL);

        // Draw functions
        virtual void Draw(void) const;
        virtual void DrawVertex(VertexType *vertex) const;
        virtual void DrawEdge(EdgeType *edge) const;
        virtual void DrawFace(FaceType *face) const;

	// Search functions
	virtual FaceType *FindFace(const R2Point& point, FaceType *seed = NULL) const;

	// Debug functions
	virtual RNBoolean IsValid(void) const;

    private:
	R2Box bbox;
};



// Class type definition

class R2WingVertex;
class R2WingEdge;
class R2WingFace;
class R2WingVertex : public R2BaseWingVertex<R2WingVertex, R2WingEdge, R2WingFace> {};
class R2WingEdge   : public R2BaseWingEdge  <R2WingVertex, R2WingEdge, R2WingFace> {};
class R2WingFace   : public R2BaseWingFace  <R2WingVertex, R2WingEdge, R2WingFace> {};
typedef R2BaseWing<R2WingVertex, R2WingEdge, R2WingFace> R2Wing;



// Flag definitions

#define R2_WING_EDGE_OPAQUE_FLAG 0x00000001



// Debugging functions 

void R2PrintWingFace(const R2Wing *wing, const R2WingFace *face);
void R2PrintWingEdge(const R2Wing *wing, const R2WingEdge *edge);
void R2PrintWingVertex(const R2Wing *wing, const R2WingVertex *vertex);



// Debugging variables

extern RNScalar R2wing_minimum_edge_length;



// Useful macros 

#define R2_FOR_EACH_WING_VERTEX(__wing, __vertex, __iterator) \
    RN_FOR_EACH_WING_VERTEX(__wing, __vertex, __iterator)

#define R2_FOR_EACH_WING_EDGE(__wing, __edge, __iterator) \
    RN_FOR_EACH_WING_EDGE(__wing, __edge, __iterator) 

#define R2_FOR_EACH_WING_FACE(__wing, __face, __iterator) \
    RN_FOR_EACH_WING_FACE(__wing, __face, __iterator) 

#define R2_FOR_EACH_WING_VERTEX_EDGE(__wing, __vertex, __edge, __iterator) \
    RN_FOR_EACH_WING_VERTEX_EDGE(__wing, __vertex, __edge, __iterator) 

#define R2_FOR_EACH_WING_VERTEX_VERTEX(__wing, __vertex1, __edge, __vertex2, __iterator) \
    RN_FOR_EACH_WING_VERTEX_VERTEX(__wing, __vertex1, __edge, __vertex2, __iterator)

#define R2_FOR_EACH_WING_VERTEX_FACE(__wing, __vertex, __edge, __face, __iterator) \
    RN_FOR_EACH_WING_VERTEX_FACE(__wing, __vertex, __edge, __face, __iterator) 

#define R2_FOR_EACH_WING_EDGE_VERTEX(__wing, __edge, __vertex, __iterator) \
    RN_FOR_EACH_WING_EDGE_VERTEX(__wing, __edge, __vertex, __iterator) 

#define R2_FOR_EACH_WING_EDGE_FACE(__wing, __edge, __face, __iterator) \
    RN_FOR_EACH_WING_EDGE_FACE(__wing, __edge, __face, __iterator)

#define R2_FOR_EACH_WING_FACE_EDGE(__wing, __face, __edge, __iterator) \
    RN_FOR_EACH_WING_FACE_EDGE(__wing, __face, __edge, __iterator) 

#define R2_FOR_EACH_WING_FACE_VERTEX(__wing, __face, __edge, __vertex, __iterator) \
    RN_FOR_EACH_WING_FACE_VERTEX(__wing, __face, __edge, __vertex, __iterator) 

#define R2_FOR_EACH_WING_FACE_FACE(__wing, __face1, __edge, __face2, __iterator) \
    RN_FOR_EACH_WING_FACE_FACE(__wing, __face1, __edge, __face2, __iterator) 



// More public functions 

template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType, EdgeType, FaceType> *
R2LoadWing(R2BaseWing<VertexType, EdgeType, FaceType>& wing, 
           const RNArray<R2Span *>& spans, 
           RNBoolean ordered = FALSE);

template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType, EdgeType, FaceType> *
R2ReadWing(R2BaseWing<VertexType, EdgeType, FaceType>& wing, 
           const char *filename, 
           RNBoolean ordered = FALSE);


#if (RN_OS == RN_WINDOWSNT)
  // For some reason, VC++ doesn't get the default variable above
  template<class VertexType, class EdgeType, class FaceType> 
  R2BaseWing<VertexType, EdgeType, FaceType> *
  R2LoadWing(R2BaseWing<VertexType, EdgeType, FaceType>& wing, const RNArray<R2Span *>& spans) 
     { return R2LoadWing<VertexType, EdgeType, FaceType>(wing, spans, FALSE); };
  template<class VertexType, class EdgeType, class FaceType> 
  R2BaseWing<VertexType, EdgeType, FaceType> *
  R2ReadWing(R2BaseWing<VertexType, EdgeType, FaceType>& wing, const char *filename)
     { return R2ReadWing<VertexType, EdgeType, FaceType>(wing, filename, FALSE); };
#endif




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Template functions
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#include "R2Spaces/R2Wing.I"





#endif

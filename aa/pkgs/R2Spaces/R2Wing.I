// Source file for the R2Wing class 

#ifndef __R2__WING__I__
#define __R2__WING__I__



// Private constants

#define R2_WING_MINIMUM_EDGE_LENGTH RN_EPSILON



template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType, EdgeType, FaceType>::
R2BaseWing(void) 
    : RNBaseWing<VertexType,EdgeType,FaceType>(),
      bbox(R2null_box)
{
}



template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType, EdgeType, FaceType>::
R2BaseWing(const R2Box& box) 
    : RNBaseWing<VertexType,EdgeType,FaceType>(),
      bbox(R2null_box)
{
    // Create box
    CreateBox(box);
}



template<class VertexType, class EdgeType, class FaceType> 
const RNBoolean R2BaseWing<VertexType, EdgeType, FaceType>::
IsEdgeOpaque(const EdgeType *edge) const
{
    // Return edge opacity
    return EdgeFlags(edge)[R2_WING_EDGE_OPAQUE_FLAG];
}



template<class VertexType, class EdgeType, class FaceType> 
const R2Point R2BaseWing<VertexType, EdgeType, FaceType>::
FaceCentroid(const FaceType *face) const
{
    // Return face centroid
    int nvertices = 0;
    R2Point centroid = R2zero_point;
    RNIterator iterator;
    EdgeType *edge;
    VertexType *vertex;
    R2_FOR_EACH_WING_FACE_VERTEX(*this, face, edge, vertex, iterator) {
        centroid += VertexPosition(vertex);
	nvertices++;
    }
    if (nvertices > 0) centroid /= (RNScalar) nvertices;
    return centroid;
}



template<class VertexType, class EdgeType, class FaceType> 
const RNArea R2BaseWing<VertexType, EdgeType, FaceType>::
FaceArea(const FaceType *face) const
{
    // Compute area using method on page 1112 of Foley & Van Dam
    RNArea sum = 0.0;
    EdgeType *edge = EdgeOnFace(face);
    do {
	const R2Point& p1 = VertexPosition(VertexOnEdge(edge, face, RN_CW));
	const R2Point& p2 = VertexPosition(VertexOnEdge(edge, face, RN_CCW));
        sum += (p1.X() * p2.Y()) - (p2.X() * p1.Y());
	edge = EdgeOnFace(face, edge, RN_CCW);
    } while (edge != EdgeOnFace(face));
    
    // Return area
    return 0.5 * sum;
}



template<class VertexType, class EdgeType, class FaceType> 
const R2Box R2BaseWing<VertexType, EdgeType, FaceType>::
FaceBBox(const FaceType *face) const
{
    // Return face bounding box
    R2Box bbox = R2null_box;
    RNIterator iterator;
    EdgeType *edge;
    VertexType *vertex;
    R2_FOR_EACH_WING_FACE_VERTEX(*this, face, edge, vertex, iterator) 
        bbox.Union(VertexPosition(vertex));
    return bbox;
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
SetVertexPosition(VertexType *vertex, const R2Point& point)
{
    // Set vertex position
    vertex->point = point;

    // Update all edge spans
    EdgeType *edge;
    RNIterator iterator;
    R2_FOR_EACH_WING_VERTEX_EDGE(*this, vertex, edge, iterator) {
	const R2Point& p1 = VertexPosition(VertexOnEdge(edge, 0));
	const R2Point& p2 = VertexPosition(VertexOnEdge(edge, 1));
	edge->span.Reset(p1, p2);
    }

    // Update bounding box
    bbox.Union(point);
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
SetEdgeOpacity(EdgeType *edge, RNBoolean opaque)
{
    // Set edge opacity
    if (opaque) edge->flags.Add(R2_WING_EDGE_OPAQUE_FLAG);
    else edge->flags.Remove(R2_WING_EDGE_OPAQUE_FLAG);
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
CreateVertex(void)
{
    // Create vertex -- but don't update bbox
    return RNBaseWing<VertexType, EdgeType, FaceType>::CreateVertex();
}



template<class VertexType, class EdgeType, class FaceType> 
EdgeType *R2BaseWing<VertexType, EdgeType, FaceType>::
CreateEdge(void)
{
    // Create edge
    return RNBaseWing<VertexType, EdgeType, FaceType>::CreateEdge();
}



template<class VertexType, class EdgeType, class FaceType> 
EdgeType *R2BaseWing<VertexType, EdgeType, FaceType>::
CreateEdge(VertexType *v1, VertexType *v2, FaceType *face)
{
    // Create edge
    EdgeType *edge = RNBaseWing<VertexType, EdgeType, FaceType>::CreateEdge(v1, v2, face);

    // Update span
    edge->span.Reset(VertexPosition(v1), VertexPosition(v2));

    // Return edge
    return edge;
}



template<class VertexType, class EdgeType, class FaceType> 
EdgeType *R2BaseWing<VertexType, EdgeType, FaceType>::
CreateEdge(VertexType *v1, VertexType *v2, EdgeType *ccw1, EdgeType *ccw2, EdgeType *cw1, EdgeType *cw2)
{
#if FALSE
    // Find adjacent edges on v1
    if ((!ccw1) && (!cw1)) {
	ccw1 = EdgeOnVertex(v1);
	if (ccw1) {
	    cw1 = EdgeOnVertex(v1, ccw1, RN_CCW);
	    if (cw1 != ccw1) {
	        VertexType *vccw1 = VertexAcrossEdge(ccw1, v1);
		VertexType *vcw1 = VertexAcrossEdge(cw1, v1);
		assert(vccw1 != vcw1);
	        const R2Point& p1 = VertexPosition(v1);
		const R2Point& p2 = VertexPosition(v2);
		R2Vector v = p2 - p1;
		  
	}
    }
#endif

    // Create edge
    EdgeType *edge = RNBaseWing<VertexType, EdgeType, FaceType>::CreateEdge(v1, v2, ccw1, ccw2, cw1, cw2);

    // Update span
    edge->span.Reset(VertexPosition(v1), VertexPosition(v2));

    // Return edge
    return edge;
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
CollapseEdge(EdgeType *edge)
{
    // Collapse edge to it's midpoint
    return CollapseEdge(edge, EdgeMidpoint(edge));
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
CollapseFace(FaceType *face)
{
    // Collapse face to it's centroid
    return CollapseFace(face, FaceCentroid(face));
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
SplitEdge(EdgeType *edge)
{
    // Split edge at it's midpoint
    return SplitEdge(edge, EdgeMidpoint(edge));
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
MergeVertices(VertexType *vertex1, VertexType *vertex2)
{
    // Merge vertices at their average point
    R2Point midpoint = 0.5 * (VertexPosition(vertex1) + VertexPosition(vertex2));
    return MergeVertices(vertex1, vertex2, midpoint);
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
CreateVertex(const R2Point& point)
{
    // Create vertex
    VertexType *vertex = RNBaseWing<VertexType, EdgeType, FaceType>::CreateVertex();

    // Set vertex position
    SetVertexPosition(vertex, point);

    // Return vertex
    return vertex;
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
CollapseEdge(EdgeType *edge, const R2Point& point)
{
    // Collapse edge
    VertexType *vertex = RNBaseWing<VertexType, EdgeType, FaceType>::CollapseEdge(edge);

    // Set vertex position
    SetVertexPosition(vertex, point);

    // Return vertex
    return vertex;
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
CollapseFace(FaceType *face, const R2Point& point)
{
    // Collapse face
    VertexType *vertex = RNBaseWing<VertexType, EdgeType, FaceType>::CollapseFace(face);

    // Set vertex position
    SetVertexPosition(vertex, point);

    // Return vertex
    return vertex;
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
SplitEdge(EdgeType *edge, const R2Point& point)
{
    // Split Edge
    VertexType *vertex = RNBaseWing<VertexType, EdgeType, FaceType>::SplitEdge(edge);

    // Set flags of new edge ???
    EdgeType *edge2 = EdgeOnVertex(vertex, edge, RN_CCW);
    edge2->flags = EdgeFlags(edge);

    // Set vertex position
    SetVertexPosition(vertex, point);

    // Just checking
    assert(edge->span == R2Span(VertexPosition(VertexOnEdge(edge, 0)), VertexPosition(VertexOnEdge(edge, 1))));
    assert(edge2->span == R2Span(VertexPosition(VertexOnEdge(edge2, 0)), VertexPosition(VertexOnEdge(edge2, 1))));

#ifndef NDEBUG
    assert(vertex == edge->vertex[1]);
    assert(vertex == edge2->vertex[0]);
    VertexType *start = edge->vertex[0];
    VertexType *end = edge2->vertex[1];
    const R2Point& start_point = VertexPosition(start);
    const R2Point& end_point = VertexPosition(end);

    // Check for zero length edges
    R2Vector v1 = point - start_point;
    RNLength d1 = v1.Length();
    assert(RNIsPositive(d1, 0.5 * R2_WING_MINIMUM_EDGE_LENGTH));
    R2Vector v2 = end_point - point;
    RNLength d2 = v2.Length();
    assert(RNIsPositive(d2, 0.5 * R2_WING_MINIMUM_EDGE_LENGTH));
    
    // Check for turns
    // if (edge->face[0] || edge->face[1]) {
    //     if (RNIsPositive(d1) && RNIsPositive(d2)) {
    // 	       v1 /= d1;
    // 	       v2 /= d2;
    // 	       RNScalar cross = v1 % v2;
    //         assert((!edge->face[0]) || RNIsPositiveOrZero(cross, RN_BIG_EPSILON));
    // 	       assert((!edge->face[1]) || RNIsNegativeOrZero(cross, RN_BIG_EPSILON));
    //     }
    // }
#endif

    // Return vertex
    return vertex;
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
SplitEdge(EdgeType *edge, const R2Line& line)
{
    // Compute intersection point with line
    RNScalar t;
    R2Point point;
    if (!R2Intersects(edge->span, line, &point, &t)) return NULL;
    if (RNIsEqual(t, 0.0, R2_WING_MINIMUM_EDGE_LENGTH)) return NULL;
    if (RNIsEqual(t, edge->span.Length(), R2_WING_MINIMUM_EDGE_LENGTH)) return NULL;

    // Split Edge
    return SplitEdge(edge, point);
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
SplitEdge(EdgeType *edge1, const R2Span& span, void *user_data)
{
    // Check span
    if (RNIsZero(span.Length(), R2_WING_MINIMUM_EDGE_LENGTH)) return;

    // Find parametric values for span endpoints
    R2Span edge_span = EdgeSpan(edge1);
    RNScalar t1 = edge_span.T(span.Start());
    RNScalar t2 = edge_span.T(span.End());
    RNBoolean in_order = (t1 <= t2);
    if (!in_order) { RNScalar swap = t1; t1 = t2; t2 = swap; }
    if (RNIsGreaterOrEqual(t1, edge_span.Length(), R2_WING_MINIMUM_EDGE_LENGTH)) return;
    if (RNIsLessOrEqual(t2, 0.0, R2_WING_MINIMUM_EDGE_LENGTH)) return;

    // Insert vertices at span endpoints
    EdgeType *edge2 = edge1;
    if (RNIsPositive(t1, R2_WING_MINIMUM_EDGE_LENGTH)) {
        VertexType *vertex1 = SplitEdge(edge1, (in_order) ? span.Start() : span.End());
	edge2 = EdgeOnVertex(vertex1, edge1);
    }
    if (RNIsLess(t2, edge_span.Length(), R2_WING_MINIMUM_EDGE_LENGTH)) {
        VertexType *vertex2 = SplitEdge(edge2, (in_order) ? span.End() : span.Start());
	EdgeType *edge3 = EdgeOnVertex(vertex2, edge2);
    }

    // Set opacity of edge covered by span
    SetEdgeOpacity(edge2, TRUE);
}



template<class VertexType, class EdgeType, class FaceType> 
EdgeType *R2BaseWing<VertexType, EdgeType, FaceType>::
SplitFace(FaceType *face, const R2Line& line)
{
    // Create vertices on edge-line intersections
    VertexType *hit1 = NULL;
    VertexType *hit2 = NULL;
    EdgeType *edge = EdgeOnFace(face); 
    VertexType *v1 = VertexOnEdge(edge, face, RN_CW);
    RNScalar d1 = R2SignedDistance(line, v1->point);
    VertexType *start_vertex = v1;
    do {
        // Get 2nd vertex
        VertexType *v2 = VertexAcrossEdge(edge, v1);
	RNScalar d2 = R2SignedDistance(line, v2->point);

	// Check relationship to v1
	if (RNIsNegative(d1, R2_WING_MINIMUM_EDGE_LENGTH)) {
	    if (RNIsPositive(d2, R2_WING_MINIMUM_EDGE_LENGTH)) {
	        RNScalar d = d2 - d1;
	        R2Point point = d2/d * v1->point + -d1/d * v2->point;
	        VertexType *vertex = SplitEdge(edge, point);
		edge = EdgeOnFace(face, vertex, RN_CCW);
		if (!hit1) hit1 = vertex;
		else { hit2 = vertex; break; }
	    }
	}
	else if (RNIsPositive(d1, R2_WING_MINIMUM_EDGE_LENGTH)) {
	    if (RNIsNegative(d2, R2_WING_MINIMUM_EDGE_LENGTH)) {
	        RNScalar d = d1 - d2;
	        R2Point point = -d2/d * v1->point + d1/d * v2->point;
	        VertexType *vertex = SplitEdge(edge, point);
		edge = EdgeOnFace(face, vertex, RN_CCW);
		if (!hit1) hit1 = vertex;
		else { hit2 = vertex; break; }
	    }
	}
	else {
	    // Line contains v1
	    if (RNIsNotZero(d2, R2_WING_MINIMUM_EDGE_LENGTH)) {
		if (!hit1) hit1 = v1;
		else { hit2 = v1; break; }
	    }
	}

	// Go to next edge
	edge = EdgeOnFace(face, edge, RN_CCW);
	v1 = v2;
	d1 = d2;
    } while (v1 != start_vertex);

    // Check if found two hits
    if (!hit1 || !hit2) return NULL;
    assert(hit1 != hit2);

    // Sort hits
    R2Vector vector = hit2->point - hit1->point;
    if (vector.Dot(line.Vector()) < 0.0) { 
        VertexType *swap = hit1; hit1 = hit2; hit2 = swap; 
    }

    // Create edge across face
    return CreateEdge(hit1, hit2, face);
}



template<class VertexType, class EdgeType, class FaceType> 
EdgeType *R2BaseWing<VertexType, EdgeType, FaceType>::
SplitFace(FaceType *face, VertexType *v1, VertexType *v2)
{
    return RNBaseWing<VertexType, EdgeType, FaceType>::SplitFace(face, v1, v2);
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
SplitFace(FaceType *face, const R2Span& span, void *user_data)
{
    // Check span
    if (RNIsZero(span.Length(), R2_WING_MINIMUM_EDGE_LENGTH)) return;

    // Split face
    EdgeType *edge = SplitFace(face, span.Line());

    // Split edges 
    if (edge) {
        // Split new edge spanning interior of face
        SplitEdge(edge, span, user_data);
    }
    else {
        // Split preexisting edges colinear with span along boundary of face
        // NOTE: Splits edges during iteration ???
	RNIterator iterator;
	R2_FOR_EACH_WING_FACE_EDGE(*this, face, edge, iterator) {
	    VertexType *v1 = VertexOnEdge(edge, face, RN_CW);
	    VertexType *v2 = VertexOnEdge(edge, face, RN_CCW);
	    R2Line edge_line(VertexPosition(v1), VertexPosition(v2));
	    if (R2Contains(edge_line, span)) {
	        SplitEdge(edge, span, user_data);
	    }
	}
    }
}




template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
Split(const R2Span& span, void *user_data)
{
    // Check if bbox intersects span
    if (RNIsZero(span.Length())) return;
    if (!R2Intersects(bbox, span)) return;

    // Initialize mark
    RNMark mark = ++RNwing_mark;

    // Find the face containing the start point
    FaceType *face = FindFace(span.Start());

    // Iteratively visit faces along span
    while (face) {
        // Mark face
        face->mark = mark;
	FaceType *next_face = NULL;

        // Find the first edge intersecting the span
        EdgeType *edge;
	RNIterator iterator;
	R2_FOR_EACH_WING_FACE_EDGE(*this, face, edge, iterator) {
	    // Check if neighbor face has already been visited
	    FaceType *neighbor_face = FaceAcrossEdge(edge, face);
	    if ((neighbor_face) && (neighbor_face->mark == mark)) continue;

	    // Get oriented edge span
	    VertexType *v1 = VertexOnEdge(edge, face, RN_CW);
	    VertexType *v2 = VertexOnEdge(edge, face, RN_CCW);
	    R2Span edge_span(VertexPosition(v1), VertexPosition(v2));
	    
	    // Check if edge span is oriented properly compared to span direction
	    if (RNIsNegativeOrZero(span.Vector() % edge_span.Vector())) continue;

	    // Check if edge intersects span's ray
	    R2Point point;
	    RNScalar t1;
	    if (R2Intersects(span.Ray(), edge_span, NULL, &t1) == R2_POINT_CLASS_ID) {
		// Find next face
		next_face = (RNIsLess(t1, span.Length(), R2_WING_MINIMUM_EDGE_LENGTH)) ? FaceAcrossEdge(edge, face) : NULL;

	        // Split face
	        if (RNIsGreater(t1, 0.0, R2_WING_MINIMUM_EDGE_LENGTH)) SplitFace(face, span, user_data);

		// Done with this face
		break;
	    }
	}

	// Go to next face
	face = next_face;
    }
}



template<class VertexType, class EdgeType, class FaceType> 
VertexType *R2BaseWing<VertexType, EdgeType, FaceType>::
MergeVertices(VertexType *vertex1, VertexType *vertex2, const R2Point& point)
{
    // Merge vertices
    VertexType *vertex = RNBaseWing<VertexType, EdgeType, FaceType>::MergeVertices(vertex1, vertex2);
    
    // Set vertex position
    SetVertexPosition(vertex, point);

    // Return vertex
    return vertex;
}



template<class VertexType, class EdgeType, class FaceType> 
FaceType *R2BaseWing<VertexType, EdgeType, FaceType>::
CreateBox(const R2Box& box)
{
    // Make sure box does not contain any vertices
    // Make sure box does not cross any edges
    // Make sure box does not overlap any other faces
    // ???

    // Create box
    VertexType *vertex1 = CreateVertex(box.Corner(RN_LO, RN_LO));
    VertexType *vertex2 = CreateVertex(box.Corner(RN_HI, RN_LO));
    VertexType *vertex3 = CreateVertex(box.Corner(RN_HI, RN_HI));
    VertexType *vertex4 = CreateVertex(box.Corner(RN_LO, RN_HI));
    EdgeType *edge1 = CreateEdge(vertex1, vertex2);
    EdgeType *edge3 = CreateEdge(vertex3, vertex4);
    EdgeType *edge2 = CreateEdge(vertex2, vertex3, edge1, edge3);
    EdgeType *edge4 = CreateEdge(vertex4, vertex1, edge3, edge1);
    FaceType *face = CreateFace(vertex1, edge1);

    // Return face
    return face;
}



template<class VertexType, class EdgeType, class FaceType> 
FaceType *R2BaseWing<VertexType, EdgeType, FaceType>::
CreatePolygon(const R2Polygon& polygon)
{
    // Check polygon
    if (polygon.IsDegenerate()) return NULL;

    // Create vertices
    RNArray<VertexType *> vertices;
    for (int i = 0; i < polygon.NPoints(); i++) {
	const R2Point& point = polygon.Point(i);
	VertexType *vertex = CreateVertex(point);
	vertices.Insert(vertex);
    }

    // Create n-1 edges
    EdgeType *first_edge = NULL;
    EdgeType *last_edge = NULL;
    for (int i = 1; i < vertices.NEntries(); i++) {
	VertexType *v1 = vertices.Kth(i-1);
	VertexType *v2 = vertices.Kth(i);
	EdgeType *edge = CreateEdge(v1, v2, last_edge);
	if (!first_edge) first_edge = edge;
	last_edge = edge;
    }
    
    // Create last edge and face
    VertexType *v1 = vertices.Tail();
    VertexType *v2 = vertices.Head();
    EdgeType *edge = CreateEdge(v1, v2, last_edge, first_edge);
    FaceType *face = CreateFace(v1, edge, RN_CCW);

    // Return face
    return face;
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
Draw(void) const
{
    // Draw all edges
    EdgeType *edge;
    RNIterator iterator;
    R2_FOR_EACH_WING_EDGE(*this, edge, iterator)
        DrawEdge(edge);
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
DrawVertex(VertexType *vertex) const
{
    // Draw box around vertex 
    const R2Point& p = VertexPosition(vertex);
    RNScalar d = 0.004 * BBox().LongestAxisLength();
    R2BeginPolygon();
    R2LoadPoint(p.X() - d, p.Y() - d);
    R2LoadPoint(p.X() + d, p.Y() - d);
    R2LoadPoint(p.X() + d, p.Y() + d);
    R2LoadPoint(p.X() - d, p.Y() + d);
    R2EndPolygon();
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
DrawEdge(EdgeType *edge) const
{
    // Draw span
    edge->span.Draw();
}



template<class VertexType, class EdgeType, class FaceType> 
void R2BaseWing<VertexType, EdgeType, FaceType>::
DrawFace(FaceType *face) const
{
    // Draw polygon
    R2BeginPolygon();
    EdgeType *edge;
    RNIterator iterator;
    R2_FOR_EACH_WING_FACE_EDGE(*this, face, edge, iterator) {
        VertexType *vertex = VertexOnEdge(edge, face, RN_CCW);
        R2LoadPoint(VertexPosition(vertex));
    }
    R2EndPolygon();
}



template<class VertexType, class EdgeType, class FaceType> 
FaceType *R2BaseWing<VertexType, EdgeType, FaceType>::
FindFace(const R2Point& point, FaceType *seed) const
{
    // Check if bbox contains point
    if (!R2Contains(bbox, point)) return NULL;

    // Get seed face
    if (NFaces() == 0) return NULL;
    if (!seed) seed = Face(0);
    assert(seed);
    
    // Initialize mark
    RNMark mark = ++RNwing_mark;

    // Search faces by traversing neighbors
    while (seed) {
        // Mark face
        seed->mark = mark;

        // Find edge of face closest to point
	RNScalar best_d = 0.0;
	FaceType *best_face = NULL;
	EdgeType *edge;
        RNIterator iterator;
	R2_FOR_EACH_WING_FACE_EDGE(*this, seed, edge, iterator) {
	    // Check if neighbor face has already been visited
	    FaceType *face = FaceAcrossEdge(edge, seed);
	    if ((face) && (face->mark == mark)) continue;
	    if (!face) continue;

  	    // Create oriented edge line 
	    VertexType *v1, *v2;
	    if (edge->face[0] == seed) { v1 = edge->vertex[0]; v2 = edge->vertex[1]; }
	    else { v1 = edge->vertex[1]; v2 = edge->vertex[0]; }
	    R2Line edge_line(v1->point, v2->point);

	    // Check if signed distance from edge line to point is positive and maximal
	    RNScalar d = R2SignedDistance(edge_line, point);
	    if (d >= best_d) {
	        best_face = face;
		best_d = d;
	    }
	}

	// Check if found no edges for which point is outside
	if (!best_face) {
 	    // Face contains point OR outside bbox
  	    return (best_d == 0.0) ? seed : NULL;
	}
	else {
	    // Continue search from neighbor face
	    seed = best_face;
	}
    }

    // Should never get here
    RNAbort("Error in R2BaseWing::FindFace");
    return NULL;
}



template<class VertexType, class EdgeType, class FaceType> 
RNBoolean R2BaseWing<VertexType, EdgeType, FaceType>::
IsValid(void) const
{
    // Check topology
    if (!RNBaseWing<VertexType, EdgeType, FaceType>::IsValid()) return FALSE;

    // Check vertices
    RNIterator iterator;
    VertexType *vertex;
    R2_FOR_EACH_WING_VERTEX(*this, vertex, iterator) {
        // Check for vertices outside bounding box
        const R2Point& p = VertexPosition(vertex);
	assert(R2Contains(bbox, p));

	// Check for unconnected vertices
	assert(EdgeOnVertex(vertex));
    }
    
    // Check edges
    EdgeType *edge;
    R2_FOR_EACH_WING_EDGE(*this, edge, iterator) {
        // Check for zero-length edges
        // assert(RNIsPositive(edge->span.Length(), 0.5 * R2_WING_MINIMUM_EDGE_LENGTH));
	if (edge->span.Length() < 0.5 * R2_WING_MINIMUM_EDGE_LENGTH) {
	    RNWarning("Edge from ( %.6f %.6f ) to ( %.6f %.6f ) has length: %.6f",
		edge->span.Start().X(), edge->span.Start().Y(), edge->span.End().X(), edge->span.End().Y(), edge->span.Length());
	}

	// Check for unconnected edges
	assert(FaceOnEdge(edge));
    }
    
    // Check faces
    FaceType *face;
    R2_FOR_EACH_WING_FACE(*this, face, iterator) {
	// Check for non-convex faces
	int nedges = 0;
	EdgeType *edge1 = EdgeOnFace(face);
	VertexType *v1 = VertexOnFace(face, edge1, RN_CW);
	VertexType *v2 = VertexOnFace(face, edge1, RN_CCW);
	const R2Point *p1 = &(VertexPosition(v1));
	const R2Point *p2 = &(VertexPosition(v2));
	do {
  	    // Get next edge
	    EdgeType *edge2 = EdgeOnFace(face, edge1, RN_CCW);
	    assert(v2 == VertexOnFace(face, edge2, RN_CW));
	    VertexType *v3 = VertexOnFace(face, edge2, RN_CCW);
	    assert((v1 != v2) && (v2 != v3) && (v1 != v3));
	    const R2Point *p3 = &(VertexPosition(v3));

	    // Check for zero length edges
	    R2Vector vec1 = *p2 - *p1;
	    RNLength d1 = vec1.Length();
	    // assert(RNIsPositive(d1, RN_SMALL_EPSILON));
	    R2Vector vec2 = *p3 - *p2;
	    RNLength d2 = vec2.Length();
	    // assert(RNIsPositive(d2, RN_SMALL_EPSILON));

	    // Check for right-turns
	    // if (RNIsPositive(d1) && RNIsPositive(d2)) {
	    //     vec1 /= d1;
	    //     vec2 /= d2;
	    //     RNScalar cross = vec1 % vec2;
	    //     assert(RNIsPositiveOrZero(cross, RN_BIG_EPSILON));
	    // }

	    // Go to next edge
	    edge1 = edge2;
	    v1 = v2; 
	    v2 = v3;
	    p1 = p2;
	    p2 = p3;
	    nedges++;
        } while (edge1 != EdgeOnFace(face));

	// Check number of edges
	assert(nedges > 2);

        // Check for zero-area faces
        // RNArea area = FaceArea(face);
	// assert(RNIsPositive(area));
	// if (RNIsZero(area)) RNWarning("Face has zero area: %.6f", area);
    }

    // Return OK status
    return TRUE;
}



template <class VertexType, class EdgeType, class FaceType>
R2BaseWingVertex<VertexType, EdgeType, FaceType>::
R2BaseWingVertex(void) 
  : RNBaseWingVertex<VertexType, EdgeType, FaceType>(),
    point(R2zero_point)
{
}



template <class VertexType, class EdgeType, class FaceType>
R2BaseWingEdge<VertexType, EdgeType, FaceType>::
R2BaseWingEdge(void) 
  : RNBaseWingEdge<VertexType, EdgeType, FaceType>(),
    span(R2zero_point, R2zero_point)
{
}



template <class VertexType, class EdgeType, class FaceType>
R2BaseWingFace<VertexType, EdgeType, FaceType>::
R2BaseWingFace(void) 
  : RNBaseWingFace<VertexType, EdgeType, FaceType>()
{
}



#if TRUE

template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2LoadWing(R2BaseWing<VertexType,EdgeType,FaceType>& wing, 
    const RNArray<R2Span *>& spans, RNBoolean ordered)
{
    // Compute bounding box of spans
    R2Box bbox(R2null_box);
    for (int i = 0; i < spans.NEntries(); i++) 
        bbox.Union(spans.Kth(i)->BBox());
    if (bbox.IsEmpty()) return NULL;

    // Create box
    wing.CreateBox(bbox);

    // Split along each span 
    if (ordered) {
        for (int i = 0; i < spans.NEntries(); i++) {
	    R2Span *span = spans.Kth(i);
	    wing.Split(*span);
	}
    }
    else {
        // Create list of spans sorted by length
        RNList<R2Span *> sorted_spans;
	for (int i = 0; i < spans.NEntries(); i++) {
            int j = 0;
	    R2Span *span = spans.Kth(i);
	    for (j = 0; j < sorted_spans.NEntries(); j++) {
	        R2Span *sorted_span = sorted_spans.Kth(j);
		if (span->Length() > sorted_span->Length()) break;
	    }
	    sorted_spans.InsertKth(span, j);
	}

	// Split along each span 
        for (int i = 0; i < sorted_spans.NEntries(); i++) {
	    R2Span *span = sorted_spans.Kth(i);
	    wing.Split(*span);
	}
    }

    // Check if data structure is valid
    assert(wing.IsValid());

    // Return success
    return &wing;
}

#else

template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2LoadWingFace(R2BaseWing<VertexType,EdgeType,FaceType>& wing, FaceType *face1, 
    const RNArray<R2Span *>& spans, RNBoolean ordered)
{
    // Check if list of spans is empty
    if (spans.IsEmpty()) return NULL;

    // Choose span from list
    R2Span *span = spans.Head();

    // Partition other spans into above/below sets
    R2Span *candidate;
    RNIterator iterator;
    RNArray<R2Span *> above_spans;
    RNArray<R2Span *> below_spans;
    RN_FOR_EACH_ARRAY(spans, candidate, iterator) {
        if (candidate == span) continue;
	int split_result = R2Splits(span->Line(), *candidate);
	if ((split_result == RN_BELOW) || (split_result == RN_CROSSING)) 
	    below_spans.Insert(candidate);
	if ((split_result == RN_ABOVE) || (split_result == RN_CROSSING) || (split_result == RN_INSIDE)) 
	    above_spans.Insert(candidate);
    }
    
    // Split along span
    EdgeType *edge = wing.SplitFace(face1, span->Line());
    if (edge) {
	// Get face across edge
        FaceType *face2 = wing.FaceAcrossEdge(edge, face1);
	assert(face2);

        // Decide orientation of new edge
        VertexType *v1 = wing.VertexOnEdge(edge, face1, RN_CW);
	VertexType *v2 = wing.VertexOnEdge(edge, face1, RN_CCW);
	R2Span edge_span(wing.VertexPosition(v1), wing.VertexPosition(v2));
	RNScalar dot = edge_span.Vector().Dot(span->Vector());
	RNBoolean same_direction = (dot > 0.0);
	assert(RNIsEqual(dot, (same_direction) ? 1.0 : -1.0));
	R2Point p1 = wing.FaceCentroid(face1);
	RNScalar d1 = R2SignedDistance(span->Line(), p1);
	R2Point p2 = wing.FaceCentroid(face2);
	RNScalar d2 = R2SignedDistance(span->Line(), p2);
        assert((same_direction) || (R2SignedDistance(span->Line(), wing.FaceCentroid(face1)) > 0.0));
	assert((same_direction) || (R2SignedDistance(span->Line(), wing.FaceCentroid(face2)) < 0.0));
	assert((!same_direction) || (R2SignedDistance(span->Line(), wing.FaceCentroid(face1)) < 0.0));
	assert((!same_direction) || (R2SignedDistance(span->Line(), wing.FaceCentroid(face2)) > 0.0));

	// Split edge at span endpoints, and load pointer to span into edges' user data
	wing.SplitEdge(edge, *span, span);

        // Recursively split faces along span 
	R2LoadWingFace(wing, face1, (same_direction) ? below_spans : above_spans, ordered);
	R2LoadWingFace(wing, face2, (same_direction) ? above_spans : below_spans, ordered);

#if FALSE
        // Recursively split faces along span 
	RNScalar d1 = R2SignedDistance(span->Line(), wing.FaceCentroid(face1));
	if (RNIsPositive(d1)) R2LoadWingFace(wing, face1, above_spans, ordered);
	if (RNIsNegative(d1)) R2LoadWingFace(wing, face1, below_spans, ordered);
	RNScalar d2 = R2SignedDistance(span->Line(), wing.FaceCentroid(face2));
	if (RNIsPositive(d2)) R2LoadWingFace(wing, face2, above_spans, ordered);
	if (RNIsNegative(d2)) R2LoadWingFace(wing, face2, below_spans, ordered);
#endif
    }
    else {
        // Span does not cross interior of face
        // Split preexisting edges colinear with span 
	RNIterator iterator;
	R2_FOR_EACH_WING_FACE_EDGE(wing, face1, edge, iterator) {
	    VertexType *v1 = wing.VertexOnEdge(edge, face1, RN_CW);
	    VertexType *v2 = wing.VertexOnEdge(edge, face1, RN_CCW);
	    R2Line edge_line(wing.VertexPosition(v1), wing.VertexPosition(v2));
	    if (R2Contains(edge_line, *span)) wing.SplitEdge(edge, *span, span);
	}

        // Recursively split face
	RNScalar d1 = R2SignedDistance(span->Line(), wing.FaceCentroid(face1));
	if (RNIsPositive(d1)) R2LoadWingFace(wing, face1, above_spans, ordered);
	if (RNIsNegative(d1)) R2LoadWingFace(wing, face1, below_spans, ordered);
    }

    // Return success
    return &wing;
}



template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2LoadWing(R2BaseWing<VertexType,EdgeType,FaceType>& wing, 
    const RNArray<R2Span *>& spans, RNBoolean ordered)
{
    // Compute bounding box of spans
    R2Box bbox(R2null_box);
    for (int i = 0; i < spans.NEntries(); i++) 
        bbox.Union(spans.Kth(i)->BBox());
    if (bbox.IsEmpty()) return NULL;

    // Create box
    FaceType *face = wing.CreateBox(bbox);

    // Recursively split faces along spans 
    R2LoadWingFace(wing, face, spans, ordered);

    // Check if data structure is valid
    assert(wing.IsValid());

    // Return success
    return &wing;
}

#endif



template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2ReadWing(R2BaseWing<VertexType,EdgeType,FaceType>& wing, const char *filename, RNBoolean ordered)
{
    // Read array of spans
    RNArray<R2Span *> spans;
    if (!R2ReadSpans(spans, filename)) {
        RNFail("Unable to read file %s", filename);
	return NULL;
    }

    // Create wing from array of spans
    if (!R2LoadWing(wing, spans, ordered)) {
        RNFail("Unable to load file %s into wing", filename);
	return NULL;
    }

    // Return success
    return &wing;
}



#if FALSE

template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2SweepWing(R2BaseWing<VertexType,EdgeType,FaceType>& wing, 
    const RNArray<R2Span *>& spans, RNBoolean ordered)
{
    // Compute bounding box of spans
    R2Box bbox(R2null_box);
    for (int i = 0; i < spans.NEntries(); i++) 
        bbox.Union(spans.Kth(i)->BBox());
    if (bbox.IsEmpty()) return NULL;

    // Create box
    wing.CreateBox(bbox);

    // Create list of oriented spans sorted top-to-bottom by Y starting coordinate
    RNArray<R2Span> sorted_spans;
    for (i = 0; i < spans.NEntries(); i++) {
        R2Span *span = spans.Kth(i);
	RNBoolean oriented = (span->Start().Y() >= span->End().Y());
	RNCoord ymax = (oriented) ? span->Start().Y() : span->End().Y();
	for (int j = 0; j < sorted_spans.NEntries(); j++) {
	    const R2Span &sorted_span = sorted_spans.Kth(j);
	    if (ymax >= sorted_span.Start().Y()) break;
	}
	sorted_spans.Insert((oriented) ? *span : -(*span));
    }

    RNArray<R2Span *> active_spans;


    // Check if data structure is valid
    assert(wing.IsValid());

    // Return success
    return &wing;
}

#endif




#if FALSE

// Structures and functions for loading wing via bsp tree 

struct R2BaseWingSplitCandidate {
    R2BaseWingSplitCandidate(const R2Line& split) : split(split), total_length(0.0) {};
    R2Line split;
    RNArray<R2Span *> spans;
    RNScalar total_length;
    int total_crossings;
};



template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2SplitWingBsp(R2BaseWing<VertexType,EdgeType,FaceType>& wing, 
    R2BspTree<FaceType *>& bsptree, R2BspTreeCell *bspcell, 
    const R2Span& span)
{
    // Split all leaf cells intersecting span 
    if (bsptree.IsInterior(bspcell)) {
#if TRUE
	// Compute relationship between split line and span polygon
	int split_result = R2Splits(bsptree.CellSplitLine(bspcell), span);

	// Split appropriate child cells recursively
	if ((split_result == RN_BELOW) || (split_result == RN_CROSSING)) 
	    R2SplitWingBsp(wing, bsptree, bsptree.ChildCell(bspcell, 0), span);
	if ((split_result == RN_ABOVE) || (split_result == RN_CROSSING)) 
	    R2SplitWingBsp(wing, bsptree, bsptree.ChildCell(bspcell, 1), span);
#else
	// Compute relationship between split line and span polygon
        R2Span below_span, above_span;
	int split_result = R2Splits(bsptree.CellSplitLine(bspcell), span, &below_span, &above_span);

	// Split appropriate child cells recursively
	if ((split_result == RN_BELOW) || (split_result == RN_CROSSING)) 
	    R2SplitWingBsp(wing, bsptree, bsptree.ChildCell(bspcell, 0), below_span);
	if ((split_result == RN_ABOVE) || (split_result == RN_CROSSING)) 
	    R2SplitWingBsp(wing, bsptree, bsptree.ChildCell(bspcell, 1), above_span);
#endif
	// If span lies on split line, cover portals 
	// Use high child only, can use only one since links are shared
	if (split_result == RN_INSIDE) 
	    // R2CoverWingBsp(wing, bsptree, bsptree.ChildCell(bspcell, 1), span);
    }
    else {
	// Split wing face
        FaceType *face1 = bsptree.CellContents(bspcell);
	assert(face1);
	EdgeType *edge1 = wing.SplitFace(face1, span.Line());
	if (edge1) {
	    // Get new face (above line)
	    FaceType *face2 = wing.FaceAcrossEdge(edge1, face1);

	    // Split bsptree cell
	    bsptree.SplitCell(bspcell, span.Line());
	    bsptree.CellContents(bsptree.ChildCell(bspcell, 0)) = face1;
	    bsptree.CellContents(bsptree.ChildCell(bspcell, 1)) = face2;
	    bsptree.CellContents(bspcell) = NULL;

	    // Cover edge with span
	    R2Span edge_span = wing.EdgeSpan(edge1);
	    RNScalar t1 = edge_span.T(span.Start());
	    RNScalar t2 = edge_span.T(span.End());
	    RNBoolean in_order = (t1 <= t2);
	    assert(in_order);
	    if (!in_order) { RNScalar swap = t1; t1 = t2; t2 = swap; }
	    assert((t1 < edge_span.Length()) && (t2 > 0.0));
	    if (RNIsLess(t1, t2)) {
	        EdgeType *edge2 = edge1;
	        EdgeType *edge3 = edge1;
	        if (RNIsPositive(t1)) {
		    VertexType *vertex1 = wing.SplitEdge(edge1, (in_order) ? span.Start() : span.End());
		    edge2 = wing.EdgeOnVertex(vertex1, edge1);
		}
		if (RNIsLess(t2, edge_span.Length())) {
		    VertexType *vertex2 = wing.SplitEdge(edge2, (in_order) ? span.End() : span.Start());
		    edge3 = wing.EdgeOnVertex(vertex2, edge2);
		}

		// Mark edge over span opaque
		// wing.SetEdgeOpacity(edge2, TRUE);
	    }
	}
    }

    // Return success
    return &wing;
}



template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2LoadWingBspCell(R2BaseWing<VertexType,EdgeType,FaceType>& wing, 
    R2BspTree<FaceType *>& bsptree, R2BspTreeCell *bspcell, 
    RNArray<R2BaseWingSplitCandidate *>& candidates)
{
    // Check candidates
    if (candidates.IsEmpty()) return &wing;

    // Find best split candidate
    RNIterator iterator1;
    R2BaseWingSplitCandidate *candidate;
    R2BaseWingSplitCandidate *best_candidate = NULL;
    RN_FOR_EACH_ARRAY(candidates, candidate, iterator1) {
	if ((!best_candidate) || (candidate->total_length > best_candidate->total_length))
	    best_candidate = candidate;
    }
    assert(best_candidate);

    // Split wing by best candidate
    RNIterator iterator2;
    R2Span *span;
    RN_FOR_EACH_ARRAY(best_candidate->spans, span, iterator2) {
	R2SplitWingBsp(wing, bsptree, bsptree.RootCell(), *span);
    }

#if FALSE
    // Debug output
    for (int idebug = 1; idebug < bsptree.Depth(bspcell); idebug++) printf(" ");
    printf("Split %x (%x %x) on line (%g %g %g) with %d spans and %g length\n",
	   bspcell, 
	   (bsptree.IsInterior(bspcell)) ? bsptree.ChildCell(bspcell,0) : 0,
	   (bsptree.IsInterior(bspcell)) ? bsptree.ChildCell(bspcell,1) : 0,
	   best_candidate->split.A(), best_candidate->split.B(), best_candidate->split.C(), 
	   best_candidate->spans.NEntries(), best_candidate->total_length);
#endif

    // Check if cell was actually split  
    if (bsptree.IsInterior(bspcell)) {
	// Create new sets of split candidates below and above split
	RNArray<R2BaseWingSplitCandidate *> below_candidates;
	RNArray<R2BaseWingSplitCandidate *> above_candidates;
	RN_FOR_EACH_ARRAY(candidates, candidate, iterator1) {
	    // Skip best candidate just chosen
	    if (candidate == best_candidate) continue;
	    
	    // Classify each candidate by below/above/both
	    RNIterator iterator2;
	    R2BaseWingSplitCandidate *below_candidate = NULL;
	    R2BaseWingSplitCandidate *above_candidate = NULL;
	    RN_FOR_EACH_ARRAY(candidate->spans, span, iterator2) {
		// Determine which side of split line span lies
		int split_type = R2Splits(best_candidate->split, *span);
		
		// If span is at least partially below split line, add it to below_candidates
		if ((split_type == RN_BELOW) || (split_type == RN_CROSSING)) {
		    if (!below_candidate) {
			below_candidate = new R2BaseWingSplitCandidate(span->Line());
			below_candidates.Insert(below_candidate);
		    }
		    below_candidate->spans.Insert(span);
		    below_candidate->total_length += span->Length();
		}
		
		// If span is at least partially above split line, add it to above_candidates
		if ((split_type == RN_ABOVE) || (split_type == RN_CROSSING)) {
		    if (!above_candidate) {
			above_candidate = new R2BaseWingSplitCandidate(span->Line());
			above_candidates.Insert(above_candidate);
		    }
		    above_candidate->spans.Insert(span);
		    above_candidate->total_length += span->Length();
		}
	    }
	}

	// Split below and above subtrees
	R2LoadWingBspCell(wing, bsptree, bsptree.ChildCell(bspcell, 0), below_candidates);
	R2LoadWingBspCell(wing, bsptree, bsptree.ChildCell(bspcell, 1), above_candidates);
	
	// Delete below and above split candidates
	RN_FOR_EACH_ARRAY(below_candidates, candidate, iterator1) delete candidate;
	RN_FOR_EACH_ARRAY(above_candidates, candidate, iterator1) delete candidate;
    }
    else {
	// Copy all candidates, except one just used
	RNArray<R2BaseWingSplitCandidate *> split_candidates;
	RN_FOR_EACH_ARRAY(candidates, candidate, iterator1) {
	    if (candidate == best_candidate) continue;
	    split_candidates.Insert(new R2BaseWingSplitCandidate(*candidate));
	}		

	// Split same cell with split candidates
	R2LoadWingBspCell(wing, bsptree, bspcell, split_candidates);

	// Delete split candidates
	RN_FOR_EACH_ARRAY(split_candidates, candidate, iterator1) delete candidate;
    }

    // Return success
    return &wing;
}



template<class VertexType, class EdgeType, class FaceType> 
R2BaseWing<VertexType,EdgeType,FaceType> *
R2LoadWing(R2BaseWing<VertexType,EdgeType,FaceType>& wing, 
    const RNArray<R2Span *>& spans, RNBoolean ordered)
{
    // Create bsp tree for loading spans
    R2BspTree<FaceType *> bsptree;

    // Split bsptree along boundaries of bounding box
    const R2Box& bbox = wing.BBox();
    R2BspTreeCell *bspcell = bsptree.RootCell();
    bsptree.SplitCell(bspcell, R2Line(bbox.Max(), R2posx_vector));
    bspcell = bsptree.ChildCell(bspcell, 1);
    bsptree.SplitCell(bspcell, R2Line(bbox.Min(), R2negx_vector));
    bspcell = bsptree.ChildCell(bspcell, 1);
    bsptree.SplitCell(bspcell, R2Line(bbox.Min(), R2posy_vector));
    bspcell = bsptree.ChildCell(bspcell, 1);
    bsptree.SplitCell(bspcell, R2Line(bbox.Max(), R2negy_vector));
    bspcell = bsptree.ChildCell(bspcell, 1);
    bsptree.CellContents(bspcell) = wing.Faces().Head();
    
    // Split along each span 
    if (ordered) {
        const R2Span *span;
	RNIterator iterator;
	RN_FOR_EACH_ARRAY(spans, span, iterator) {
	    R2SplitWingBsp(wing, bsptree, bsptree.RootCell(), *span);
	}
    }
    else {
	// Create split candidates
        R2Span *span;
	RNIterator iterator1;
	RNArray<R2BaseWingSplitCandidate *> candidates;
	RN_FOR_EACH_ARRAY(spans, span, iterator1) {
	    RNIterator iterator2;
	    R2BaseWingSplitCandidate *tmp, *candidate = NULL;
	    RN_FOR_EACH_ARRAY(candidates, tmp, iterator2) {
	        if (R2Contains(tmp->split, span->Line())) { candidate = tmp; break; }
	    }
	    if (!candidate) {
	        candidate = new R2BaseWingSplitCandidate(span->Line());
		candidates.Insert(candidate);
	    }
	    candidate->total_length += span->Length();
	    candidate->spans.Insert(span);
	}

	// Split bspgraph along split candidates
	if (!R2LoadWingBspCell(wing, bsptree, bsptree.RootCell(), candidates)) return NULL;

	// Delete split candidates
	RNIterator iterator2;
	R2BaseWingSplitCandidate *candidate;
	RN_FOR_EACH_ARRAY(candidates, candidate, iterator2) {
	    delete candidate;
	}
    }

    // Return success
    return &wing;
}

#endif

#endif



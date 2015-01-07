// Source file for the RNWing class 

#ifndef __RN__WING__I__
#define __RN__WING__I__



/*********************************************************************** 
 Winged edge data structure:

      e[0][1]   e[1][0]
            \   / 
             \ /
              |v[1]
              |
      f[0]    |    f[1]
              |
              |v[0]
             / \              
            /   \
      e[0][0]   e[1][1]

***********************************************************************/



template<class VertexType, class EdgeType, class FaceType> 
RNBaseWing<VertexType, EdgeType, FaceType>::
RNBaseWing(void) 
{
    // Just checking
    assert(IsValid());
}



template <class VertexType, class EdgeType, class FaceType>
RNBaseWing<VertexType, EdgeType, FaceType>::
~RNBaseWing(void) 
{
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
EdgeBetweenVertices(const VertexType *vertex1, const VertexType *vertex2) const
{
    // Return edge between two vertices
    EdgeType *edge;
    RNIterator iterator;
    RN_FOR_EACH_WING_VERTEX_EDGE(*this, vertex1, edge, iterator) 
        if (VertexAcrossEdge(edge, vertex1) == vertex2) return edge;
    return NULL;
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
EdgeOnVertex(const VertexType *vertex, const FaceType *face, RNDirection dir) const
{
    // Return edge on vertex in dir with respect to face
    EdgeType *edge;
    RNIterator iterator;
    RN_FOR_EACH_WING_VERTEX_EDGE(*this, vertex, edge, iterator) 
        if (FaceOnEdge(edge, vertex, 1-dir) == face) return edge;
    return NULL;
}



template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
FaceBetweenVertices(const VertexType *v1, const VertexType *v2, RNDirection dir) const
{
    // Return face connected to both v1 and v2 
    EdgeType *edge = EdgeBetweenVertices(v1, v2);
    if (edge) {
        // Vertices lie on same edge, return face in dir with respect to v1
        return FaceOnVertex(v1, edge, dir);
    }
    else {
        // Vertices do not lie on the same edge, check if they are on same face
        EdgeType *e1;
        RNIterator iterator1;
	RN_FOR_EACH_WING_VERTEX_EDGE(*this, v1, e1, iterator1) {
	    EdgeType *e2;
	    RNIterator iterator2;
	    RN_FOR_EACH_WING_VERTEX_EDGE(*this, v2, e2, iterator2) {
	        if ((e1->face[0]) && ((e1->face[0] == e2->face[0]) || (e1->face[0] == e2->face[1]))) return e1->face[0];
		if ((e1->face[1]) && ((e1->face[1] == e2->face[0]) || (e1->face[1] == e2->face[1]))) return e1->face[1];
	    }
	}

	// Shared face not found
	return NULL;
    }
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
VertexBetweenEdges(const EdgeType *edge1, const EdgeType *edge2) const
{
    // Return vertex shared by edges
    if ((edge1->vertex[0] == edge2->vertex[0]) || (edge1->vertex[0] == edge2->vertex[1])) return edge1->vertex[0];
    else if ((edge1->vertex[1] == edge2->vertex[0]) || (edge1->vertex[1] == edge2->vertex[1])) return edge1->vertex[1];
    else return NULL;
}



template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
FaceBetweenEdges(const EdgeType *edge1, const EdgeType *edge2) const
{
    // Return face shared by edges
    if ((edge1->face[0]) && ((edge1->face[0] == edge2->face[0]) || (edge1->face[0] == edge2->face[1]))) return edge1->face[0];
    else if ((edge1->face[1]) && ((edge1->face[1] == edge2->face[0]) || (edge1->face[1] == edge2->face[1]))) return edge1->face[1];
    else return NULL;
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
VertexBetweenFaces(const FaceType *f1, const FaceType *f2, RNDirection dir) const
{
    // Return vertex connected to both f1 and f2
    EdgeType *edge = EdgeBetweenFaces(f1, f2);
    if (edge) {
        // Faces lie on same edge, return vertex in dir with respect to f1
        return VertexOnFace(f1, edge, dir);
    }
    else {
        // Faces do not lie on the same edge, check if they are on same vertex
        EdgeType *e1;
        RNIterator iterator1;
	RN_FOR_EACH_WING_FACE_EDGE(*this, f1, e1, iterator1) {
	    EdgeType *e2;
	    RNIterator iterator2;
	    RN_FOR_EACH_WING_FACE_EDGE(*this, f2, e2, iterator2) {
	        if ((e1->vertex[0] == e2->vertex[0]) || (e1->vertex[0] == e2->vertex[1])) return e1->vertex[0];
		if ((e1->vertex[1] == e2->vertex[0]) || (e1->vertex[1] == e2->vertex[1])) return e1->vertex[1];
	    }
	}

	// Shared vertex not found
	return NULL;
    }
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
EdgeBetweenFaces(const FaceType *face1, const FaceType *face2) const
{
    // Return edge between faces
    EdgeType *edge;
    RNIterator iterator;
    RN_FOR_EACH_WING_FACE_EDGE(*this, face1, edge, iterator) 
        if (FaceAcrossEdge(edge, face1) == face2) return edge;
    return NULL;
}



template <class VertexType, class EdgeType, class FaceType>
RNBoolean RNBaseWing<VertexType, EdgeType, FaceType>::
IsVertexOnFace(const VertexType *vertex, const FaceType *face) const
{
    // Return whether vertex lies on face
    EdgeType *edge;
    RNIterator iterator;
    RN_FOR_EACH_WING_VERTEX_EDGE(*this, vertex, edge, iterator) 
        if (FaceOnEdge(edge, vertex) == face) return TRUE;
    return FALSE;
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
CollapseEdge(EdgeType *edge)
{
    // Get vertices on edge
    VertexType *v1 = VertexOnEdge(edge);
    VertexType *v2 = VertexAcrossEdge(edge, v1);

    // Merge vertices
    return MergeVertices(v1, v2);
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
CollapseFace(FaceType *face)
{
    // Get first vertex
    VertexType *v1 = VertexOnFace(face);
    VertexType *v2 = VertexOnFace(face, v1, RN_CCW);
    assert(v1 && v2 && (v1 != v2));

    // Merge all other vertices into first vertex
    while (v1 != v2) {
        // Get next vertex
        VertexType *v = v2;
        v2 = VertexOnFace(face, v, RN_CCW);

	// Merge vertex
	VertexType *v3 = MergeVertices(v1, v);
	assert(v1 == v3);
    }

    // Return remaining vertex
    return v1;
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
SplitFace(FaceType *face, VertexType *v1, VertexType *v2)
{
    // Create edge between vertices on the same face
    return CreateEdge(v1, v2, face);
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
SplitEdge(EdgeType *edge1)
{
    // Get vertices
    VertexType *v1 = edge1->vertex[0];
    VertexType *v2 = edge1->vertex[1];
    assert(v1 && v2);

    // Create vertex
    VertexType *v = CreateVertex();
    assert(v);

    // Create edge
    EdgeType *edge2 = CreateEdge();
    assert(edge2);
    
    // Update vertex-edge relations
    v->edge = edge1;
    v1->edge = edge1;
    v2->edge = edge2;
    
    // Update edge-vertex relations
    edge1->vertex[1] = v;
    edge2->vertex[0] = v;
    edge2->vertex[1] = v2;

    // Update edge-edge relations
    if (edge1->edge[0][1]->vertex[0] == v2) edge1->edge[0][1]->edge[0][0] = edge2;
    else edge1->edge[0][1]->edge[1][0] = edge2;
    if (edge1->edge[1][0]->vertex[0] == v2) edge1->edge[1][0]->edge[1][1] = edge2;
    else edge1->edge[1][0]->edge[0][1] = edge2;
    edge2->edge[0][1] = edge1->edge[0][1];
    edge2->edge[1][0] = edge1->edge[1][0];
    edge2->edge[0][0] = edge1;
    edge2->edge[1][1] = edge1;
    edge1->edge[0][1] = edge2;
    edge1->edge[1][0] = edge2;

    // Update edge-face relations
    edge2->face[0] = edge1->face[0];
    edge2->face[1] = edge1->face[1];

    // Return vertex
    return v;
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
MergeVertices(VertexType *v1, VertexType *v2)
{
    // Find/create edge between vertices
    EdgeType *edge = EdgeBetweenVertices(v1, v2);
    if (!edge) {
        // Check if there is a face between vertices 
        FaceType *face = FaceBetweenVertices(v1, v2);
	if (face) edge = CreateEdge(v1, v2, face);
	else return NULL;
    }

    // Get faces
    FaceType *f0 = edge->face[0];
    FaceType *f1 = edge->face[1];

    // Get neighbor edges
    EdgeType *e00 = edge->edge[0][0];
    EdgeType *e01 = edge->edge[0][1];
    EdgeType *e10 = edge->edge[1][0];
    EdgeType *e11 = edge->edge[1][1];

    // Check for degenerate faces
    assert(e00 != e01);
    assert(e10 != e11);

    // Remove v2 
    ReplaceVertex(v1, v2);

    // Update vertex-edge relationships
    if (v1->edge == edge) {
        if (e00 != edge) v1->edge = e00;
	else if (e10 != edge) v1->edge = e10;
	else v1->edge = NULL;
    }

    // Update face->edge relationships
    if ((f0) && (f0->edge == edge)) {
        assert(e00 != edge);
        f0->edge = e00;
    }
    if ((f1) && (f1->edge == edge)) {
        assert(e11 != edge);
	f1->edge = e11;
    }

    // Update edge-edge relationships
    if (e00 != edge) {
        assert(e11 != edge);
        if (e00->edge[0][1] == edge) {
	    assert(e00->vertex[1] == v1);
	    if (e01 == edge) e00->edge[0][1] = e11;
	    else e00->edge[0][1] = e01;
	}
	else {
	    assert(e00->vertex[0] == v1);
	    assert(e00->edge[1][1] == edge);
	    if (e01 == edge) e00->edge[1][1] = e11;
	    else e00->edge[1][1] = e01;
	}
    }
    if (e11 != edge) {
        assert(e00 != edge);
        if (e11->edge[1][0] == edge) {
	    assert(e11->vertex[1] == v1);
	    if (e10 == edge) e11->edge[1][0] = e00;
	    else e11->edge[1][0] = e10;
	}
	else {
	    assert(e11->vertex[0] == v1);
	    assert(e11->edge[0][0] == edge);
	    if (e10 == edge) e11->edge[0][0] = e00;
	    else e11->edge[0][0] = e10;
	}
    }
    if (e01 != edge) {
        assert(e10 != edge);
        if (e01->edge[0][0] == edge) {
	    assert(e01->vertex[0] == v1);
	    if (e00 == edge) e01->edge[0][0] = e10;
	    else e01->edge[0][0] = e00;
	}
	else {
	    assert(e01->vertex[1] == v1);
	    assert(e01->edge[1][0] == edge);
	    if (e00 == edge) e01->edge[1][0] = e10;
	    else e01->edge[1][0] = e00;
	}
    }
    if (e10 != edge) {
        assert(e01 != edge);
        if (e10->edge[1][1] == edge) {
	    assert(e10->vertex[0] == v1);
	    if (e11 == edge) e01->edge[1][1] = e01;
	    else e10->edge[1][1] = e11;
	}
	else {
	    assert(e10->vertex[1] == v1);
	    assert(e10->edge[0][1] == edge);
	    if (e11 == edge) e01->edge[0][1] = e01;
	    else e10->edge[0][1] = e11;
	}
    }
    
    // Remove degenerate faces
    if ((e00 != edge) && (e01 != edge) && 
	(VertexAcrossEdge(e00, v1) == VertexAcrossEdge(e01, v1)))
        ReplaceEdge(e00, e01);
    if ((e11 != edge) && (e10 != edge) && 
	(VertexAcrossEdge(e11, v1) == VertexAcrossEdge(e10, v1)))
        ReplaceEdge(e11, e10);

    // Delete edge
    DeleteEdge(edge);

    // Return remaining vertex
    return v1;
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateEdge(VertexType *v1, VertexType *v2, FaceType *face1)
{
#if TRUE
    // Get edges adjacent to v1 and v2 on face1
    EdgeType *ccw1 = EdgeOnFace(face1, v1, RN_CW);
    if (!ccw1) return NULL;
    EdgeType *ccw2 = EdgeOnFace(face1, v2, RN_CW);
    if (!ccw2) return NULL;

    // Create Edge
    return CreateEdge(v1, v2, ccw1, ccw2);
#else
    // Get edges adjacent to v1 on face1
    EdgeType *v1e1 = EdgeOnFace(face1, v1, RN_CW);
    EdgeType *v1e2 = EdgeOnFace(face1, v1e1, RN_CCW);

    // Get edges adjacent to v2 on face1
    EdgeType *v2e1 = EdgeOnFace(face1, v2, RN_CW);
    EdgeType *v2e2 = EdgeOnFace(face1, v2e1, RN_CCW);

    // Check if v1 and v2 are adjacent
    if (VertexAcrossEdge(v1e1, v1) == v2) return NULL;
    if (VertexAcrossEdge(v1e2, v1) == v2) return NULL;
    if (VertexAcrossEdge(v2e1, v2) == v1) return NULL;
    if (VertexAcrossEdge(v2e2, v2) == v1) return NULL;

    // Create edge
    EdgeType *edge = CreateEdge();

    // Create face2
    FaceType *face2 = CreateFace();

    // Update vertex-edge relations
    v1->edge = edge;
    v2->edge = edge;

    // Update edge-vertex relationships
    edge->vertex[0] = v1;
    edge->vertex[1] = v2;

    // Update edge-edge relations
    edge->edge[0][0] = v1e1;
    edge->edge[1][1] = v1e2;
    edge->edge[1][0] = v2e1;
    edge->edge[0][1] = v2e2;
    if (v1e1->vertex[0] == v1) v1e1->edge[1][1] = edge;
    else v1e1->edge[0][1] = edge;
    if (v1e2->vertex[0] == v1) v1e2->edge[0][0] = edge;
    else v1e2->edge[1][0] = edge;
    if (v2e1->vertex[0] == v2) v2e1->edge[1][1] = edge;
    else v2e1->edge[0][1] = edge;
    if (v2e2->vertex[0] == v2) v2e2->edge[0][0] = edge;
    else v2e2->edge[1][0] = edge;
    
    // Update edge-face relations
    edge->face[0] = face1;
    edge->face[1] = face2;
    EdgeType *e = v1e2;
    while (e != edge) {
        EdgeType *enext = EdgeOnFace(face1, e, RN_CCW);
        if (e->face[0] == face1) e->face[0] = face2;
        else e->face[1] = face2;
        e = enext;
    }

    // Update face-edge relations
    face1->edge = edge;
    face2->edge = edge;

    // Return edge
    return edge;
#endif
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateEdge(VertexType *v1, VertexType *v2, 
  EdgeType *ccw1, EdgeType *ccw2, EdgeType *cw1, EdgeType *cw2)
{
    // Check if there already is an edge between the vertices
    EdgeType *edge = EdgeBetweenVertices(v1, v2);
    if (edge) return edge;

    // Create edge 
    edge = CreateEdge();
    assert(edge);
    
    // Find adjacent edges
    if (ccw1 && !cw1) cw1 = EdgeOnVertex(v1, ccw1, RN_CW);
    else if (cw1 && !ccw1) ccw1 = EdgeOnVertex(v1, cw1, RN_CCW);
    assert((!(v1->edge) || (ccw1 && cw1)));
    assert((!ccw1 && !cw1) || (ccw1 && cw1));
    assert(!ccw1 || !cw1 || (ccw1 == EdgeOnVertex(v1, cw1, RN_CCW)));
    assert(!ccw1 || !cw1 || (cw1 == EdgeOnVertex(v1, ccw1, RN_CW)));
    if (ccw2 && !cw2) cw2 = EdgeOnVertex(v2, ccw2, RN_CW);
    else if (cw2 && !ccw2) ccw2 = EdgeOnVertex(v2, cw2, RN_CCW);
    assert((!(v2->edge) || (ccw2 && cw2)));
    assert((!ccw2 && !cw2) || (ccw2 && cw2));
    assert(!ccw2 || !cw2 || (ccw2 == EdgeOnVertex(v2, cw2, RN_CCW)));
    assert(!ccw2 || !cw2 || (cw2 == EdgeOnVertex(v2, ccw2, RN_CW)));

    // Find face between vertices if there is one
    FaceType *face1 = FaceBetweenVertices(v1, v2);
    assert(!face1 || (ccw1 && cw1 && ccw2 && cw2));
    assert(!face1 || (face1 == FaceOnVertex(v1, ccw1, RN_CW)));
    assert(!face1 || (face1 == FaceOnVertex(v1, cw1, RN_CCW)));
    assert(!face1 || (face1 == FaceOnVertex(v2, ccw2, RN_CW)));
    assert(!face1 || (face1 == FaceOnVertex(v2, cw2, RN_CCW)));

    // Update edge-vertex relations
    edge->vertex[0] = v1;
    edge->vertex[1] = v2;
    
    // Update edge-edge relations
    if (ccw1) {
        assert(cw1);
	edge->edge[0][0] = ccw1;
	edge->edge[1][1] = cw1;
        if (ccw1->vertex[0] == v1) ccw1->edge[1][1] = edge;
	else ccw1->edge[0][1] = edge;
        if (cw1->vertex[0] == v1) cw1->edge[0][0] = edge;
	else cw1->edge[1][0] = edge;
    }
    else {
        assert(!cw1);
	edge->edge[0][0] = edge;
	edge->edge[1][1] = edge;
    }
    if (ccw2) {
        assert(cw2);
	edge->edge[1][0] = ccw2;
	edge->edge[0][1] = cw2;
        if (ccw2->vertex[0] == v2) ccw2->edge[1][1] = edge;
	else ccw2->edge[0][1] = edge;
        if (cw2->vertex[0] == v2) cw2->edge[0][0] = edge;
	else cw2->edge[1][0] = edge;
    }
    else {
        assert(!cw2);
	edge->edge[1][0] = edge;
	edge->edge[0][1] = edge;
	
    }
    
    // Update vertex-edge relations
    v1->edge = edge;
    v2->edge = edge;
    
    // Update faces
    if (face1) {
        // Create face2
        FaceType *face2 = CreateFace();

	// Update edge-face relations
	edge->face[0] = face1;
	edge->face[1] = face2;
	assert(cw1);
	EdgeType *e = cw1;
	while (e != edge) {
	    EdgeType *enext = EdgeOnFace(face1, e, RN_CCW);
	    if (e->face[0] == face1) e->face[0] = face2;
	    else e->face[1] = face2;
	    e = enext;
	}
	
	// Update face-edge relations
	face1->edge = edge;
	face2->edge = edge;
    }

    // Return edge
    return edge;
}



template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateFace(EdgeType *edge, VertexType *vertex, RNDirection dir)
{
    // Check if face already exists
    FaceType *face = FaceOnEdge(edge, vertex, dir);
    if (face) return face;

    // Create face
    face = CreateFace();

    // Update edge-face relations
    VertexType *v = vertex;
    EdgeType *e = edge;
    do {
	// Update edge-face relations
	if (v == e->vertex[0]) {
	    assert(!(e->face[1-dir]));
	    e->face[1-dir] = face;
	}
	else {
	    assert(!(e->face[dir]));
	    e->face[dir] = face;
	}

	// Go to next edge
	v = VertexAcrossEdge(e, v);
	e = EdgeOnVertex(v, e, 1-dir);
    } while (e != edge);

    // Update face-edge relations
    face->edge = edge;

    // Return face
    return face;
}



#if FALSE

template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateFace(const RNArray<VertexType *>& vertices, RNDirection dir)
{
    // Check number of vertices
    if (vertices.NEntries() < 3) return NULL;

    // Create edges
    VertexType *v1 = vertices.Tail();
    for (int i = 0; i < vertices.NEntries(); i++) {
	VertexType *v2 = vertices.Kth(i);
	EdgeType *edge = CreateEdge(v1, v2, ???);
	if (FaceOnVertex(v1, edge, dir)) return NULL;
	v1 = v2;
    }

    // Create face
    VertexType *tail = vertices.Tail();
    VertexType *head = vertices.Head();
    EdgeType *edge = EdgeBetweenVertices(tail, head);
    return CreateFace(tail, edge, dir);
}

#endif



template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateQuad(void)
{
    // Create four vertices
    VertexType *v1 = CreateVertex();
    VertexType *v2 = CreateVertex();
    VertexType *v3 = CreateVertex();
    VertexType *v4 = CreateVertex();

    // Create four edges
    EdgeType *e1 = CreateEdge();
    EdgeType *e2 = CreateEdge();
    EdgeType *e3 = CreateEdge();
    EdgeType *e4 = CreateEdge();
    
    // Create face
    FaceType *face = CreateFace();

    // Update vertex-edge relationships
    v1->edge = e1;
    v2->edge = e2;
    v3->edge = e3;
    v4->edge = e4;

    // Update edge-vertex relationships
    e1->vertex[0] = v1;
    e1->vertex[1] = v2;
    e2->vertex[0] = v2;
    e2->vertex[1] = v3;
    e3->vertex[0] = v3;
    e3->vertex[1] = v4;
    e4->vertex[0] = v4;
    e4->vertex[1] = v1;

    // Update edge-edge relationships
    e1->edge[0][0] = e4;
    e1->edge[0][1] = e2;
    e1->edge[1][0] = e2;
    e1->edge[1][1] = e4;
    e2->edge[0][0] = e1;
    e2->edge[0][1] = e3;
    e2->edge[1][0] = e3;
    e2->edge[1][1] = e1;
    e3->edge[0][0] = e2;
    e3->edge[0][1] = e4;
    e3->edge[1][0] = e4;
    e3->edge[1][1] = e2;
    e4->edge[0][0] = e3;
    e4->edge[0][1] = e1;
    e4->edge[1][0] = e1;
    e4->edge[1][1] = e3;

    // Update face-edge relationships
    face->edge = e1;

    // Update edge-face relationships
    e1->face[0] = face;
    e1->face[1] = NULL;
    e2->face[0] = face;
    e2->face[1] = NULL;
    e3->face[0] = face;
    e3->face[1] = NULL;
    e4->face[0] = face;
    e4->face[1] = NULL;

    // Just checking
    assert(IsValid());

    // Return face
    return face;
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateVertex(void)
{
    // Create vertex
    VertexType *vertex = new VertexType();

    // Insert vertex into array
    vertices.Insert(vertex);

    // Return vertex
    return vertex;
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateEdge(void)
{
    // Create edge
    EdgeType *edge = new EdgeType();

    // Insert edge into array
    edges.Insert(edge);

    // Return edge
    return edge;
}



template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
CreateFace(void)
{
    // Create face
    FaceType *face = new FaceType();

    // Insert face into array
    faces.Insert(face);

    // Return face
    return face;
}



template <class VertexType, class EdgeType, class FaceType>
void RNBaseWing<VertexType, EdgeType, FaceType>::
DeleteVertex(VertexType *vertex)
{
    // Update relations
    // ???

    // Remove vertex from array
    vertices.Remove(vertex);

    // Delete vertex
    delete vertex;
}



template <class VertexType, class EdgeType, class FaceType>
void RNBaseWing<VertexType, EdgeType, FaceType>::
DeleteEdge(EdgeType *edge)
{
    // Update relations
    // ???

    // Remove edge from array
    edges.Remove(edge);

    // Delete edge
    delete edge;
}



template <class VertexType, class EdgeType, class FaceType>
void RNBaseWing<VertexType, EdgeType, FaceType>::
DeleteFace(FaceType *face)
{
    // Update edge-face relations
    EdgeType *edge1 = EdgeOnFace(face);
    EdgeType *edge = edge1;
    if (edge) do {
         EdgeType *next_edge = EdgeOnFace(face, edge, RN_CCW);
         if (edge->face[0] = face) edge->face[0] = NULL;
         else { assert(edge->face[1] == face); edge->face[1] = NULL; }
         edge = next_edge;
     } while (edge != edge1);

    // Remove face from array
    faces.Remove(face);

    // Delete face
    delete face;
}



template <class VertexType, class EdgeType, class FaceType>
VertexType *RNBaseWing<VertexType, EdgeType, FaceType>::
ReplaceVertex(VertexType *v1, VertexType *v2)
{
    // Replace all edge-vertex references to v2 with v1
    EdgeType *edge = EdgeOnVertex(v2);
    EdgeType *e = edge;
    if (e) do {
        EdgeType *nexte = EdgeOnVertex(v2, e, RN_CCW);
	if (e->vertex[0] == v2) e->vertex[0] = v1;
	else if (e->vertex[1] == v2) e->vertex[1] = v1;
	else RNAbort("Invalid ReplaceVertex");
	e = nexte;
    } while (e != edge);

    // Delete v2
    DeleteVertex(v2);

    // Return remaining vertex
    return v1;
}



template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
ReplaceEdge(EdgeType *e1, EdgeType *e2)
{
    // Just checking 
    assert(e1 != e2);

    // Check edge-vertex references
    assert(IsVertexOnEdge(e1->vertex[0], e2));
    assert(IsVertexOnEdge(e1->vertex[1], e2));
    assert(IsVertexOnEdge(e2->vertex[0], e1));
    assert(IsVertexOnEdge(e2->vertex[1], e1));

    // Update all vertex-edge references 
    if (e2->vertex[0]->edge == e2) {
        e2->vertex[0]->edge = e1;
    }
    if (e2->vertex[1]->edge == e2) {
        e2->vertex[1]->edge = e1;
    }

    // Update all face-edge references
    if ((e2->face[0]) && (e2->face[0]->edge == e2)) {
        e2->face[0]->edge = e1;
    }
    if ((e2->face[1]) && (e2->face[1]->edge == e2)) {
        e2->face[1]->edge = e1;
    }

    // Update edge-face and edge-edge references
    FaceType *face = NULL;
    if ((e1->edge[1][0] == e2) && (e1->edge[1][1] == e2)) {
        face = e1->face[1];
	if ((e2->edge[1][0] == e1) && (e2->edge[1][1] == e1)) {
	    e1->edge[1][0] = e2->edge[0][0];
	    e1->edge[1][1] = e2->edge[0][1];
	    e1->face[1] = e2->face[0];
	}
	else {
	    assert(e2->edge[0][0] == e1);
	    assert(e2->edge[0][1] == e1);
	    e1->edge[1][0] = e2->edge[1][0];
	    e1->edge[1][1] = e2->edge[1][1];
	    e1->face[1] = e2->face[1];
	}
    }
    else {
        assert(e1->edge[0][0] == e2);
        assert(e1->edge[0][1] == e2);
        face = e1->face[0];
	if ((e2->edge[1][0] == e1) && (e2->edge[1][1] == e1)) {
	    e1->edge[0][0] = e2->edge[0][0];
	    e1->edge[0][1] = e2->edge[0][1];
	    e1->face[0] = e2->face[0];
	}
	else {
	    assert(e2->edge[0][0] == e1);
	    assert(e2->edge[0][1] == e1);
	    e1->edge[0][0] = e2->edge[1][0];
	    e1->edge[0][1] = e2->edge[1][1];
	    e1->face[0] = e2->face[1];
	}
    }

    // Update e1 if e1 and e2 formed elbow
    if ((e1->edge[0][0] == e1) || (e1->edge[1][1] == e1) ||
	(e1->edge[0][0] == e2) || (e1->edge[1][1] == e2)) {
        assert((e1->edge[0][0] == e1) || (e1->edge[0][0] == e2));
        assert((e1->edge[1][1] == e1) || (e1->edge[1][1] == e2));
	e1->edge[0][0] = e1;
	e1->edge[1][1] = e1;
    }
    if ((e1->edge[0][1] == e1) || (e1->edge[1][0] == e1) ||
	(e1->edge[0][1] == e2) || (e1->edge[1][0] == e2)) {
        assert((e1->edge[0][1] == e1) || (e1->edge[0][1] == e2));
        assert((e1->edge[1][0] == e1) || (e1->edge[1][0] == e2));
	e1->edge[1][0] = e1;
	e1->edge[0][1] = e1;
    }
    
    // Just checking
    assert(e1->edge[0][0] != e2);
    assert(e1->edge[0][1] != e2);
    assert(e1->edge[1][0] != e2);
    assert(e1->edge[1][1] != e2);

    // Update more edge-edge references
    if ((e2->edge[0][0] != e1) && (e2->edge[0][0] != e2)) {
        if (e2->edge[0][0]->edge[0][0] == e2) e2->edge[0][0]->edge[0][0] = e1;
	if (e2->edge[0][0]->edge[0][1] == e2) e2->edge[0][0]->edge[0][1] = e1;
	if (e2->edge[0][0]->edge[1][0] == e2) e2->edge[0][0]->edge[1][0] = e1;
	if (e2->edge[0][0]->edge[1][1] == e2) e2->edge[0][0]->edge[1][1] = e1;
    }
    if ((e2->edge[0][1] != e1) && (e2->edge[0][1] != e2)) {
        if (e2->edge[0][1]->edge[0][0] == e2) e2->edge[0][1]->edge[0][0] = e1;
	if (e2->edge[0][1]->edge[0][1] == e2) e2->edge[0][1]->edge[0][1] = e1;
	if (e2->edge[0][1]->edge[1][0] == e2) e2->edge[0][1]->edge[1][0] = e1;
	if (e2->edge[0][1]->edge[1][1] == e2) e2->edge[0][1]->edge[1][1] = e1;
    }
    if ((e2->edge[1][0] != e1) && (e2->edge[1][0] != e2)) {
        if (e2->edge[1][0]->edge[0][0] == e2) e2->edge[1][0]->edge[0][0] = e1;
	if (e2->edge[1][0]->edge[0][1] == e2) e2->edge[1][0]->edge[0][1] = e1;
	if (e2->edge[1][0]->edge[1][0] == e2) e2->edge[1][0]->edge[1][0] = e1;
	if (e2->edge[1][0]->edge[1][1] == e2) e2->edge[1][0]->edge[1][1] = e1;
    }
    if ((e2->edge[1][1] != e1) && (e2->edge[1][1] != e2)) {
        if (e2->edge[1][1]->edge[0][0] == e2) e2->edge[1][1]->edge[0][0] = e1;
	if (e2->edge[1][1]->edge[0][1] == e2) e2->edge[1][1]->edge[0][1] = e1;
	if (e2->edge[1][1]->edge[1][0] == e2) e2->edge[1][1]->edge[1][0] = e1;
	if (e2->edge[1][1]->edge[1][1] == e2) e2->edge[1][1]->edge[1][1] = e1;
    }

    // Delete face
    assert(face);
    DeleteFace(face);

    // Delete e2
    DeleteEdge(e2);

    // Return remaining edge
    return e1;
}



template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
ReplaceFace(FaceType *f1, FaceType *f2)
{
    // Replace all edge-face references to f2 with f1
    EdgeType *edge = EdgeOnFace(f2);
    EdgeType *e = edge;
    if (e) do {
        EdgeType *nexte = EdgeOnFace(f2, e, RN_CCW);
	if (e->face[0] == f2) e->face[0] = f1;
	else if (e->face[1] == f2) e->face[1] = f1;
	else RNAbort("Invalid ReplaceFace");
	e = nexte;
    } while (e != edge);

    // Delete f2
    DeleteFace(f2);

    // Return remaining face
    return f1;
}



template <class VertexType, class EdgeType, class FaceType>
RNBoolean RNBaseWing<VertexType, EdgeType, FaceType>::
IsValid(void) const
{
    RNIterator iterator;

    // Check all vertices
    VertexType *v;
    RN_FOR_EACH_WING_VERTEX(*this, v, iterator) {
        // Check vertex-edge relations
        assert(!(v->edge) || (IsVertexOnEdge(v, v->edge)));
    }
    
    // Check all edges
    EdgeType *e;
    RN_FOR_EACH_WING_EDGE(*this, e, iterator) {
        // Check edge-vertex relations
        assert(e->vertex[0] != e->vertex[1]);
	assert(IsVertexOnEdge(e->vertex[0], e));
	assert(IsVertexOnEdge(e->vertex[1], e));

	// Check edge endpoint consistency
	if ((e->edge[0][0] == e) || (e->edge[1][1] == e)) {
	    assert(e->edge[0][0] == e);
	    assert(e->edge[1][1] == e);
	}
	else if (e->edge[0][0] == e->edge[1][1]) {
	    assert(e->edge[0][1] != e->edge[0][0]);
	    assert(e->edge[1][0] != e->edge[0][0]);
	}
	if ((e->edge[0][1] == e) || (e->edge[1][0] == e)) {
	    assert(e->edge[0][1] == e);
	    assert(e->edge[1][0] == e);
	}
	else if (e->edge[0][1] == e->edge[1][0]) {
	    assert(e->edge[0][0] != e->edge[0][1]);
	    assert(e->edge[1][1] != e->edge[0][1]);
	}

        // Check edge-edge connectivity
	if (e->edge[0][0]->vertex[0] == e->vertex[0]) {
	    assert(e == e->edge[0][0]->edge[1][1]);
	    assert(e != e->edge[0][0]->edge[1][0]);
	}
	else {
	    assert(e == e->edge[0][0]->edge[0][1]);
	    assert(e != e->edge[0][0]->edge[0][0]);
	}
	if (e->edge[0][1]->vertex[0] == e->vertex[1]) {
	    assert(e == e->edge[0][1]->edge[0][0]);
	    assert(e != e->edge[0][1]->edge[0][1]);
	}
	else {
	    assert(e == e->edge[0][1]->edge[1][0]);
	    assert(e != e->edge[0][1]->edge[1][1]);
	}
	if (e->edge[1][0]->vertex[0] == e->vertex[1]) {
	    assert(e == e->edge[1][0]->edge[1][1]);
	    assert(e != e->edge[1][0]->edge[1][0]);
	}
	else {
	    assert(e == e->edge[1][0]->edge[0][1]);
	    assert(e != e->edge[1][0]->edge[0][0]);
	}
	if (e->edge[1][1]->vertex[0] == e->vertex[0]) {
	    assert(e == e->edge[1][1]->edge[0][0]);
	    assert(e != e->edge[1][1]->edge[0][1]);
	}
	else {
	    assert(e == e->edge[1][1]->edge[1][0]);
	    assert(e != e->edge[1][1]->edge[1][1]);
	}

        // Check edge-face relations
	if (e->face[0]) {
	    assert(IsEdgeOnFace(e, e->face[0]));
	}
	if (e->face[1]) {
	    assert(IsEdgeOnFace(e, e->face[1]));
	}
    }
    
    // Check all faces
    FaceType *f;
    RN_FOR_EACH_WING_FACE(*this, f, iterator) {
        // Check face-edge relations
        assert(f->edge);
        assert(IsEdgeOnFace(f->edge, f));

        // Check edge->face relations
	RNIterator iterator2;
        RN_FOR_EACH_WING_FACE_EDGE(*this, f, e, iterator2) {
	    assert(IsEdgeOnFace(e, f));
	}
    }

    // Return success
    return TRUE;
}



template <class VertexType, class EdgeType, class FaceType>
RNBaseWingVertex<VertexType, EdgeType, FaceType>::
RNBaseWingVertex(void) 
    : edge(NULL),
      flags(0),
      mark(0),
      data(NULL)
{
}



template <class VertexType, class EdgeType, class FaceType>
RNBaseWingEdge<VertexType, EdgeType, FaceType>::
RNBaseWingEdge(void) 
    : flags(0),
      mark(0),
      data(NULL)
{
    // Initialize edge fields
    vertex[0] = vertex[1] = NULL;
    face[0] = face[1] = NULL;
    edge[0][0] = edge[0][1] = edge[1][0] = edge[1][1] = NULL;
}



template <class VertexType, class EdgeType, class FaceType>
RNBaseWingFace<VertexType, EdgeType, FaceType>::
RNBaseWingFace(void) 
    : edge(NULL),
      flags(0),
      mark(0),
      data(NULL)
{
}



#if FALSE

template <class VertexType, class EdgeType, class FaceType>
EdgeType *RNBaseWing<VertexType, EdgeType, FaceType>::
MergeEdges(EdgeType *e1, EdgeType *e2)
{
    // Check edges
    if (e1 == e2) return e1;

    // Find face between edges
    FaceType *face = FaceBetweenEdges(e1, e2);
    if (!face) return NULL;

    // Find vertices of e1 and e2, ordered CCW
    VertexType *v11 = VertexOnFace(face, e1, RN_CW);
    VertexType *v12 = VertexOnFace(face, e1, RN_CCW);
    VertexType *v21 = VertexOnFace(face, e2, RN_CW);
    VertexType *v22 = VertexOnFace(face, e2, RN_CCW);
    assert(v11 != v21);
    assert(v22 != v22);

    // Merge vertices
    VertexType *v1 = (v12 == v21) ? v12 : MergeVertices(v12, v21);
    VertexType *v2 = (v11 == v22) ? v11 : MergeVertices(v11, v22);
    assert(v1 && v2);

    // Return remaining edge
    EdgeType *edge = EdgeBetweenVertices(v1, v2);
    assert(edge);
    return edge;
}



template <class VertexType, class EdgeType, class FaceType>
FaceType *RNBaseWing<VertexType, EdgeType, FaceType>::
MergeFaces(FaceType *f1, FaceType *f2)
{
    // Check faces
    if (f1 == f2) return f1;

    // Find edge between faces
    EdgeType *edge1 = EdgeBetweenFaces(f1, f2);
    if (!edge1) return NULL;

    // Update face-edge relations
    if (f1->edge == edge1) {
        f1->edge = EdgeOnFace(f1, edge1, RN_CCW);
	assert((f1->edge) && (f1->edge != edge1));
    }

    // Update edge-face relations
    EdgeType *edge2 = EdgeOnFace(f2, edge1, RN_CCW);
    if (edge2) do {
        EdgeType *next_edge = EdgeOnFace(f2, edge2, RN_CCW);
	if (edge2->face[0] == f2) edge2->face[0] = f1;
	else { assert(edge2->face[1] == f2); edge2->face[1] = f1; }
	edge2 = next_edge;
    } while (edge2 != edge1);

    // Return remaining face
    return f1;
}

#endif



#endif

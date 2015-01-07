/* Source file for the RING point beam class */



/* Include files */

#include "R2Spaces/R2Spaces.h"



/* Public functions */

int 
R2InitPointBeam()
{
    /* Return success */
    return TRUE;
}



void 
R2StopPointBeam()
{
}



R2PointBeam::
R2PointBeam(void)
{
}



R2PointBeam::
R2PointBeam(const R2Point& source)
    : source(source),
      flags(RN_NO_FLAGS)
{
}



R2PointBeam::
R2PointBeam(const R2Point& source, const R2Halfspace& h0, const R2Halfspace& h1)
    : source(source),
      flags(R2_POINTBEAM_TRIMMED)
{
    // Assign halfspaces
    halfspaces[RN_LO] = h0;
    halfspaces[RN_HI] = h1;
}



R2PointBeam::
R2PointBeam(const R2PointBeam& beam)
    : source(beam.source),
      flags(beam.flags)
{
    // Copy halfspaces
    if (flags[R2_POINTBEAM_TRIMMED]) {
        halfspaces[RN_LO] = beam.halfspaces[RN_LO];
	halfspaces[RN_HI] = beam.halfspaces[RN_HI];
    }
}



void R2PointBeam::
Trim(const R2Span& span)
{
    // Order span endpoints
    RNScalar d = R2SignedDistance(span.Line(), source);
    if (RNIsNegative(d)) {
        Trim(span.End(), span.Start());
    }
    else if (RNIsPositive(d)) {
        Trim(span.Start(), span.End());
    }
}



void R2PointBeam::
Trim(const R2Point& p1, const R2Point& p2)
{
    // Trim beam
    if (flags[R2_POINTBEAM_TRIMMED]) {
        // Update halfspaces
        RNScalar d0 = R2SignedDistance(halfspaces[0].Line(), p1); 
	if (d0 > 0.0) halfspaces[0].Reset(R2Line(source, p1));
	RNScalar d1 = R2SignedDistance(halfspaces[1].Line(), p2);
	if (d1 > 0.0) halfspaces[1].Reset(R2Line(p2, source));
    }
    else {
        // Set halfspaces
        halfspaces[0].Reset(R2Line(source, p1));
	halfspaces[1].Reset(R2Line(p2, source));
	flags.Add(R2_POINTBEAM_TRIMMED);
    }

    // Just checking
    // assert((halfspaces[0].Normal() % halfspaces[1].Normal()) <= 0.0);
}



void R2PointBeam::
Mirror(const R2Line& line)
{
    // Mirror source
    source.Mirror(line);

    // Mirror halfspaces
    if (flags[R2_POINTBEAM_TRIMMED]) {
        // Mirror halfspaces
        halfspaces[0].Mirror(line);
	halfspaces[1].Mirror(line);

	// Flip and swap halfspaces
	R2Halfspace swap = -(halfspaces[0]);
	halfspaces[0] = -(halfspaces[1]);
	halfspaces[1] = swap;

	// Just checking
	// assert((halfspaces[0].Normal() % halfspaces[1].Normal()) >= 0.0);
    }
}



void R2PointBeam::
Reset(const R2Point& source, const R2Halfspace& h0, const R2Halfspace& h1)
{
    // Reset everything
    this->source = source;
    this->flags = R2_POINTBEAM_TRIMMED;
    this->halfspaces[RN_LO] = h0;
    this->halfspaces[RN_HI] = h1;
}



RNBoolean R2PointBeam::
Contains(const R2Point& point, RNScalar epsilon) const
{
    // Return whether all active halfspaces contain point
    if (!flags[R2_POINTBEAM_TRIMMED]) return TRUE;
    if (!R2Contains(halfspaces[RN_LO], point)) return FALSE;
    if (!R2Contains(halfspaces[RN_HI], point)) return FALSE;
    return TRUE;
}



RNBoolean R2PointBeam::
Contains(const R2Shape& shape, RNScalar epsilon) const
{
    // Return whether all active halfspaces contain shape
    if (!flags[R2_POINTBEAM_TRIMMED]) return TRUE;
    if (!R2Contains(halfspaces[RN_LO], shape)) return FALSE;
    if (!R2Contains(halfspaces[RN_HI], shape)) return FALSE;
    return TRUE;
}



RNClassID R2PointBeam::
Intersects(const R2Span& span, R2Span *result, RNScalar epsilon) const
{
    // Check whether beam is trimmed
    if (!flags[R2_POINTBEAM_TRIMMED]) {
        // Return whole span
        if (result) *result = span;
	return R2_SPAN_CLASS_ID;
    }
    else {
        // Compute intersection
        return Intersects(span.Start(), span.End(), result, epsilon);
    }
}



RNClassID R2PointBeam::
Intersects(const R2Point& p1, const R2Point& p2, R2Span *result, RNScalar epsilon) const
{
    // Check whether beam is trimmed
    if (!flags[R2_POINTBEAM_TRIMMED]) {
        if (result) result->Reset(p1, p2);
	return R2_SPAN_CLASS_ID;
    }

#if TRUE
    // Initialize result
    R2Point result_start, result_end;
    if (result) {
        result_start = p1;
	result_end = p2;
    }

    // Get more convenient variables
    const R2Line& line1 = halfspaces[RN_LO].Line();
    const R2Line& line2 = halfspaces[RN_HI].Line();

    // Check span versus LO halfspace
    RNScalar d11 = R2SignedDistance(line1, p1);
    RNScalar d12 = R2SignedDistance(line1, p2);
    if (RNIsNegative(d11, epsilon)) {
        if (RNIsNegative(d12, epsilon)) {
	    return RN_NULL_CLASS_ID;
	}
	else if ((result) && (RNIsPositive(d12, epsilon))) {
	    RNScalar denom = d12 - d11;
	    if (RNIsNotZero(denom)) result_start = (p1 * d12 + p2 * -d11) / denom;
	}
    }
    else if ((result) && (RNIsNegative(d12, epsilon))) {
        RNScalar denom = d11 - d12;
        if (RNIsNotZero(denom)) result_end = (p1 * -d12 + p2 * d11) / denom;
    }

    // Check span versus HI halfspace
    RNScalar d21 = R2SignedDistance(line2, p1);
    RNScalar d22 = R2SignedDistance(line2, p2);
    if (RNIsNegative(d21, epsilon)) {
        if (RNIsNegative(d22, epsilon)) {
	    return RN_NULL_CLASS_ID;
	}
	else if ((result) && (RNIsPositive(d22, epsilon))) {
	    RNScalar denom = d22 - d21;
	    if (RNIsNotZero(denom)) result_start = (p1 * d22 + p2 * -d21) / denom;
	}
    }
    else if ((result) && (RNIsNegative(d22, epsilon))) {
        RNScalar denom = d21 - d22;
        if (RNIsNotZero(denom)) result_end = (p1 * -d22 + p2 * d21) / denom;
    }

    // Fill in result
    if (result) result->Reset(result_start, result_end);

    // Return valid intersection
    return R2_SPAN_CLASS_ID;
#else
    // Too much tolerance
    R2Span s(p1, p2);
    if (!s.Clip(halfspaces[RN_LO].Line())) return RN_NULL_CLASS_ID;
    if (!s.Clip(halfspaces[RN_HI].Line())) return RN_NULL_CLASS_ID;
    if (result) *result = s;
    return R2_SPAN_CLASS_ID;
#endif
}



RNClassID R2PointBeam::
Intersects(const R2Shape& shape, RNScalar epsilon) const
{
    RNAbort("Not implemented");
    return RN_UNKNOWN_CLASS_ID;
}



void R2PointBeam::
Draw(const R2Box& mask) const
{
    // Check if beam is trimmed
    if (flags[R2_POINTBEAM_TRIMMED]) {
        // Make a polygon from the mask
        R2Polygon polygon;
	polygon.InsertVertex(new R2Vertex(mask.XMin(), mask.YMin()));
	polygon.InsertVertex(new R2Vertex(mask.XMax(), mask.YMin()));
	polygon.InsertVertex(new R2Vertex(mask.XMax(), mask.YMax()));
	polygon.InsertVertex(new R2Vertex(mask.XMin(), mask.YMax()));

        // Clip polygon to halfspaces
	polygon.Clip(halfspaces[0].Line());
	polygon.Clip(halfspaces[1].Line());

        // Draw polygon
	polygon.Draw();

        // Delete polygon vertices
	for (int i = 0; i < polygon.NVertices(); i++) 
	    delete polygon.Vertices().Kth(i);
    }
    else {
        // Draw mask
        mask.Draw();
    }
}



void R2PointBeam::
Draw(const R2Polygon& mask) const
{
    // Check if beam is trimmed
    if (flags[R2_POINTBEAM_TRIMMED]) {
        // Make a copy of the mask polygon
        R2Polygon polygon;
	for (int i = 0; i < mask.NVertices(); i++) {
	    const R2Point& p = mask.Point(i);
	    polygon.InsertVertex(new R2Vertex(p));
	}

        // Clip polygon to halfspaces
	polygon.Clip(halfspaces[0].Line());
	polygon.Clip(halfspaces[1].Line());

        // Draw polygon
	polygon.Draw();

        // Delete polygon vertices
	for (i = 0; i < polygon.NVertices(); i++) 
	    delete polygon.Vertices().Kth(i);
    }
    else {
        // Draw mask
        mask.Draw();
    }
}



void R2PointBeam::
Draw(const R2Wing *wing, R2WingFace *face) const
{
    // Enable OpenGL clip planes
    if (IsTrimmed()) {
#       if (RN_2D_GRFX == RN_OPENGL)
            double plane[4];
	    plane[2] = 0.0;
	    plane[0] = Halfspace(RN_LO).Line().A();
	    plane[1] = Halfspace(RN_LO).Line().B();
	    plane[3] = Halfspace(RN_LO).Line().C();
	    glClipPlane(GL_CLIP_PLANE0, plane);
	    glEnable(GL_CLIP_PLANE0);
	    plane[0] = Halfspace(RN_HI).Line().A();
	    plane[1] = Halfspace(RN_HI).Line().B();
	    plane[3] = Halfspace(RN_HI).Line().C();
	    glClipPlane(GL_CLIP_PLANE1, plane);
	    glEnable(GL_CLIP_PLANE1);
#       endif
    }

    // Draw face clipped to beam
    wing->DrawFace(face);

    // Disable OpenGL clip planes
    if (IsTrimmed()) {
#       if (RN_2D_GRFX == RN_OPENGL)
	    glDisable(GL_CLIP_PLANE0);
	    glDisable(GL_CLIP_PLANE1);
#       endif
    }
}



void R2PointBeam::
Outline(void) const
{
    // Check if beam is trimmed
    if (flags[R2_POINTBEAM_TRIMMED]) {
        // Draw beam halfspace lines
        R2BeginLine();
	R2LoadPoint(source + halfspaces[0].Line().Vector());
	R2LoadPoint(source);
	R2LoadPoint(source + halfspaces[1].Line().Vector());
	R2EndLine();
    }
}



#if FALSE

void R2PointBeam::
Outline(const R2Brep *brep, const R2BrepFace *face) const
{
    // Create spans along beam halfspace lines
    RNBoolean valid0 = TRUE;
    RNBoolean valid1 = TRUE;
    RNScalar length = 100.0 * brep->BBox().DiagonalLength();
    R2Span span0(Source(), Source() + Halfspace(0).Line().Vector() * length);
    R2Span span1(Source(), Source() - Halfspace(1).Line().Vector() * length);

#if TRUE
    // Clip spans to face edges
    R2BrepEdge *edge;
    RNIterator iterator;
    R2_FOR_EACH_BREP_FACE_EDGE(*brep, face, edge, iterator) {
        R2BrepVertex *v1 = brep->VertexOnEdge(edge, face, RN_CW);
        R2BrepVertex *v2 = brep->VertexOnEdge(edge, face, RN_CCW);
        R2Line edge_line(brep->VertexPosition(v1), brep->VertexPosition(v2));
	if ((valid0) && (!span0.Clip(edge_line))) valid0 = FALSE;
	if ((valid1) && (!span1.Clip(edge_line))) valid1 = FALSE;
	if (!valid0 && !valid1) break;
    }
#endif

    // Draw spans
    if (valid0) span0.Draw();
    if (valid1) span1.Draw();
}

#endif

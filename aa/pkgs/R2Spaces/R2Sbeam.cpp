/* Source file for the RING span beam class */



/* Include files */

#include "R2Spaces/R2Spaces.h"



/* Public functions */

int 
R2InitSpanBeam()
{
    /* Return success */
    return TRUE;
}



void 
R2StopSpanBeam()
{
}



R2SpanBeam::
R2SpanBeam(void)
{
}



R2SpanBeam::
R2SpanBeam(const R2Span& source)
    : source(source),
      flags(RN_NO_FLAGS)
{
    // Initialize hulls
    hulls[0].Insert(source.End());
    hulls[1].Insert(source.Start());
}



R2SpanBeam::
R2SpanBeam(const R2Span& source, const R2Halfspace& h0, const R2Halfspace& h1)
    : source(source),
      flags(R2_SPANBEAM_TRIMMED)
{
    // Initialize halfspaces
    halfspaces[RN_LO] = h0;
    halfspaces[RN_HI] = h1;

    // Initialize hulls
    hulls[0].Insert(source.End());
    hulls[1].Insert(source.Start());
}



R2SpanBeam::
R2SpanBeam(const R2SpanBeam& beam)
    : source(beam.source),
      flags(beam.flags)
{
    // Copy halfspaces
    if (flags[R2_SPANBEAM_TRIMMED]) {
        halfspaces[RN_LO] = beam.halfspaces[RN_LO];
	halfspaces[RN_HI] = beam.halfspaces[RN_HI];
    }

    // Copy hulls
    hulls[RN_LO] = beam.hulls[RN_LO];
    hulls[RN_HI] = beam.hulls[RN_HI];
}



void R2SpanBeam::
Trim(const R2Span& span)
{
    // Order span endpoints
    const R2Point *points[2];
    RNScalar d0 = R2SignedDistance(span.Line(), source.Start());
    RNScalar d1 = R2SignedDistance(span.Line(), source.End());
    if (RNIsNegative(d0)) points[0] = &span.End();
    else if (RNIsPositive(d0)) points[0] = &span.Start();
    else points[0] = (RNIsPositive(d1)) ? &span.Start() : &span.End();
    if (RNIsNegative(d1)) points[1] = &span.Start();
    else if (RNIsPositive(d1)) points[1] = &span.End();
    else points[1] = (RNIsPositive(d0)) ? &span.End() : &span.Start();

    // Trim with ordered points
    Trim(*(points[0]), *(points[1]));
}



void R2SpanBeam::
Trim(const R2Point& p1, const R2Point& p2)
{
    // Update halfspaces
    if (flags[R2_SPANBEAM_TRIMMED]) {
        // Remember halfspaces
        R2Halfspace prev_halfspaces[2];
	prev_halfspaces[0] = halfspaces[0];
	prev_halfspaces[1] = halfspaces[1];

	// Update left halfspace
	if (R2SignedDistance(halfspaces[0].Line(), p1) > 0.0) {
	    for (int k = 0; k <= hulls[1].NEntries()-2; k++) {
	        R2Vector v1 = hulls[1].Kth(k+1) - hulls[1].Kth(k);
		R2Vector v2 = p1 - hulls[1].Kth(k+1);
		if ((v1 % v2) >= 0.0) break;
	    }
	    halfspaces[0].Reset(R2Line(hulls[1].Kth(k), p1));
	}

	// Update right halfspace
	if (R2SignedDistance(halfspaces[1].Line(), p2) > 0.0) {
	    for (int k = 0; k <= hulls[0].NEntries()-2; k++) {
	        R2Vector v1 = hulls[0].Kth(k+1) - hulls[0].Kth(k);
		R2Vector v2 = p2 - hulls[0].Kth(k+1);
		if ((v1 % v2) <= 0.0) break;
	    }
	    halfspaces[1].Reset(R2Line(p2, hulls[0].Kth(k)));
	}

	// Update left hull
	if (R2SignedDistance(prev_halfspaces[0].Line(), p1) >= 0.0) {
	    for (int k = hulls[0].NEntries()-2; k >= 0; k--) {
	        R2Vector v1 = hulls[0].Kth(k+1) - hulls[0].Kth(k);
		R2Vector v2 = p1 - hulls[0].Kth(k+1);
		if ((v1 % v2) > 0.0) break;
	    }
	    hulls[0].Truncate(k+2);
	    hulls[0].Insert(p1);
	}

	// Update right hull
	if (R2SignedDistance(prev_halfspaces[1].Line(), p2) >= 0.0) {
	    for (int k = hulls[1].NEntries()-2; k >= 0; k--) {
	        R2Vector v1 = hulls[1].Kth(k+1) - hulls[1].Kth(k);
		R2Vector v2 = p2 - hulls[1].Kth(k+1);
		if ((v1 % v2) < 0.0) break;
	    }
	    hulls[1].Truncate(k+2);
	    hulls[1].Insert(p2);
	}
    }
    else {
        // Update halfspaces
	halfspaces[0].Reset(R2Line(source.Start(), p1));
	halfspaces[1].Reset(R2Line(p2, source.End()));

	// Update hulls
	hulls[0].Insert(p1);
	hulls[1].Insert(p2);

	// Update flags
	flags.Add(R2_SPANBEAM_TRIMMED);
    }

    // Just checking
    // assert((halfspaces[0].Normal() % halfspaces[1].Normal()) <= 0.0);
}



void R2SpanBeam::
Mirror(const R2Line& line)
{
    // Mirror and flip source
    source.Mirror(line);
    source.Flip();

    // Mirror, flip and swap halfspaces
    if (flags[R2_SPANBEAM_TRIMMED]) {
        // Mirror halfspaces
        halfspaces[0].Mirror(line);
	halfspaces[1].Mirror(line);

	// Flip and swap halfspaces
	R2Halfspace swap = -(halfspaces[0]);
	halfspaces[0] = -(halfspaces[1]);
	halfspaces[1] = swap;
    }

    // Swap and mirror hulls
#if TRUE
    RNSwap(&(hulls[0]), &(hulls[1]), NULL, sizeof(RNArray<R2Point>));
    for (int i = 0; i < hulls[0].NEntries(); i++) 
        hulls[0].Kth(i).Mirror(line);
    for (i = 0; i < hulls[1].NEntries(); i++) 
        hulls[1].Kth(i).Mirror(line);
#else
    int n0 = hulls[0].NEntries();
    int n1 = hulls[0].NEntries();
    for (int i = 0; i < n0; i++) {
        R2Point p = hulls[0].Kth(i);
	p.Mirror(line);
	hulls[1].Insert(p);
    }
    for (i = 0; i < n1; i++) {
        R2Point p = hulls[1].Kth(i);
	p.Mirror(line);
	hulls[0].Insert(p);
    }
    hulls[0].Shift(-n0);
    hulls[1].Shift(-n1);
#endif

    // Just checking
    // assert((halfspaces[0].Normal() % halfspaces[1].Normal()) >= 0.0);
}



void R2SpanBeam::
TrimAndMirror(const R2Span& span)
{
    // Trim and mirror beam
    Trim(span);
    Mirror(span.Line());
}



void R2SpanBeam::
Draw(void) const
{
    // Draw infinite span beam
    Draw(R2infinite_box);
}



void R2SpanBeam::
Draw(const R2Box& mask) const
{
    // Check if beam is trimmed
    if (flags[R2_SPANBEAM_TRIMMED]) {
        // Make a polygon from the mask
        R2Polygon polygon;
	polygon.InsertVertex(new R2Vertex(mask.XMin(), mask.YMin()));
	polygon.InsertVertex(new R2Vertex(mask.XMax(), mask.YMin()));
	polygon.InsertVertex(new R2Vertex(mask.XMax(), mask.YMax()));
	polygon.InsertVertex(new R2Vertex(mask.XMin(), mask.YMax()));
	
	// Clip polygon to side halfspaces
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



void R2SpanBeam::
Draw(const R2Polygon& mask) const
{
    // Check if beam is trimmed
    if (flags[R2_SPANBEAM_TRIMMED]) {
        // Make a copy of the mask polygon
        R2Polygon polygon;
	for (int i = 0; i < mask.NVertices(); i++) {
	    const R2Point& p = mask.Point(i);
	    polygon.InsertVertex(new R2Vertex(p));
	}

	// Clip polygon to side halfspaces
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



void R2SpanBeam::
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



void R2SpanBeam::
Outline(void) const
{
    // Check if beam is trimmed
    if (flags[R2_POINTBEAM_TRIMMED]) {
        // Draw beam halfspace lines
        R2BeginLine();
	R2LoadPoint(source.Start() + halfspaces[0].Line().Vector());
	R2LoadPoint(source.Start());
	R2LoadPoint(source.End());
	R2LoadPoint(source.End() + halfspaces[1].Line().Vector());
	R2EndLine();
    }
}



RNBoolean R2SpanBeam::
Contains(const R2Point& point, RNScalar epsilon) const
{
    // Return whether all active halfspaces contain point
    if (!flags[R2_SPANBEAM_TRIMMED]) return TRUE;
    if (!R2Contains(halfspaces[RN_LO], point)) return FALSE;
    if (!R2Contains(halfspaces[RN_HI], point)) return FALSE;
    return TRUE;
}



RNBoolean R2SpanBeam::
Contains(const R2Shape& shape, RNScalar epsilon) const
{
    // Return whether all active halfspaces contain shape
    if (!flags[R2_SPANBEAM_TRIMMED]) return TRUE;
    if (!R2Contains(halfspaces[RN_LO], shape)) return FALSE;
    if (!R2Contains(halfspaces[RN_HI], shape)) return FALSE;
    return TRUE;
}



RNClassID R2SpanBeam::
Intersects(const R2Span& span, R2Span *result, RNScalar epsilon) const
{
    // Check whether beam is trimmed
    if (!flags[R2_SPANBEAM_TRIMMED]) {
        // Return whole span
        if (result) *result = span;
	return R2_SPAN_CLASS_ID;
    }
    else {
        // Compute intersection
        return Intersects(span.Start(), span.End(), result, epsilon);
    }
}



RNClassID R2SpanBeam::
Intersects(const R2Point& p1, const R2Point& p2, R2Span *result, RNScalar epsilon) const
{
    // Check whether beam is trimmed
    if (!flags[R2_SPANBEAM_TRIMMED]) {
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
	    RNScalar denom = (d12 - d11);
	    if (RNIsNotZero(denom)) result_start = (p1 * d12 + p2 * -d11) / denom;
	}
    }
    else if ((result) && (RNIsNegative(d12, epsilon))) {
        RNScalar denom = (d11 - d12);
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
	    RNScalar denom = (d22 - d21);
	    if (RNIsNotZero(denom)) result_start = (p1 * d22 + p2 * -d21) / denom;
	}
    }
    else if ((result) && (RNIsNegative(d22, epsilon))) {
        RNScalar denom = (d21 - d22);
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



RNClassID R2SpanBeam::
Intersects(const R2Shape& shape, RNScalar epsilon) const
{
    RNAbort("Not implemented");
    return RN_UNKNOWN_CLASS_ID;
}




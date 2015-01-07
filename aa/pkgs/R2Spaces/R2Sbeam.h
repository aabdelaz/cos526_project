/* Include file for the RING span beam class */



/* Initialization functions */

int R2InitSpanBeam();
void R2StopSpanBeam();



/* Class definition */

class R2SpanBeam : public R2Beam {
    public:
        // Constructors/destructors
        R2SpanBeam(void);
        R2SpanBeam(const R2Span& source);
        R2SpanBeam(const R2Span& source, const R2Halfspace& h0, const R2Halfspace& h1);
        R2SpanBeam(const R2SpanBeam& beam);

        // Property functions/operators
	const R2Span& Source(void) const;
	const R2Halfspace& Halfspace(RNDirection dir) const;
	const RNBoolean IsSourcePoint(void) const;
	const RNBoolean IsTrimmed(void) const;
	
        // Manipulation functions/operators
	virtual void Trim(const R2Span& span);
	virtual void Trim(const R2Point& p1, const R2Point& p2);
	virtual void Mirror(const R2Line& line);
	virtual void TrimAndMirror(const R2Span& span);
	
	// Relationship functions/operators
	virtual RNBoolean Contains(const R2Point& point, RNScalar epsilon = RN_EPSILON) const;
	virtual RNBoolean Contains(const R2Shape& shape, RNScalar epsilon = RN_EPSILON) const;
	virtual RNClassID Intersects(const R2Point& p1, const R2Point& p2, R2Span *result = NULL, RNScalar epsilon = RN_EPSILON) const;
	virtual RNClassID Intersects(const R2Span& span, R2Span *result = NULL, RNScalar epsilon = RN_EPSILON) const;
	virtual RNClassID Intersects(const R2Shape& shape, RNScalar epsilon = RN_EPSILON) const;
	
        // Draw functions/operators
        virtual void Draw(void) const;
        virtual void Draw(const R2Box& mask) const;
        virtual void Draw(const R2Polygon& mask) const;
	virtual void Draw(const R2Wing *wing, R2WingFace *face) const;
        virtual void Outline(void) const;

    private:
	R2Span source;
	R2Halfspace halfspaces[2];
	RNArray<R2Point> hulls[2];
	RNFlags flags;
};



/* Private flags */

#define R2_SPANBEAM_TRIMMED 0x1



/* Inline functions */

inline const R2Span& R2SpanBeam::
Source(void) const
{
    // Return source of beam
    return source;
}



inline const R2Halfspace& R2SpanBeam::
Halfspace(RNDirection dir) const
{
    // Return halfspace
    return halfspaces[dir];
}



inline const RNBoolean R2SpanBeam::
IsSourcePoint(void) const
{
    // Return whether source is point
    return source.IsPoint();
}



inline const RNBoolean R2SpanBeam::
IsTrimmed(void) const
{
    // Return whether halfspaces are active
    return flags[R2_SPANBEAM_TRIMMED];
}





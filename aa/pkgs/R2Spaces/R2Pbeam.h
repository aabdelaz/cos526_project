/* Include file for the RING point beam class */



/* Initialization functions */

int R2InitPointBeam();
void R2StopPointBeam();



/* Class definition */

class R2PointBeam : public R2Beam {
    public:
        // Constructors/destructors
        R2PointBeam(void);
        R2PointBeam(const R2Point& source);
        R2PointBeam(const R2Point& source, const R2Halfspace& h0, const R2Halfspace& h1);
        R2PointBeam(const R2PointBeam& beam);

        // Property functions/operators
	const R2Point& Source(void) const;
	const R2Halfspace& Halfspace(RNDirection dir) const;
	const RNBoolean IsSourcePoint(void) const;
	const RNBoolean IsTrimmed(void) const;
	
        // Manipulation functions/operators
	virtual void Trim(const R2Span& span);
	virtual void Trim(const R2Point& p1, const R2Point& p2);
	virtual void Mirror(const R2Line& line);
	virtual void TrimAndMirror(const R2Span& span);
	virtual void Reset(const R2Point& source, const R2Halfspace& h0, const R2Halfspace& h1);
	
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
	R2Point source;
	R2Halfspace halfspaces[2];
	RNFlags flags;
};



/* Private flags */

#define R2_POINTBEAM_TRIMMED 0x1



/* Inline functions */

inline const R2Point& R2PointBeam::
Source(void) const
{
    // Return source of beam
    return source;
}



inline const R2Halfspace& R2PointBeam::
Halfspace(RNDirection dir) const
{
    // Return halfspace
    return halfspaces[dir];
}



inline const RNBoolean R2PointBeam::
IsSourcePoint(void) const
{
    // Return whether source is point
    return TRUE;
}



inline const RNBoolean R2PointBeam::
IsTrimmed(void) const
{
    // Return whether halfspaces are active
    return flags[R2_POINTBEAM_TRIMMED];
}



inline void R2PointBeam::
TrimAndMirror(const R2Span& span)
{
    // Trim and mirror beam
    Trim(span);
    Mirror(span.Line());
}



inline void R2PointBeam::
Draw(void) const
{
    // Draw infinite point beam
    Draw(R2infinite_box);
}




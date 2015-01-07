/* Include file for the RING point beam class */



/* Initialization functions */

int R2InitBeam();
void R2StopBeam();



/* Class definition */

class R2Beam {
    public:
        // Manipulation functions/operators
	virtual void Trim(const R2Span& span) = 0;
	virtual void Mirror(const R2Line& line) = 0;
	virtual void TrimAndMirror(const R2Span& span) = 0;

	// Relationship functions/operators
	virtual RNBoolean Contains(const R2Point& point, RNScalar epsilon = RN_EPSILON) const = 0;
	virtual RNBoolean Contains(const R2Shape& shape, RNScalar epsilon = RN_EPSILON) const = 0;
	virtual RNClassID Intersects(const R2Point& p1, const R2Point& p2, R2Span *result = NULL, RNScalar epsilon = RN_EPSILON) const = 0;
	virtual RNClassID Intersects(const R2Span& span, R2Span *result = NULL, RNScalar epsilon = RN_EPSILON) const = 0;
	virtual RNClassID Intersects(const R2Shape& shape, RNScalar epsilon = RN_EPSILON) const = 0;
	
        // Draw functions/operators
        virtual void Draw(void) const = 0;
        virtual void Draw(const R2Box& mask) const = 0;
        virtual void Draw(const R2Polygon& mask) const = 0;
        virtual void Outline(void) const = 0;
};




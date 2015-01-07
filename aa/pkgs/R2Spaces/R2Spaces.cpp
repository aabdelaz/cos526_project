/* Source file for RING spaces module */



/* Include files */

#include "R2Spaces.h"



/* Private variables */

static int R2spaces_active_count = 0;



int R2InitSpaces()
{
    // Check whether are already initialized 
    if ((R2spaces_active_count++) > 0) return TRUE;

    // Initialize dependencies
    if (!R2InitShapes()) return FALSE;

    // Initialize submodules 
    // ??? 

    // return OK status 
    return TRUE;
}



void R2StopSpaces()
{
    // Check whether have been initialized 
    if ((--R2spaces_active_count) > 0) return;

    // Stop submodules 
    // ???

    // Stop dependencies
    R2StopShapes();
}





/***********************************************************************
* This is here because SGI compiler includes .C files with templates
* and the linker generates multiple definition warnings
***********************************************************************/

int 
R2InitWing()
{
    // Return success 
    return TRUE;
}



void 
R2StopWing()
{
}



void 
R2PrintWingFace(const R2Wing *wing, const R2WingFace *face)
{
    // Print polygon
    R2WingEdge *edge;
    RNIterator iterator;
    int nvertices = 0;
    R2_FOR_EACH_WING_FACE_EDGE(*wing, face, edge, iterator) {
        R2WingVertex *v = wing->VertexOnEdge(edge, face, RN_CCW);
	const R2Point& p = wing->VertexPosition(v);
        printf("( %.4f %.4f ) ", p.X(), p.Y());
	nvertices++;
    }
    printf("%d\n", nvertices);
}



void 
R2PrintWingEdge(const R2Wing *wing, const R2WingEdge *edge)
{
    // Print span
    R2WingVertex *v1 = wing->VertexOnEdge(edge);
    const R2Point& p1 = wing->VertexPosition(v1);
    R2WingVertex *v2 = wing->VertexAcrossEdge(edge, v1);
    const R2Point& p2 = wing->VertexPosition(v2);
    printf("( %.4f %.4f ) ( %.4f %.4f )\n", p1.X(), p1.Y(), p2.X(), p2.Y());
}



void 
R2PrintWingVertex(const R2Wing *wing, const R2WingVertex *vertex)
{
    // Print point
    const R2Point& p = wing->VertexPosition(vertex);
    printf("( %.4f %.4f )\n", p.X(), p.Y());
}




/* Test program for the R2XXXBeam and R2Wing classes of the R2Spaces package */



/* Package include files */

#include "R2Spaces/R2Spaces.h"



/* OpenGL include files */

#if (RN_OS == RN_WINDOWSNT)
#   include <windows.h>
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glaux.h>
#else
#   define CALLBACK 
#   include <GL/gl.h>
#   include <GL/glu.h>
    extern "C" {
#       include <RNUtils/aux.h>
    };
#endif



/* Type definitions */

#define R2Space R2Wing



/* Function declarations */

int InitAll();
void StopAll();
int ParseArgs(int, char **);

int InitInterface();
void StopInterface();
void RunInterface();

void CALLBACK Redraw();
void CALLBACK Reshape(GLsizei, GLsizei);
void CALLBACK StartMove(AUX_EVENTREC *event);
void CALLBACK StopMove(AUX_EVENTREC *event);
void CALLBACK DoMove(AUX_EVENTREC *event);
void CALLBACK StartRotate(AUX_EVENTREC *event);
void CALLBACK StopRotate(AUX_EVENTREC *event);
void CALLBACK DoRotate(AUX_EVENTREC *event);
void CALLBACK StartScale(AUX_EVENTREC *event);
void CALLBACK StopScale(AUX_EVENTREC *event);
void CALLBACK DoScale(AUX_EVENTREC *event);
void CALLBACK StartTranslate(AUX_EVENTREC *event);
void CALLBACK StopTranslate(AUX_EVENTREC *event);
void CALLBACK DoTranslate(AUX_EVENTREC *event);
void CALLBACK ToggleSource();
void CALLBACK ToggleReceiver();
void CALLBACK ToggleOccluders();
void CALLBACK TogglePortals();
void CALLBACK ToggleFaces();
void CALLBACK ToggleBeams();
void CALLBACK ToggleSourceImages();
void CALLBACK ToggleSpeculars();
void CALLBACK DecreaseSpeculars();
void CALLBACK IncreaseSpeculars();
void CALLBACK ToggleDiffuses();
void CALLBACK DecreaseDiffuses();
void CALLBACK IncreaseDiffuses();
void CALLBACK ToggleTransmissions();
void CALLBACK DecreaseTransmissions();
void CALLBACK IncreaseTransmissions();
void CALLBACK ToggleDiffractions();
void CALLBACK DecreaseDiffractions();
void CALLBACK IncreaseDiffractions();
void FlushMouseMovements();

R2Space *CreateSpace(void);
void DeleteSpace(R2Space *space);

void TraceBeams(R2Space& space, 
    const R2Point& source_point, R2WingFace *&source_face,
    const R2Point& receiver_point, R2WingFace *&receiver_face);
void TraceBeams(R2Space& space, 
    const R2Point& source_point, R2WingFace *source_face,
    const R2Point& receiver_point, R2WingFace *receiver_face,
    const R2SpanBeam& beam, R2WingFace *face, R2WingEdge *edge, 
    int speculars, int diffuses, int transmissions, int diffractions);
void TraceDiffractionBeams(R2Space& space, 
    const R2Point& source_point, R2WingFace *source_face,
    const R2Point& receiver_point, R2WingFace *receiver_face,
    const R2SpanBeam& beam, R2WingFace *face, R2WingEdge *edge,
    const R2SpanBeam& transmission_beam, R2WingEdge *transmission_edge, RNDirection dir,
    int speculars, int diffuses, int transmissions, int diffractions);



/* Program arguments */

char *filename = NULL;



/* Space variables */

static R2Space *space = NULL;



/* Mouse variables */

GLint xmouse = 0;
GLint ymouse = 0;



/* Viewing variables */

int rotating = 0;
int scaling = 0;
int translating = 0;
float xcenter = 0.0;
float ycenter = 0.0;
float zcenter = 0.0;
float xtranslate = 0.0;
float ytranslate = 0.0;
float ztranslate = -8.0;
float xscale = 1.0;
float yscale = 1.0;
float zscale = 1.0;
float xrotate = 0.0;
float yrotate = 0.0;
float zrotate = 0.0;



/* Display variables */

int max_speculars = 0;
int max_diffuses = 0;
int max_transmissions = 0;
int max_diffractions = 0;
R2Point source_point(0.0, 0.0);
R2Point receiver_point(0.0, 0.0);
RNBoolean show_source = TRUE;
RNBoolean show_receiver = FALSE;
RNBoolean show_occluders = TRUE;
RNBoolean show_portals = FALSE;
RNBoolean show_faces = FALSE;
RNBoolean show_beams = FALSE;
RNBoolean show_source_images = FALSE;
RNBoolean show_speculars = TRUE;
RNBoolean show_diffuses = TRUE;
RNBoolean show_transmissions = TRUE;
RNBoolean show_diffractions = TRUE;
RNBoolean moving_source = FALSE;
RNBoolean moving_receiver = FALSE;
R2WingFace *source_face = NULL;
R2WingFace *receiver_face = NULL;

int found_receiver = FALSE;



int main(int argc, char **argv)
{
    /* Parse command line arguments */
    if (!ParseArgs(argc, argv)) exit(1);

    /* Initialize everything */
    if (!InitAll()) exit(1);

    /* Run the interactive interface */
    RunInterface();

    /* Stop everything */
    StopAll();

    /* Return success */
    return 0;
}



int 
ParseArgs(int argc, char *argv[])
{
    // Check number of arguments
    if ((argc == 2) && (!strcmp(argv[1], "-"))) {
	printf("Usage: ogltest spacename [-source x y] [-receiver x y]\n");
	exit(0);
    }

    // Parse arguments
    argc--; argv++;
    while (argc > 0) {
	if ((*argv)[0] == '-') {
	    if (!strcmp(*argv, "-source")) { 
	        RNCoord x, y;
		argv++; argc--; x = atof(*argv); 
		argv++; argc--; y = atof(*argv); 
		source_point = R2Point(x, y);
	    }
	    else if (!strcmp(*argv, "-receiver")) { 
	        RNCoord x, y;
		argv++; argc--; x = atof(*argv); 
		argv++; argc--; y = atof(*argv); 
		receiver_point = R2Point(x, y);
	    }
	    else if (!strcmp(*argv, "-speculars")) { 
	        argv++; argc--; max_speculars = atoi(*argv); 
	    }
	    else if (!strcmp(*argv, "-diffuses")) { 
	        argv++; argc--; max_diffuses = atoi(*argv); 
	    }
	    else if (!strcmp(*argv, "-transmissions")) { 
	        argv++; argc--; max_transmissions = atoi(*argv); 
	    }
	    else if (!strcmp(*argv, "-diffractions")) { 
	        argv++; argc--; max_diffractions = atoi(*argv); 
	    }
	    argv += 1; argc -= 1;
	}
	else {
            if (!filename) filename = *argv;
	    else RNAbort("Invalid program argument: %s", *argv);
	    argv += 1; argc -= 1;
	}
    }

    // Check arguments
    if (!filename) {
	RNFail("You did not specify a space file name");	
	return FALSE;
    }

    // Return OK status 
    return TRUE;
}



int InitAll()
{
    /* Initialize the R2 library */
    if (!R2InitSpaces()) return FALSE;

    /* Create the space */
    if (!(space = CreateSpace())) return FALSE;

    /* Initialize the user interface */
    if (!InitInterface()) return 0;

    /* Return OK status */
    return 1;
}



void StopAll()
{
    /* Stop the user interface */
    StopInterface();

    /* Delete the space */
    DeleteSpace(space);

    /* Stop the R2 library */
    R2StopSpaces();
}



R2Space *
CreateSpace(void)
{
    // Read array of spans
    RNArray<R2Span *> spans;
    if (!R2ReadSpans(spans, filename)) {
        RNFail("Unable to read file %s", filename);
	return NULL;
    }

    // Compute bounding box of spans
    R2Box bbox(R2null_box);
    for (int i = 0; i < spans.NEntries(); i++) 
        bbox.Union(spans.Kth(i)->BBox());

#if FALSE
    // Grow bounding box by factor of 2
    R2Point c = bbox.Centroid();
    RNScalar rx = 2 * bbox.XRadius();
    RNScalar ry = 2 * bbox.YRadius();
    bbox[0][0] = c.X() - rx;
    bbox[0][1] = c.Y() - ry;
    bbox[1][0] = c.X() + rx;
    bbox[1][1] = c.Y() + ry;
#endif

    // Create wing
    R2Wing *wing = new R2Wing(bbox);
    assert(wing->IsValid());

    // Load wing from spans
    if (!R2LoadWing(*wing, spans)) {
        RNFail("Unable to load file %s into wing", filename);
	return NULL;
    }

    // Initialize source point
    source_point = bbox.Centroid();
    receiver_point = bbox.Min();

    // Return wing
    assert(wing->IsValid());
    return wing;
}



void 
DeleteSpace(R2Space *space)
{
    // Delete space
    delete space;
}



int InitInterface()
{
    /* Open window */
    auxInitDisplayMode(AUX_DOUBLE | AUX_RGB | AUX_DEPTH);
    auxInitPosition(100, 100, 512, 512);
    auxInitWindow("OpenGL test program");

    /* Set initial viewing parameters */
    int viewport_width = 512; 
    int viewport_height = 512; 
    const R2Box& bbox = space->BBox();
    double window_width = bbox.XMax() - bbox.XMin();
    double window_height = bbox.YMax() - bbox.YMin();
    double window_xcenter = 0.5 * (bbox.XMin() + bbox.XMax());
    double window_ycenter = 0.5 * (bbox.YMin() + bbox.YMax());
    float xscale = viewport_width / window_width;
    float yscale = viewport_height / window_height;
    float scale = (xscale > yscale) ? 0.9 * yscale: 0.9 * xscale;
    R2SetViewport(0, 0, viewport_width, viewport_height);
    R2SetWindow(window_xcenter - 0.5 * viewport_width / scale,
		window_ycenter - 0.5 * viewport_height / scale,
		window_xcenter + 0.5 * viewport_width / scale,
		window_ycenter + 0.5 * viewport_height / scale);

    /* Initialize graphics modes */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    /* Return OK status */
    return 1;
}



void StopInterface()
{
}



void RunInterface()
{
    /* Setup callback functions */
    auxReshapeFunc (Reshape);
    auxMouseFunc (AUX_LEFTBUTTON, AUX_MOUSEDOWN, StartMove);
    auxMouseFunc (AUX_LEFTBUTTON, AUX_MOUSEUP, StopMove);
    auxMouseFunc (AUX_LEFTBUTTON, AUX_MOUSELOC, DoMove);
    auxMouseFunc (AUX_MIDDLEBUTTON, AUX_MOUSEDOWN, StartScale);
    auxMouseFunc (AUX_MIDDLEBUTTON, AUX_MOUSEUP, StopScale);
    auxMouseFunc (AUX_MIDDLEBUTTON, AUX_MOUSELOC, DoScale);
    auxMouseFunc (AUX_RIGHTBUTTON, AUX_MOUSEDOWN, StartTranslate);
    auxMouseFunc (AUX_RIGHTBUTTON, AUX_MOUSEUP, StopTranslate);
    auxMouseFunc (AUX_RIGHTBUTTON, AUX_MOUSELOC, DoTranslate);
    auxKeyFunc(AUX_s, ToggleSource);
    auxKeyFunc(AUX_S, ToggleSource);
    auxKeyFunc(AUX_x, ToggleReceiver);
    auxKeyFunc(AUX_X, ToggleReceiver);
    auxKeyFunc(AUX_o, ToggleOccluders);
    auxKeyFunc(AUX_O, ToggleOccluders);
    auxKeyFunc(AUX_p, TogglePortals);
    auxKeyFunc(AUX_P, TogglePortals);
    auxKeyFunc(AUX_f, ToggleFaces);
    auxKeyFunc(AUX_F, ToggleFaces);
    auxKeyFunc(AUX_b, ToggleBeams);
    auxKeyFunc(AUX_B, ToggleBeams);
    auxKeyFunc(AUX_i, ToggleSourceImages);
    auxKeyFunc(AUX_I, ToggleSourceImages);
    auxKeyFunc(AUX_r, ToggleSpeculars);
    auxKeyFunc(AUX_R, ToggleSpeculars);
    auxKeyFunc(AUX_DOWN, DecreaseSpeculars);
    auxKeyFunc(AUX_UP, IncreaseSpeculars);
    auxKeyFunc(AUX_k, ToggleDiffuses);
    auxKeyFunc(AUX_K, ToggleDiffuses);
    auxKeyFunc(AUX_2, DecreaseDiffuses);
    auxKeyFunc(AUX_8, IncreaseDiffuses);
    auxKeyFunc(AUX_t, ToggleTransmissions);
    auxKeyFunc(AUX_T, ToggleTransmissions);
    auxKeyFunc(AUX_LEFT, DecreaseTransmissions);
    auxKeyFunc(AUX_RIGHT, IncreaseTransmissions);
    auxKeyFunc(AUX_d, ToggleDiffractions);
    auxKeyFunc(AUX_D, ToggleDiffractions);
    auxKeyFunc(AUX_4, DecreaseDiffractions);
    auxKeyFunc(AUX_6, IncreaseDiffractions);
    
    /* Run main loop -- never returns */
    auxMainLoop(Redraw);
}



void CALLBACK Reshape(GLsizei w, GLsizei h)
{
    // Compute scale factor
    float scale = (RNgrfx_window_xscale > RNgrfx_window_yscale) ? RNgrfx_window_xscale: RNgrfx_window_yscale;

    /* Reset GL viewport */
    glViewport(0, 0, w, h);

    // Reset viewport
    R2SetViewport(0, 0, w, h);

    // Reset window
    R2SetWindow(RNgrfx_window_xcenter - 0.5 * w / scale,
		RNgrfx_window_ycenter - 0.5 * h / scale,
		RNgrfx_window_xcenter + 0.5 * w / scale,
		RNgrfx_window_ycenter + 0.5 * h / scale);
}



void CALLBACK Redraw()
{
    /* Set viewing transformation */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(RNgrfx_window_xmin, RNgrfx_window_xmax, RNgrfx_window_ymin, RNgrfx_window_ymax, 0.1, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -1.0);

    /* Clear window */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw beams
    if (show_beams) {
        TraceBeams(*space, source_point, source_face, receiver_point, receiver_face);
    }

    // Draw portals
    if (show_portals) {
	R2WingEdge *edge;
        RNIterator iterator;
	R2_FOR_EACH_WING_EDGE(*space, edge, iterator) {
	    if (space->IsEdgeOpaque(edge)) continue;
	    RNLoadRgb(0.2, 0.2, 1.0);
	    space->DrawEdge(edge);
	}
    }

    // Draw occluders
    if (show_occluders) {
	R2WingEdge *edge;
        RNIterator iterator;
	R2_FOR_EACH_WING_EDGE(*space, edge, iterator) {
	    if (!space->IsEdgeOpaque(edge)) continue;
	    RNLoadRgb(1.0, 1.0, 1.0);
	    space->DrawEdge(edge);
	}
    }

    // Draw source point
    if (show_source) {
        RNLoadRgb(1.0, 1.0, 0.0);
	R2Vector v = 0.01 * space->BBox().LongestAxisLength() * R2ones_vector;
	R2Box(source_point - v, source_point + v).Draw();
    }

    // Draw receiver point
    if (show_receiver) {
        if (found_receiver) RNLoadRgb(1.0, 0.8, 1.0);
	else RNLoadRgb(0.8, 0.0, 0.8);
	R2Vector v = 0.01 * space->BBox().LongestAxisLength() * R2ones_vector;
	R2Box(receiver_point - v, receiver_point + v).Draw();
    }

    /* Flush mouse movements from input queue ??? */
    FlushMouseMovements();

    /* Swap buffers */
    auxSwapBuffers();
}    



void CALLBACK StartMove(AUX_EVENTREC *event)
{
    /* Get mouse viewport coordinates */
    int x = event->data[AUX_MOUSEX];
    int y = RNgrfx_viewport_height - event->data[AUX_MOUSEY];

    /* Convert to window coordinates */
    float window_xpos, window_ypos;
    R2ViewportToWindow(x, y, &window_xpos, &window_ypos);
    RNLength tolerance = 0.01 * space->BBox().LongestAxisLength();

    /* Pick source */
    if (show_source) {
        RNLength distance = R2Distance(R2Point(window_xpos, window_ypos), source_point);
	moving_source = (distance < tolerance);
    }

    /* Pick receiver */
    if (show_receiver && !moving_source) {
        RNLength distance = R2Distance(R2Point(window_xpos, window_ypos), receiver_point);
	moving_receiver = (distance < tolerance);
    }
}



void CALLBACK StopMove(AUX_EVENTREC *event)
{
    /* Stop moving source and receiver */
    moving_source = FALSE;
    moving_receiver = FALSE;
}



void CALLBACK DoMove(AUX_EVENTREC *event)
{
    // Check if moving source or receiver
    if (!moving_source && !moving_receiver) return;

    /* Flush mouse movements from input queue ??? */
    FlushMouseMovements();

    // Get mouse viewport coordinates
    int x = event->data[AUX_MOUSEX];
    int y = RNgrfx_viewport_height - event->data[AUX_MOUSEY];

    // Convert to window coordinates
    float window_xpos, window_ypos;
    R2ViewportToWindow(x, y, &window_xpos, &window_ypos);
    R2Point point(window_xpos, window_ypos);

    // Move source or receiver
    if (moving_source) source_point = point;
    else if (moving_receiver) receiver_point = point;
}



void CALLBACK StartRotate(AUX_EVENTREC *event)
{
    /* Start rotating */
    rotating = 1;

    /* Remember mouse coordinates */
    xmouse = event->data[AUX_MOUSEX];
    ymouse = RNgrfx_viewport_height - event->data[AUX_MOUSEY];
}



void CALLBACK StopRotate(AUX_EVENTREC * /* event */)
{
    /* Stop rotating */
    rotating = 0;
}



void CALLBACK DoRotate(AUX_EVENTREC *event)
{
    /* Check whether we are rotating */
    if (!rotating) return;

    /* Flush mouse movements from input queue ??? */
    FlushMouseMovements();

    /* Get mouse coordinates */
    int x = event->data[AUX_MOUSEX];
    int y = RNgrfx_viewport_height - event->data[AUX_MOUSEY];

    /* Update rotation */
    xrotate += 0.5 * (float) (ymouse - y);
    zrotate += 0.5 * (float) (x - xmouse);

    /* Remember mouse coordinates */
    xmouse = x;
    ymouse = y;
}



void CALLBACK StartScale(AUX_EVENTREC *event)
{
    /* Start scaling */
    scaling = 1;

    /* Remember mouse coordinates */
    xmouse = event->data[AUX_MOUSEX];
    ymouse = RNgrfx_viewport_height - event->data[AUX_MOUSEY];
}



void CALLBACK StopScale(AUX_EVENTREC * /* event */)
{
    /* Stop scaling */
    scaling = 0;
}



void CALLBACK DoScale(AUX_EVENTREC *event)
{
    /* Check whether we are scaling */
    if (!scaling) return;

    /* Flush mouse movements from input queue ??? */
    FlushMouseMovements();

    /* Get mouse coordinates */
    int x = event->data[AUX_MOUSEX];
    int y = RNgrfx_viewport_height - event->data[AUX_MOUSEY];

    /* Scale window */
    float scale = exp(-2.0 * (float) (x - xmouse) / (float) RNgrfx_viewport_width);
    R2ScaleWindow(scale, scale);

    /* Remember mouse coordinates */
    xmouse = x;
    ymouse = y;
}



void CALLBACK StartTranslate(AUX_EVENTREC *event)
{
    /* Start translating */
    translating = 1;

    /* Remember mouse coordinates */
    xmouse = event->data[AUX_MOUSEX];
    ymouse = RNgrfx_viewport_height - event->data[AUX_MOUSEY];
}



void CALLBACK StopTranslate(AUX_EVENTREC * /* event */)
{
    /* Stop translating */
    translating = 0;
}



void CALLBACK DoTranslate(AUX_EVENTREC *event)
{
    /* Check whether we are translating */
    if (!translating) return;

    /* Flush mouse movements from input queue ??? */
    FlushMouseMovements();

    /* Get mouse coordinates */
    int x = event->data[AUX_MOUSEX];
    int y = RNgrfx_viewport_height - event->data[AUX_MOUSEY];

    /* Translate window */
    float xtranslate = RNgrfx_window_width * (float) (x - xmouse) / (float) RNgrfx_viewport_width;
    float ytranslate = RNgrfx_window_height * (float) (ymouse - y) / (float) RNgrfx_viewport_height;
    R2TranslateWindow(-xtranslate, ytranslate);

    /* Remember mouse coordinates */
    xmouse = x;
    ymouse = y;
}



void CALLBACK ToggleSource()
{
    /* Toggle variable */
    show_source = !show_source;
}



void CALLBACK ToggleReceiver()
{
    /* Toggle variable */
    show_receiver = !show_receiver;
}



void CALLBACK ToggleOccluders()
{
    /* Toggle variable */
    show_occluders = !show_occluders;
}



void CALLBACK TogglePortals()
{
    /* Toggle variable */
    show_portals = !show_portals;
}



void CALLBACK ToggleFaces()
{
    /* Toggle variable */
    show_faces = !show_faces;
}



void CALLBACK ToggleBeams()
{
    /* Toggle variable */
    show_beams = !show_beams;
}



void CALLBACK ToggleSourceImages()
{
    /* Toggle variable */
    show_source_images = !show_source_images;
}



void CALLBACK ToggleSpeculars()
{
    /* Toggle variable */
    show_speculars = !show_speculars;
}



void CALLBACK DecreaseSpeculars()
{
    /* Decrease specular reflections */
    if (max_speculars > 0) max_speculars--;
    printf("Maximum number of specular reflections: %d\n", max_speculars);
}



void CALLBACK IncreaseSpeculars()
{
    /* Decrease specular reflections */
    max_speculars++;
    printf("Maximum number of specular reflections: %d\n", max_speculars);
}



void CALLBACK ToggleDiffuses()
{
    /* Toggle variable */
    show_diffuses = !show_diffuses;
}



void CALLBACK DecreaseDiffuses()
{
    /* Decrease diffuse reflections */
    if (max_diffuses > 0) max_diffuses--;
    printf("Maximum number of diffuse reflections: %d\n", max_diffuses);
}



void CALLBACK IncreaseDiffuses()
{
    /* Decrease diffuse reflections */
    max_diffuses++;
    printf("Maximum number of diffuse reflections: %d\n", max_diffuses);
}



void CALLBACK ToggleTransmissions()
{
    /* Toggle variable */
    show_transmissions = !show_transmissions;
}



void CALLBACK DecreaseTransmissions()
{
    /* Decrease transmissions */
    if (max_transmissions > 0) max_transmissions--;
    printf("Maximum number of transmissions: %d\n", max_transmissions);
}



void CALLBACK IncreaseTransmissions()
{
    /* Decrease transmissions */
    max_transmissions++;
    printf("Maximum number of transmissions: %d\n", max_transmissions);
}



void CALLBACK ToggleDiffractions()
{
    /* Toggle variable */
    show_diffractions = !show_diffractions;
}



void CALLBACK DecreaseDiffractions()
{
    /* Decrease diffractions */
    if (max_diffractions > 0) max_diffractions--;
    printf("Maximum number of diffractions: %d\n", max_diffractions);
}



void CALLBACK IncreaseDiffractions()
{
    /* Decrease diffractions */
    max_diffractions++;
    printf("Maximum number of diffractions: %d\n", max_diffractions);
}



void FlushMouseMovements()
{
#if (RN_OS == RN_IRIX)
    /* Flush mouse movements from X input queue ??? */
    XEvent xevent;
    while (XCheckTypedEvent(auxXDisplay(), MotionNotify, &xevent)) ;
#endif
}



void 
TraceBeams(R2Wing& wing, 
    const R2Point& source_point, R2WingFace *&source_face,
    const R2Point& receiver_point, R2WingFace *&receiver_face)
{
    // Create beam
    R2SpanBeam beam(R2Span(source_point, source_point));

    // Update source face
    source_face = wing.FindFace(source_point, source_face);
    if (!source_face) return;

    // Update receiver face
    receiver_face = wing.FindFace(receiver_point, receiver_face);

    // Update mark - must be after FindFace
    RNwing_mark++;

    // Initialize receiver search ???
    found_receiver = FALSE;

    // Recursively draw beams
    TraceBeams(wing, source_point, source_face, receiver_point, receiver_face,
	      beam, source_face, NULL, 
	      0, 0, 0, 0);
}



void 
TraceBeams(R2Wing& wing, 
    const R2Point& source_point, R2WingFace *source_face,
    const R2Point& receiver_point, R2WingFace *receiver_face,
    const R2SpanBeam& beam, R2WingFace *face, R2WingEdge *edge,
    int speculars, int diffuses, int transmissions, int diffractions)
{
    // Trace beams through wing
    RNIterator iterator;
    R2WingEdge *neighbor_edge;
    R2_FOR_EACH_WING_FACE_EDGE(wing, face, neighbor_edge, iterator) {
        // Check if edge came from
        if ((edge) && (neighbor_edge == edge)) continue;

	// Get neighbor face
	R2WingFace *neighbor_face = wing.FaceAcrossEdge(neighbor_edge, face);

	// Get edge span
	R2Span neighbor_span(wing.EdgeSpan(neighbor_edge));

	// Check if edge intersects beam
	R2Span intersection_span;
	if (!beam.Intersects(neighbor_span, &intersection_span, -RN_SMALL_EPSILON)) 
	    continue;

        // Trace neighbor beams
        if (!wing.IsEdgeOpaque(neighbor_edge)) {
	    if (neighbor_face) {
		// Recurse along transmission beam
	        R2SpanBeam transmission_beam(beam);
		transmission_beam.Trim(neighbor_span);
		TraceBeams(wing, source_point, source_face, receiver_point, receiver_face,
			  transmission_beam, neighbor_face, neighbor_edge, 
			  speculars, diffuses, transmissions, diffractions);

		// Recurse along diffraction beams
		if (diffractions < max_diffractions) {
		    TraceDiffractionBeams(wing, source_point, source_face, receiver_point, receiver_face,
			  beam, face, edge, transmission_beam, neighbor_edge, RN_CW, 
			  speculars, diffuses, transmissions, diffractions);
		    TraceDiffractionBeams(wing, source_point, source_face, receiver_point, receiver_face,
			  beam, face, edge, transmission_beam, neighbor_edge, RN_CCW, 
			  speculars, diffuses, transmissions, diffractions);
		}
	    }
	}
        else {
	    // Recurse along transmission beam
	    if (neighbor_face && (transmissions < max_transmissions)) {
	        R2SpanBeam transmission_beam(beam);
		transmission_beam.Trim(neighbor_span);
		TraceBeams(wing, source_point, source_face, receiver_point, receiver_face,
			  transmission_beam, neighbor_face, neighbor_edge, 
			  speculars, diffuses, transmissions + 1, diffractions);
	    }

	    // Recurse along specular reflection beam
	    if (speculars < max_speculars) {
	        R2SpanBeam reflection_beam(beam);
		reflection_beam.TrimAndMirror(neighbor_span);
		TraceBeams(wing, source_point, source_face, receiver_point, receiver_face,
			  reflection_beam, face, neighbor_edge, 
			  speculars + 1, diffuses, transmissions, diffractions);
	    }

	    // Recurse along diffuse reflection beam
	    if (diffuses < max_diffuses) {
		// Trace beam from diffuse span
	        RNBoolean flip = (wing.FaceOnEdge(neighbor_edge) != face);
		R2Span diffuse_span((flip) ? -intersection_span : intersection_span);
	        R2SpanBeam reflection_beam(diffuse_span);
		TraceBeams(wing, source_point, source_face, receiver_point, receiver_face,
			  reflection_beam, face, neighbor_edge, 
			  speculars, diffuses + 1, transmissions, diffractions);

		// Draw diffuse span
	        glPushMatrix();
		glTranslatef(0.0, 0.0, 0.1);
		RNLoadRgb(0.5, 1.0, 1.0); 
		diffuse_span.Draw();
		glPopMatrix();
	    }
	}
    }

    // Check if beam contains receiver
    if ((receiver_face) && (face == receiver_face)) {
        found_receiver |= beam.Contains(receiver_point);
    }

    // Draw beams
    if (show_beams) {
        // Draw face
        if ((show_faces) && (wing.FaceMark(face) != RNwing_mark)) {
	    wing.SetFaceMark(face, RNwing_mark);
	    glPushMatrix();
	    glTranslatef(0.0, 0.0, -1.0);
	    RNLoadRgb(0.3, 0.3, 0.3);
	    wing.DrawFace(face);
	    glPopMatrix();
	}

	// Draw source images
	if (show_source_images && speculars) {
	    RNLoadRgb(0.5, 0.5, 0.0); 
	    beam.Outline(); 
	}

	// Draw beam
	RNBoolean show_beam = TRUE, direct_beam = TRUE;
	if (speculars) { show_beam &= show_speculars; direct_beam = FALSE; }
	if (diffuses) { show_beam &= show_diffuses; direct_beam = FALSE; }
	if (transmissions) { show_beam &= show_transmissions; direct_beam = FALSE; }
	if (diffractions) { show_beam &= show_diffractions; direct_beam = FALSE; }
	if (show_beam) {
	    if (direct_beam) {
	        // Load representative color 
	        RNLoadRgb(0.6, 0.6, 0.6);

		// Draw beam at appropriate depth
	        glPushMatrix();
		glTranslatef(0.0, 0.0, -0.1);
		beam.Draw(&wing, face);
		glPopMatrix();
	    }
	    else {
	        // Load representative color 
	        RNScalar r, g, b;
	        if (diffuses) { 
		    r = 0.8 - 0.15 * speculars - 0.025 * diffuses;
		    g = 0.8 - 0.15 * transmissions - 0.05 * diffuses;
		    b = 0.8 - 0.15 * diffractions - 0.075 * diffuses; 
		}
		else {
		    r = (speculars) ? 0.8 - 0.15 * speculars : 0.0;
		    g = (transmissions) ? 0.8 - 0.15 * transmissions : 0.0;
		    b = (diffractions) ? 0.8 - 0.15 * diffractions : 0.0;
		}
		if (r < 0.1) r = 0.1;
		if (g < 0.1) g = 0.1;
		if (b < 0.1) b = 0.1;
		RNLoadRgb(r, g, b);

		// Draw beam at appropriate depth
	        glPushMatrix();
		glTranslatef(0.0, 0.0, -0.1 - 0.001 * speculars - 0.005 * transmissions 
                             - 0.02 * diffuses - 0.1 * diffractions);
		beam.Draw(&wing, face);
		glPopMatrix();
	    }
	}
    }
}



void 
TraceDiffractionBeams(R2Wing& wing, 
    const R2Point& source_point, R2WingFace *source_face,
    const R2Point& receiver_point, R2WingFace *receiver_face,
    const R2SpanBeam& beam, R2WingFace *face, R2WingEdge *edge,
    const R2SpanBeam& transmission_beam, R2WingEdge *transmission_edge, RNDirection dir,
    int speculars, int diffuses, int transmissions, int diffractions)
{
    
    // Get vertex that will be virtual source
    R2WingVertex *diffraction_vertex = wing.VertexOnEdge(transmission_edge, face, dir);
    R2WingVertex *wedge_vertex[2] = { NULL, NULL };
    R2WingEdge *wedge_edge[2] = { NULL, NULL };
    
    // Check if vertex bounds incoming beam 
    if ((beam.IsTrimmed()) &&
	(R2SignedDistance(beam.Halfspace(1-dir).Line(), wing.VertexPosition(diffraction_vertex)) < -RN_BIG_EPSILON))
        return;

    // Search around vertex in dir for first opaque edge that admits diffraction
    for (R2WingEdge *e = wing.EdgeOnVertex(diffraction_vertex, transmission_edge, dir);
	 e != transmission_edge;
	 e = wing.EdgeOnVertex(diffraction_vertex, e, dir)) {
	 if (!wing.IsEdgeOpaque(e)) continue;
	 R2WingVertex *v = wing.VertexAcrossEdge(e, diffraction_vertex);
	 if (R2SignedDistance(transmission_beam.Halfspace(1-dir).Line(), wing.VertexPosition(v)) >= 0.0) return;
	 wedge_edge[dir] = e; 
	 wedge_vertex[dir] = v; 
	 break; 
    }

    // Check if didn't find opaque edge
    if (!wedge_edge[dir]) return;

    // Search around vertex in opposite dir for first opaque edge 
    for (e = wing.EdgeOnVertex(diffraction_vertex, transmission_edge, 1-dir);
	 e != transmission_edge;
	 e = wing.EdgeOnVertex(diffraction_vertex, e, 1-dir)) {
 	 if (!wing.IsEdgeOpaque(e)) continue;
	 R2WingVertex *v = wing.VertexAcrossEdge(e, diffraction_vertex);
  	 // assert(!R2Contains(transmission_beam.Halfspace(1-dir), wing.VertexPosition(v)));
	 wedge_edge[1-dir] = e; 
	 wedge_vertex[1-dir] = v; 
	 break; 
    }

    // Check if didn't find opaque edge
    assert(wedge_edge[1-dir]);

    // Construct diffraction beam
    R2SpanBeam diffraction_beam;
    R2Point diffraction_point = wing.VertexPosition(diffraction_vertex);
    if (dir == RN_CW) {
        R2Halfspace h0(-(transmission_beam.Halfspace(1)));
        R2Halfspace h1(wing.VertexPosition(wedge_vertex[dir]), wing.VertexPosition(diffraction_vertex));
	diffraction_beam = R2SpanBeam(R2Span(diffraction_point, diffraction_point), h0, h1);
    }
    else {
        R2Halfspace h0(wing.VertexPosition(diffraction_vertex), wing.VertexPosition(wedge_vertex[dir]));
        R2Halfspace h1(-(transmission_beam.Halfspace(0)));
	diffraction_beam = R2SpanBeam(R2Span(diffraction_point, diffraction_point), h0, h1);
    }
    
    // Recurse along diffraction beams
    for (e = transmission_edge;
	 e != wedge_edge[dir];
	 e = wing.EdgeOnVertex(diffraction_vertex, e, dir)) {
        R2WingFace *diffraction_face = wing.FaceOnEdge(e, diffraction_vertex, dir);
	TraceBeams(wing, source_point, source_face, receiver_point, receiver_face,
		  diffraction_beam, diffraction_face, e, 
		  speculars, diffuses, transmissions, diffractions + 1);
    }
}
    




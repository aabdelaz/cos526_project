// Source file for the scene viewer program



// Include files 

#include "R3Graphics/R3Graphics.h"
#include "fglut/fglut.h"
#include "Radiator.h"

// Program variables

static char *input_scene_name = NULL;
static char *output_image_name = NULL;
static char *screenshot_image_name = NULL;
static int render_image_width = 64;
static int render_image_height = 64;
static int print_verbose = 0;

// Grid
static double grid_point_radius = 0.00625;
static double* grid;
static int grid_nx = 10;
static int grid_ny = 10;
static double grid_dx;
static double grid_dy;
static double grid_x0;
static double grid_y0;
static double grid_scale = 1.0;

// GLUT variables 

static int GLUTwindow = 0;
static int GLUTwindow_height = 800;
static int GLUTwindow_width = 800;
static int GLUTmouse[2] = { 0, 0 };
static int GLUTbutton[3] = { 0, 0, 0 };
static int GLUTmouse_drag = 0;
static int GLUTmodifiers = 0;
// movement: 0 = camera, 1 for rad sources
static int movement = 0;
static int current_rad_source = 0;
static int num_rad_sources = 0;
static int moving = 0;
// 1 = left, 2 = right, 3 = up, 4 = down
static double camera_dx = 0.02;
static double camera_dy = 0.02;

// ray_tracing
static int max_layer = 15;

#if 0
// FPS
static int fpsmode = 0;
static R3Camera FPSToggleCam;
#endif



// Application variables

static R3Viewer *viewer = NULL;
static R3Scene *scene = NULL;
static R3Point center(0, 0, 0);



// Display variables

static int show_shapes = 1;
static int show_camera = 0;
static int show_lights = 0;
static int show_sources = 1;
static int show_bboxes = 0;
static int show_rays = 0;
static int show_frame_rate = 0;
static int show_grid = 1;

static void initGridValues(R3Scene *scene);

static void initGrid(R3Scene *scene)
{
  assert(scene);
  grid_x0 = scene->BBox().XMin();
  grid_y0 = scene->BBox().YMin();
  grid_dx = (scene->BBox().XMax() - grid_x0) / (grid_nx + 1);
  grid_dy = (scene->BBox().YMax() - grid_y0) / (grid_ny + 1);
  grid_x0 += grid_dx;
  grid_y0 += grid_dy;
  if (print_verbose)
  {
    printf("Grid: (%.3f:%.3f:%.3f) X (%.3f:%.3f:%.3f)\n", grid_x0, grid_dx, grid_x0 + (grid_nx - 1) * grid_dx,
      grid_y0, grid_dy, grid_y0 + (grid_ny - 1) * grid_dy);
  }
  grid = new double[grid_nx * grid_ny];
  initGridValues(scene);
}

/* TODO: inline these? */
static R3Point getGridPosition(int ix, int iy)
{
   return R3Point(grid_x0 + grid_dx * ix, grid_y0 + grid_dy * iy, 0.0);
}

static RNScalar getGridValue(int ix, int iy)
{
  return grid[ix * grid_ny + iy];
}

static void setGridValue(RNScalar value, int ix, int iy)
{
  grid[ix * grid_ny + iy] = value * grid_scale;
}

static void incGridValue(RNScalar inc, int ix, int iy)
{
  grid[ix * grid_ny + iy] += inc * grid_scale;
}

// sets 0.5 to the arithmetic mean
static void NormalizeGridScale(void)
{
  double sum = 0;
  for (int i = 0; i < grid_nx * grid_ny; i++)
    sum += grid[i];
  sum /= 2 * grid_nx * grid_ny;
  for (int i = 0; i < grid_nx * grid_ny; i++)
    grid[i] /= sum;
  grid_scale /= sum;
}

// returns distance from source to a point 
static RNScalar source2pointDistance(R3Point point, Radiator &source) {
    return (point - source.Position()).Length();
}

RNScalar opticalPath(R3Ray &ray, R3Scene *scene, R3Point source_point, RNBoolean in, int layer) {
    R3SceneNode *node = NULL;
    R3SceneElement *element = NULL;
    R3Shape *shape = NULL;
    R3Point point;
    R3Vector normal;
    RNScalar t;    
    RNScalar epsilon = 1e-15;
    RNScalar mu = 0;
    assert(scene);

    RNBoolean intersects = scene->Intersects(ray, &node, &element, 
                &shape, &point, &normal, &t); 
    // if in is true we assume that our ray intersected with the scene. subtract 1 from IoR
    if (element && element->Material())
      mu = in ? (element->Material()->Brdf()->IndexOfRefraction() - 1): 0;
    else if (print_verbose)
      printf("Lack of element or material!\n");
#if 0
    // debug
    if (print_verbose)
    {
      printf("In: %d, IoR: %f: from (%.3f,%.3f,%.3f) to (%.3f,%.3f,%.3f)\n", in, element->Material()->Brdf()->IndexOfRefraction(),
        ray.Start().X(), ray.Start().Y(), ray.Start().Z(), point.X(),point.Y(), point.Z());
      printf("t: %.3f; intersects: %d; ray.T: %.3f\n", t, intersects, ray.T(source_point));
    }
#endif
    // in this case we have that we have traced our way to the 
    // destination point, and we need not recurse 
    if (!intersects || t > ray.T(source_point) || layer >= max_layer) {
      if (print_verbose && (layer >= max_layer))
        printf("Hit max layer!\n");

       return (ray.Start() - source_point).Length()*mu;
    }  

    R3Ray newRay(point + epsilon*ray.Vector(), source_point);

    return (ray.Start() - point).Length()*mu +
        opticalPath(newRay, scene, source_point, !in, layer + 1);  
}

// returns optical path length
static RNScalar opticalPath(R3Point point, Radiator &source, R3Scene *scene) {
    R3Ray ray(point, source.Position());

    RNBoolean in = FALSE;

    return opticalPath(ray, scene, source.Position(), in, 0);

}

static RNScalar strength(R3Point point, Radiator &source, R3Scene *scene)
{
  RNScalar r = source2pointDistance(point, source);
  return exp(-opticalPath(point, source, scene)) / (r * r);
}


// adds radiator strength from source to the strength grid

static void SubtractStrength(Radiator &source, R3Scene *scene)
{
  for (int i = 0; i < grid_nx; i++)
    for (int j = 0; j < grid_ny; j++)
      incGridValue(-strength(getGridPosition(i,j), source, scene),i,j);
}

static void UpdateStrength(Radiator &source, R3Scene *scene)
{
  for (int i = 0; i < grid_nx; i++)
    for (int j = 0; j < grid_ny; j++)
      incGridValue(strength(getGridPosition(i,j), source, scene),i,j);
}

// initializes grid with source strengths

static void initGridValues(R3Scene *scene)
{
  for (int i = 0; i < grid_nx * grid_ny; i++)
    grid[i] = 0;
  for (int i = 0; i < scene->NRadSources(); i++)
  {
    Radiator &source = *(scene->RadSource(i));
    UpdateStrength(source, scene);
  }
  NormalizeGridScale();
}

// 
static void MoveRadiator(Radiator *source, R3Vector displacement, R3Scene *scene)
{
  SubtractStrength(*source, scene);
  source->Move(displacement);
  UpdateStrength(*source, scene);
}

////////////////////////////////////////////////////////////////////////
// Draw functions
////////////////////////////////////////////////////////////////////////

static void 
LoadLights(R3Scene *scene)
{
  // Load ambient light
  static GLfloat ambient[4];
  ambient[0] = scene->Ambient().R();
  ambient[1] = scene->Ambient().G();
  ambient[2] = scene->Ambient().B();
  ambient[3] = 1;
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

  // Load scene lights
  for (int i = 0; i < scene->NLights(); i++) {
    R3Light *light = scene->Light(i);
    light->Draw(i);
  }
}



#if 0

static void 
DrawText(const R3Point& p, const char *s)
{
  // Draw text string s and position p
  glRasterPos3d(p[0], p[1], p[2]);
  while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *(s++));
}
  
#endif



static void 
DrawText(const R2Point& p, const char *s)
{
  // Draw text string s and position p
  R3Ray ray = viewer->WorldRay((int) p[0], (int) p[1]);
  R3Point position = ray.Point(2 * viewer->Camera().Near());
  glRasterPos3d(position[0], position[1], position[2]);
  while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *(s++));
}



static void 
DrawCamera(R3Scene *scene)
{
  // Draw view frustum
  const R3Camera& camera = scene->Camera();
  R3Point eye = camera.Origin();
  R3Vector towards = camera.Towards();
  R3Vector up = camera.Up();
  R3Vector right = camera.Right();
  RNAngle xfov = camera.XFOV();
  RNAngle yfov = camera.YFOV();
  double radius = scene->BBox().DiagonalRadius();
  R3Point org = eye + towards * radius;
  R3Vector dx = right * radius * tan(xfov);
  R3Vector dy = up * radius * tan(yfov);
  R3Point ur = org + dx + dy;
  R3Point lr = org + dx - dy;
  R3Point ul = org - dx + dy;
  R3Point ll = org - dx - dy;
  glBegin(GL_LINE_LOOP);
  glVertex3d(ur[0], ur[1], ur[2]);
  glVertex3d(ul[0], ul[1], ul[2]);
  glVertex3d(ll[0], ll[1], ll[2]);
  glVertex3d(lr[0], lr[1], lr[2]);
  glVertex3d(ur[0], ur[1], ur[2]);
  glVertex3d(eye[0], eye[1], eye[2]);
  glVertex3d(lr[0], lr[1], lr[2]);
  glVertex3d(ll[0], ll[1], ll[2]);
  glVertex3d(eye[0], eye[1], eye[2]);
  glVertex3d(ul[0], ul[1], ul[2]);
  glEnd();
}



static void 
DrawLights(R3Scene *scene)
{
  // Draw all lights
  double radius = scene->BBox().DiagonalRadius();
  for (int i = 0; i < scene->NLights(); i++) {
    R3Light *light = scene->Light(i);
    RNLoadRgb(light->Color());
    if (light->ClassID() == R3DirectionalLight::CLASS_ID()) {
      R3DirectionalLight *directional_light = (R3DirectionalLight *) light;
      R3Vector direction = directional_light->Direction();

      // Draw direction vector
      glBegin(GL_LINES);
      R3Point centroid = scene->BBox().Centroid();
      R3LoadPoint(centroid - radius * direction);
      R3LoadPoint(centroid - 1.25 * radius * direction);
      glEnd();
    }
    else if (light->ClassID() == R3PointLight::CLASS_ID()) {
      // Draw sphere at point light position
      R3PointLight *point_light = (R3PointLight *) light;
      R3Point position = point_light->Position();

     // Draw sphere at light position 
       R3Sphere(position, 0.1 * radius).Draw();
    }
    else if (light->ClassID() == R3SpotLight::CLASS_ID()) {
      R3SpotLight *spot_light = (R3SpotLight *) light;
      R3Point position = spot_light->Position();
      R3Vector direction = spot_light->Direction();

      // Draw sphere at light position 
      R3Sphere(position, 0.1 * radius).Draw();
  
      // Draw direction vector
      glBegin(GL_LINES);
      R3LoadPoint(position);
      R3LoadPoint(position + 0.25 * radius * direction);
      glEnd();
    }
    else {
      fprintf(stderr, "Unrecognized light type: %d\n", light->ClassID());
      return;
    }
  }
}

static void 
DrawSources(R3Scene *scene)
{
  // Draw all lights
  double radius = scene->BBox().DiagonalRadius() * grid_point_radius * 3;
  for (int i = 0; i < scene->NRadSources(); i++) {
    Radiator *source = scene->RadSource(i);
    if ((movement) && (i == current_rad_source))
      RNLoadRgb(RNRgb(1.0,0.0,1.0));
    else
      RNLoadRgb(RNRgb(0.0,1.0,1.0));

    R3Point position = source->Position();

    // Draw sphere at source position 
    R3Sphere(position, radius).Draw();
  }
}



static void 
DrawShapes(R3Scene *scene, R3SceneNode *node, RNFlags draw_flags = R3_DEFAULT_DRAW_FLAGS)
{
  // Push transformation
  node->Transformation().Push();

  // Draw elements 
  for (int i = 0; i < node->NElements(); i++) {
    R3SceneElement *element = node->Element(i);
    element->Draw(draw_flags);
  }

  // Draw children
  for (int i = 0; i < node->NChildren(); i++) {
    R3SceneNode *child = node->Child(i);
    DrawShapes(scene, child, draw_flags);
  }

  // Pop transformation
  node->Transformation().Pop();
}



static void 
DrawBBoxes(R3Scene *scene, R3SceneNode *node)
{
  // Draw node bounding box
  node->BBox().Outline();

  // Push transformation
  node->Transformation().Push();

  // Draw children bboxes
  for (int i = 0; i < node->NChildren(); i++) {
    R3SceneNode *child = node->Child(i);
    DrawBBoxes(scene, child);
  }

  // Pop transformation
  node->Transformation().Pop();
}



static void 
DrawRays(R3Scene *scene)
{
  // Ray intersection variables
  R3SceneNode *node;
  R3SceneElement *element;
  R3Shape *shape;
  R3Point point;
  R3Vector normal;
  RNScalar t;

  // Ray generation variables
  int istep = scene->Viewport().Width() / 20;
  int jstep = scene->Viewport().Height() / 20;
  if (istep == 0) istep = 1;
  if (jstep == 0) jstep = 1;

  // Ray drawing variables
  double radius = 0.025 * scene->BBox().DiagonalRadius();

  // Draw intersection point and normal for some rays
  for (int i = istep/2; i < scene->Viewport().Width(); i += istep) {
    for (int j = jstep/2; j < scene->Viewport().Height(); j += jstep) {
      R3Ray ray = scene->Viewer().WorldRay(i, j);
      if (scene->Intersects(ray, &node, &element, &shape, &point, &normal, &t)) {
        R3Sphere(point, radius).Draw();
        R3Span(point, point + 2 * radius * normal).Draw();
      }
    }
  }
}


/* draws the sphere at grid point position with value value */
/* value must be scaled to be from 0 to 1 */
static void 
DrawSphere(R3Scene *scene, R3Point position, RNScalar value)
{
  double radius = grid_point_radius * scene->BBox().DiagonalRadius();
  if (value > 1.0)
    value = 1.0;
  if (value < 0.0)
    value = 0.0;
  /*glColor3d(0.0, value, 1.0 - value);

  GLfloat c[4];
  c[0] = 0.0;
  c[1] = value;
  c[2] = 1.0 - value;
  c[3] = 0.5;
  glMaterialfv(GL_FRONT, GL_AMBIENT, c);*/
  RNLoadRgb(RNRgb(0.0,value,1.0 - value));
  R3Sphere(position, radius).Draw();

}

/* draws the grid */
static void DrawGrid(R3Scene *scene)
{

  for (int ix = 0; ix < grid_nx; ix++)
    for (int iy = 0; iy < grid_ny; iy++)
      DrawSphere(scene, getGridPosition(ix, iy), getGridValue(ix, iy));
}



////////////////////////////////////////////////////////////////////////
// Glut user interface functions
////////////////////////////////////////////////////////////////////////

void GLUTStop(void)
{
  // Destroy window 
  glutDestroyWindow(GLUTwindow);

  // Exit
  exit(0);
}



void GLUTRedraw(void)
{
  // Check scene
  if (!scene) return;

  // Set viewing transformation
  viewer->Camera().Load();

  // Clear window 
  RNRgb background = scene->Background();
  glClearColor(background.R(), background.G(), background.B(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Load lights
  LoadLights(scene);

  // Draw camera
  if (show_camera) {
    glDisable(GL_LIGHTING);
    glColor3d(1.0, 1.0, 1.0);
    glLineWidth(5);
    DrawCamera(scene);
    glLineWidth(1);
  }

  // Draw lights
  if (show_lights) {
    glDisable(GL_LIGHTING);
    glColor3d(1.0, 1.0, 1.0);
    glLineWidth(5);
    DrawLights(scene);
    glLineWidth(1);
  }

  // Draw sources
  if (show_sources) {
    glDisable(GL_LIGHTING);
    glColor3d(1.0, 1.0, 1.0);
    glLineWidth(5);
    DrawSources(scene);
    glLineWidth(1);
  }

  // Draw rays
  if (show_rays) {
    glDisable(GL_LIGHTING);
    glColor3d(0.0, 1.0, 0.0);
    glLineWidth(3);
    DrawRays(scene);
    glLineWidth(1);
  }

  // Draw scene nodes
  if (show_shapes) {
    glEnable(GL_LIGHTING);
    R3null_material.Draw();
    DrawShapes(scene, scene->Root());
    R3null_material.Draw();
  }

  // Draw bboxes
  if (show_bboxes) {
    glDisable(GL_LIGHTING);
    glColor3d(1.0, 0.0, 0.0);
    DrawBBoxes(scene, scene->Root());
  }

  // Draw grid
  if (show_grid) {
    glDisable(GL_LIGHTING);
    glColor3d(1.0, 1.0, 1.0);
    DrawGrid(scene);
  }

  // Draw frame time
  if (show_frame_rate) {
    char buffer[128];
    static RNTime last_time;
    double frame_time = last_time.Elapsed();
    last_time.Read();
    if ((frame_time > 0) && (frame_time < 10)) {
      glDisable(GL_LIGHTING);
      glColor3d(1.0, 1.0, 1.0);
      sprintf(buffer, "%.1f fps", 1.0 / frame_time);
      DrawText(R2Point(100, 100), buffer);
    }
  }  

  // Capture screenshot image 
  if (screenshot_image_name) {
    if (print_verbose) printf("Creating image %s\n", screenshot_image_name);
    R2Image image(GLUTwindow_width, GLUTwindow_height, 3);
    image.Capture();
    image.Write(screenshot_image_name);
    screenshot_image_name = NULL;
  }

  // Swap buffers 
  glutSwapBuffers();
}    



void GLUTResize(int w, int h)
{
  // Resize window
  glViewport(0, 0, w, h);

  // Resize viewer viewport
  viewer->ResizeViewport(0, 0, w, h);

  // Resize scene viewport
  scene->SetViewport(viewer->Viewport());

  // Remember window size 
  GLUTwindow_width = w;
  GLUTwindow_height = h;

  // Redraw
  glutPostRedisplay();
}



void GLUTMotion(int x, int y)
{
  // Invert y coordinate
  y = GLUTwindow_height - y;

  // Compute mouse movement
  int dx = x - GLUTmouse[0];
  int dy = y - GLUTmouse[1];
  
  // Update mouse drag
  GLUTmouse_drag += dx*dx + dy*dy;

  // World in hand navigation 
  if (GLUTbutton[0]) viewer->RotateWorld(1.0, center, x, y, dx, dy);
  else if (GLUTbutton[1]) viewer->ScaleWorld(1.0, center, x, y, dx, dy);
  else if (GLUTbutton[2]) viewer->TranslateWorld(1.0, center, x, y, dx, dy);
  if (GLUTbutton[0] || GLUTbutton[1] || GLUTbutton[2]) glutPostRedisplay();

  // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = y;
}



void GLUTMouse(int button, int state, int x, int y)
{
  // Invert y coordinate
  y = GLUTwindow_height - y;

  // Mouse is going down
  if (state == GLUT_DOWN) {
    // Reset mouse drag
    GLUTmouse_drag = 0;
  }
  else {
    // Check for double click  
    static RNBoolean double_click = FALSE;
    static RNTime last_mouse_up_time;
    double_click = (!double_click) && (last_mouse_up_time.Elapsed() < 0.4);
    last_mouse_up_time.Read();

    // Check for click (rather than drag)
    if (GLUTmouse_drag < 100) {
      // Check for double click
      if (double_click) {
        // Set viewing center point 
        R3Ray ray = viewer->WorldRay(x, y);
        R3Point intersection_point;
        if (scene->Intersects(ray, NULL, NULL, NULL, &intersection_point)) {
          center = intersection_point;
        }
      }
    }
  }

  // Remember button state 
  int b = (button == GLUT_LEFT_BUTTON) ? 0 : ((button == GLUT_MIDDLE_BUTTON) ? 1 : 2);
  GLUTbutton[b] = (state == GLUT_DOWN) ? 1 : 0;

  // Remember modifiers 
  GLUTmodifiers = glutGetModifiers();

   // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = y;

  // Redraw
  glutPostRedisplay();
}



void GLUTSpecial(int key, int x, int y)
{
  // Invert y coordinate
  y = GLUTwindow_height - y;

  // Process keyboard button event 

  // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = y;

  // Remember modifiers 
  GLUTmodifiers = glutGetModifiers();

  // Redraw
  glutPostRedisplay();
}



void GLUTKeyboard(unsigned char key, int x, int y)
{
  // Process keyboard button event 
  switch (key) {
  case '~': {
    // Dump screen shot to file iX.jpg
    static char buffer[64];
    static int image_count = 1;
    sprintf(buffer, "i%d.jpg", image_count++);
    screenshot_image_name = buffer;
    break; }

  case 'B':
  case 'b':
    show_bboxes = !show_bboxes;
    break;

  case 'C':
  case 'c':
#if 0
    show_camera = !show_camera;
#endif
    movement = 0;
    break;

  case 'E':
  case 'e':
    if (num_rad_sources)
    {
      if (movement) {
        current_rad_source = (current_rad_source + 1 ) % num_rad_sources;
      }
      else {
        movement = 1;
      }
    }
    break;

  case 'G':
  case 'g':
    show_grid = !show_grid;
    break;

  case 'L':
  case 'l':
    show_lights = !show_lights;
    break;

  case 'O':
  case 'o':
    show_sources = !show_sources;
    break;

  case 'R':
  case 'r':
    show_rays = !show_rays;
    break;

#if 0
  case 'S':
  case 's':
    show_shapes = !show_shapes;
    break;
#endif

  case 'T':
  case 't':
    show_frame_rate = !show_frame_rate;
    break;

  case ' ':
    viewer->SetCamera(scene->Camera());
    break;

  case 'A':
  case 'a': {
    R3Vector moveVector = camera_dx * scene->BBox().DiagonalRadius() * viewer->Camera().Left();
    if (movement == 0) {
      viewer->TranslateCamera(moveVector);
    }
    else {
      moveVector.SetZ(0.0);
      MoveRadiator(scene->RadSource(current_rad_source), moveVector, scene);
    }

    break; }

  case 'D':
  case 'd': {
    R3Vector moveVector = camera_dx * scene->BBox().DiagonalRadius() * viewer->Camera().Right();
    if (movement == 0) {
      viewer->TranslateCamera(moveVector);
    }
    else {
      moveVector.SetZ(0.0);
      MoveRadiator(scene->RadSource(current_rad_source), moveVector, scene);
    }

    break; }

  case 'W':
  case 'w': {
    R3Vector moveVector = camera_dx * scene->BBox().DiagonalRadius() * viewer->Camera().Up();
    if (movement == 0) {
      viewer->TranslateCamera(moveVector);
    }
    else {
      moveVector.SetZ(0.0);
      MoveRadiator(scene->RadSource(current_rad_source), moveVector, scene);
    }

    break; }

  case 'S':
  case 's':{
    R3Vector moveVector = camera_dx * scene->BBox().DiagonalRadius() * viewer->Camera().Down();
    if (movement == 0) {
      viewer->TranslateCamera(moveVector);
    }
    else {
      moveVector.SetZ(0.0);
      MoveRadiator(scene->RadSource(current_rad_source), moveVector, scene);
    }
    break; }

  case 27: // ESCAPE
    GLUTStop();
    break;
  }

  // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = GLUTwindow_height - y;

  // Remember modifiers 
  GLUTmodifiers = glutGetModifiers();

  // Redraw
  glutPostRedisplay();  
}




void GLUTInit(int *argc, char **argv)
{
  // Open window 
  glutInit(argc, argv);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(GLUTwindow_width, GLUTwindow_height);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // | GLUT_STENCIL
  GLUTwindow = glutCreateWindow("Property Viewer");

  // Initialize lighting
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  static GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING); 

  // Initialize headlight
  // static GLfloat light0_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
  // static GLfloat light0_position[] = { 0.0, 0.0, 1.0, 0.0 };
  // glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  // glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  // glEnable(GL_LIGHT0);

  // Initialize graphics modes  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Initialize GLUT callback functions 
  glutDisplayFunc(GLUTRedraw);
  glutReshapeFunc(GLUTResize);
  glutKeyboardFunc(GLUTKeyboard);
  glutSpecialFunc(GLUTSpecial);
  glutMouseFunc(GLUTMouse);
  glutMotionFunc(GLUTMotion);
}



void GLUTMainLoop(void)
{
  // Initialize viewing center
  if (scene) center = scene->BBox().Centroid();

  // Run main loop -- never returns 
  glutMainLoop();
}


 
////////////////////////////////////////////////////////////////////////
// Input/output
////////////////////////////////////////////////////////////////////////

static R3Scene *
ReadScene(char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate scene
  R3Scene *scene = new R3Scene();
  if (!scene) {
    fprintf(stderr, "Unable to allocate scene for %s\n", filename);
    return NULL;
  }

  // Read scene from file
  if (!scene->ReadFile(filename)) {
    delete scene;
    return NULL;
  }

  // Print statistics
  if (print_verbose) {
    printf("Read scene from %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    fflush(stdout);
  }

  // Return scene
  return scene;
}



static int
WriteImage(R2Image *image, const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Write image to file
  if (!image->Write(filename)) return 0;

  // Print statistics
  if (print_verbose) {
    printf("Wrote image to %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  Width = %d\n", image->Width());
    printf("  Height = %d\n", image->Height());
    fflush(stdout);
  }

  // Return success
  return 1;
}  



////////////////////////////////////////////////////////////////////////
// Program argument parsing
////////////////////////////////////////////////////////////////////////

static int 
ParseArgs(int argc, char **argv)
{
  // Parse arguments
  argc--; argv++;
  while (argc > 0) {
    if ((*argv)[0] == '-') {
      if (!strcmp(*argv, "-v")) {
        print_verbose = 1; 
      }
      else if (!strcmp(*argv, "-resolution")) { 
        argc--; argv++; render_image_width = atoi(*argv); 
        argc--; argv++; render_image_height = atoi(*argv); 
      }
      else if (!strcmp(*argv, "-gr")) { 
        argc--; argv++; grid_point_radius = atof(*argv); 
      }
      else if (!strcmp(*argv, "-gdim")) { 
        argc--; argv++; grid_nx = atoi(*argv); 
        argc--; argv++; grid_ny = atoi(*argv); 
      }
      else { 
        fprintf(stderr, "Invalid program argument: %s", *argv); 
        exit(1); 
      }
      argv++; argc--;
    }
    else {
      if (!input_scene_name) input_scene_name = *argv;
      else if (!output_image_name) output_image_name = *argv;
      else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
      argv++; argc--;
    }
  }

  // Check scene filename
  if (!input_scene_name) {
    fprintf(stderr, "Usage: photonmap inputscenefile [outputimagefile] [-resolution <int> <int>] [-v]\n");
    return 0;
  }

  // Return OK status 
  return 1;
}



////////////////////////////////////////////////////////////////////////
// Main program
////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  // Parse program arguments
  if (!ParseArgs(argc, argv)) exit(-1);

  // Read scene
  scene = ReadScene(input_scene_name);
  if (!scene) exit(-1);


  else {
    initGrid(scene);
    num_rad_sources = scene->NRadSources();

    if (!num_rad_sources)
      movement = 0;

    // Initialize GLUT
    GLUTInit(&argc, argv);

    // Create viewer
    viewer = new R3Viewer(scene->Viewer());
    if (!viewer) exit(-1);
    
    // Run GLUT interface
    GLUTMainLoop();

    // Delete viewer (doesn't ever get here)
    delete viewer;
  }

  // Return success 
  return 0;
}


















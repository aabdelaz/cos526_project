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
static double* optical_paths;
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

static RNScalar getOptPathValue(int ix, int iy)
{
  return optical_paths[ix * grid_ny + iy];
}

static void setGridValue(RNScalar value, int ix, int iy)
{
  grid[ix * grid_ny + iy] = value * grid_scale;
}

static void incGridValue(RNScalar inc, int ix, int iy)
{
  grid[ix * grid_ny + iy] += inc * grid_scale;
}

static void incOptPathValue(RNScalar inc, int ix, int iy)
{
  optical_paths[ix * grid_ny + iy] += inc;
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

}

// returns optical path length to point
static RNScalar opticalPath(R3Point point, Radiator &source, R3Scene *scene)
{
  
}

static RNScalar strength(int ix, int iy, Radiator &source)
{
  RNScalar r = source2pointDistance(getGridPosition(ix, iy), source);
  return exp(-getOptPathValue(ix, iy)) / (r * r);
}

/* Helpful function */
static void GetBoundingVertices(R3Box &wall, R3Affine &transformation, R3Point *v1, R3Point *v2, R3Point *v3, R3Point *v4)
{
  assert(v1);
  assert(v2);
  assert(v3);
  assert(v4);
  RNScalar x1 = wall.XMin();
  RNScalar x2 = wall.XMax();
  RNScalar y1 = wall.YMin();
  RNScalar y2 = wall.YMax();
  R3Point vert1 = R3Point(x1,y1,0);
  R3Point vert2 = R3Point(x2,y1,0);
  R3Point vert3 = R3Point(x2,y2,0);
  R3Point vert4 = R3Point(x1,y2,0);
  transformation.Apply(vert1);
  transformation.Apply(vert2);
  transformation.Apply(vert3);
  transformation.Apply(vert4);
  *v1 = vert1;
  *v2 = vert2;
  *v3 = vert3;
  *v4 = vert4;
}

/* Ameera */
// input boundaries of wall in vert1 vert2 vert3 vert4 (counter-clockwise order);
//   output vertices into v1 and v2
static void GetBoundingVectors(R3Point vert1, R3Point vert2, R3Point vert3, R3Point vert4,
  R3Point source_point, R3Vector *v1, R3Vector *v2)
{


}

/* Ameera */
static RNBoolean InBounds(const R3Vector &v1, const R3Vector &v2, 
  const R3Point &source_point, const R3Point &grid_point)
{

}

// returns a distance from source to edge. Negative if edge behind source. Returns negative-1 if beyond edge
static RNScalar SourceDistToEdge(const R3Point &endpt1, const R3Point &endpt2, 
  const R3Point &source, const R3Point &dest)
{
  RNScalar t1, t2;
  RNScalar a1, a2, b1, b2, c1, c2, d1, d2;
  // set a + t1 ( b-a) = c + t2 (d-c) and solve for t1 and t2
  a1 = endpt1.X();
  a2 = endpt1.Y();
  b1 = endpt2.X();
  b2 = endpt2.Y();
  c1 = source.X();
  c2 = source.Y();
  d1 = dest.X();
  d2 = dest.Y();
  RNScalar denom = a2 * d1 - a1 * d2 + a1 * c2 - a2 * c1 + b2 * c1 - b1 * c2 + b1 * d2 - b2 * d1;
  if (RNIsZero(denom))
    return -1;
  t2 = a1 * c2 - a2 * c1 + c1 * b2 - c2 * b1 + a2 * b1 - a1 * b2;
  t2 /= denom;
  t1 = a1 * c2 - a2 * c1 + a2 * d1 - a1 * d2 + c1 * d2 - c2 * d1;
  t1 /= denom;
  if (RNIsNegative(t1) || RNIsPositive(t1 - 1.0))
    return -1;
  return t2 * (dest - source).Length();
}

// Given a grid point and vertices and source, update the optical path length
static void UpdatePathLength(int ix, int iy, const R3Point &v1, const R3Point &v2, const R3Point &v3, 
  const R3Point &v4, const R3Point &source_point, RNScalar mu)
{
  RNScalar dist1, dist2, dist3, dist4;
  dist1 = SourceDistToEdge(v1, v2, getGridPosition(ix, iy), source_point);
  dist2 = SourceDistToEdge(v2, v3, getGridPosition(ix, iy), source_point);
  dist3 = SourceDistToEdge(v3, v4, getGridPosition(ix, iy), source_point);
  dist4 = SourceDistToEdge(v4, v1, getGridPosition(ix, iy), source_point);
  // either two negative, three negative, or four negative. If one negative, then really unlucky...
  RNScalar sign = dist1 * dist2 * dist3 * dist4;
  if (RNIsPositive(sign))
  {
    // four or two negative. must be outside wall or really unlucky and hit a corner.
    // find the two positive distances
    RNScalar d1, d2;
    if (RNIsPositive(dist1))
    {
      d1 = dist1;
      if (RNIsPositive(dist2))
        d2 = dist2;
      else if (RNIsPositive(dist3))
        d2 = dist3;
      else
        d2 = dist4;
    }
    else if (RNIsPositive(dist2))
    {
      d1 = dist2;
      if (RNIsPositive(dist3))
        d2 = dist3;
      else
        d2 = dist4;
    }
    else if (RNIsPositive(dist3) || RNIsPositive(dist4))
    {
      d1 = dist3;
      d2 = dist4;
    }
    else 
    {
      // do nothing if none positive
      return;
    }
    // sort the two positive distances
    if (d1 < d2)
    {
      RNScalar tmp = d1;
      d1 = d2;
      d2 = tmp;
    }
    // increment path
    if (RNIsPositive(d1 - d2))
      incOptPathValue((d1-d2) * mu, ix, iy);
    else
    {
      incOptPathValue(((source_point - getGridPosition(ix, iy)).Length() - d1) * mu, ix, iy);
      if (print_verbose)
        printf("Really unlucky! Hit a corner.\n");
    }

  }
  else
  {
    // one or three negative. Most likely three negative, but if one negative then must deal with it
    RNScalar totlength = (source_point - getGridPosition(ix, iy)).Length();
    RNScalar d1, d2;
    d2 = -1;
    if (RNIsPositive(dist1))
    {
      d1 = dist1;
      if (RNIsPositive(dist2) && RNIsNotEqual(dist1,dist2))
        d2 = dist2;
      else if (RNIsPositive(dist3) && RNIsNotEqual(dist1,dist3))
        d2 = dist3;
      else if (RNIsPositive(dist4))
        d2 = dist4;
    }
    else if (RNIsPositive(dist2))
    {
      d1 = dist2;
      if (RNIsPositive(dist3) && RNIsNotEqual(dist2,dist3))
        d2 = dist3;
      else if (RNIsPositive(dist4))
        d2 = dist4;
    }
    else if (RNIsPositive(dist3))
    {
      d1 = dist3;
    }
    else
    {
      d1 = dist4;
    }
    if (RNIsPositive(d2))
    {
      if (print_verbose)
        printf("Really unlucky! Hit a corner and another edge!\n");
      if (d1 < d2)
      {
        RNScalar tmp = d1;
        d1 = d2;
        d2 = tmp;
      }
      incOptPathValue((d1-d2) * mu, ix, iy);
    }
    else
    {
      incOptPathValue((totlength-d1) * mu, ix, iy);
    }
  }

}
// adds radiator strength from source to the strength grid

static void CalculatePathsWall(R3Box &wall, R3Affine &transformation, Radiator &source, RNScalar mu)
{
  R3Point v1,v2,v3,v4;

  R3Vector bv1, bv2;
  R3Point source_pt = source.Position();
  // get bounding vertices
  GetBoundingVertices(wall, transformation, &v1, &v2, &v3, &v4);
  if (print_verbose)
  {
    printf("Mu: %.3f; Bounding vertices:\n", mu);
    printf("(%.3f,%.3f,%.3f)\n(%.3f,%.3f,%.3f)\n", v1.X(),v1.Y(),v1.Z(),
      v2.X(),v2.Y(),v2.Z());
    printf("(%.3f,%.3f,%.3f)\n(%.3f,%.3f,%.3f)\n", v3.X(),v3.Y(),v3.Z(),
      v4.X(),v4.Y(),v4.Z());
  }

  GetBoundingVectors(v1, v2, v3, v4, source_pt, &v1, &v2);

  for (int i = 0; i < grid_nx; i++)
    for (int j = 0; j < grid_ny; j++)
      if (InBounds(bv1, bv2, source_pt, getGridPosition()))
        UpdatePathLength(i, j, v1, v2, v3, v4, source_pt, mu);

}

// traversing scene tree to grab the walls
static void traverse_tree(R3SceneNode *node, R3Affine trans, Radiator& source, void (*callback)(R3Box &wall, R3Affine &transformation, Radiator &source, RNScalar mu))
{
  trans.Transform(node->Transformation());
  for (int i = 0; i < node->NElements(); i++)
    for (int j = 0; j < node->Element(i)->NShapes(); j++)
      if (node->Element(i)->Shape(j)->ClassID() == R3Box::CLASS_ID())
      {
        if (print_verbose)
        {
          printf("Hit box! Transformation: \n");
          printf("%.3f %.3f %.3f %.3f\n%.3f %.3f %.3f %.3f\n", trans.Matrix()[0][0],
            trans.Matrix()[0][1],trans.Matrix()[0][2],trans.Matrix()[0][3],trans.Matrix()[1][0],
            trans.Matrix()[1][1],trans.Matrix()[1][2],trans.Matrix()[1][3]);
          printf("%.3f %.3f %.3f %.3f\n%.3f %.3f %.3f %.3f\n", trans.Matrix()[2][0],
            trans.Matrix()[2][1],trans.Matrix()[2][2],trans.Matrix()[2][3],trans.Matrix()[3][0],
            trans.Matrix()[3][1],trans.Matrix()[3][2],trans.Matrix()[3][3]);
        }
        callback(*((R3Box*) node->Element(i)->Shape(j)),trans, source,
          node->Element(i)->Material()->Brdf()->IndexOfRefraction());
      }
  for (int i = 0; i < node->NChildren(); i++)
    traverse_tree(node->Child(i), trans, source, callback);
}

static void traverse_tree(R3SceneNode *root, Radiator& source, void (*callback)(R3Box &wall, R3Affine &transformation, Radiator &source, RNScalar mu))
{
   R3Affine trans = R3identity_affine;
   traverse_tree(root, trans, source, callback);
}


static void CalculatePaths(Radiator &source, R3Scene *scene)
{
  traverse_tree(scene->Root(), source, CalculatePathsWall);
}

static void SubtractStrength(Radiator &source, R3Scene *scene)
{
  optical_paths = new double[grid_nx * grid_ny];
  for (int i = 0; i < grid_nx * grid_ny; i++)
    optical_paths[i] = 0;
  CalculatePaths(source, scene);
  for (int i = 0; i < grid_nx; i++)
    for (int j = 0; j < grid_ny; j++)
      incGridValue(-strength(i,j,source),i,j);
  delete [] optical_paths;
}

static void UpdateStrength(Radiator &source, R3Scene *scene)
{
  optical_paths = new double[grid_nx * grid_ny];
  for (int i = 0; i < grid_nx * grid_ny; i++)
    optical_paths[i] = 0;
  CalculatePaths(source, scene);
  for (int i = 0; i < grid_nx; i++)
    for (int j = 0; j < grid_ny; j++)
      incGridValue(strength(i,j,source),i,j);
  delete [] optical_paths;
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


















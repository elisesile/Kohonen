/*
#########################
Installation des packages
sudo apt install libglu1-mesa-dev freeglut3-dev mesa-common-dev
#########################
Simple programme d'affichage de points et de segments en opengl
utilise GL et glut
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "base_opengl.h"
#include "ppm.h"

#define DEFAULT_WIDTH  500
#define DEFAULT_HEIGHT 500
#define NB_VILLE 21
#define MODE 1 // Exo 1 = 0 // exo 2 = 1
#define COMPRESSION 1
#define SORTDATA 0
#define EPSILON 0.1

#define COLORS 256

#if COMPRESSION
#define DATA_SIZE 307200
#define SIZE 307200
#define MAX_DATA 3
#else if MODE
#define DATA_SIZE 20
#define SIZE 22
#define MAX_DATA 2
#else
#define DATA_SIZE 20
#define SIZE 20
#define MAX_DATA 2
#endif
#define DEBUG 0

int cpt = 0;
int calc = 0;
char presse;
Point ville[NB_VILLE];
int anglex=0;
int angley=0;
int x, y, xold, yold;
GLuint textureID;

int width  = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;

typedef struct {
  float data[MAX_DATA];
}Dataset;

typedef struct {
  float activite;
  float weight[MAX_DATA];
  float potentiel;
}Neuron;

Dataset dataset[DATA_SIZE];
Neuron neuronset[SIZE];

int static ind[20] = {0};

float randomFloat(int max){
  return (float)(rand() / (double)RAND_MAX * (max - 1));
}

int randomInt(int max){
  return (int)(rand() / (double)RAND_MAX * (max - 1));
}

void printDataset(){
  for (int i = 0; i < DATA_SIZE; i++) {
    printf("Data %d \n", i);
    for (int j = 0; j < MAX_DATA; j++) {
      printf(" \t \t %d --- %f \t \n",j, dataset[i].data[j] );
    }
  }
}

void printNeuronset(){
  for (int i = 0; i < SIZE; i++) {
    printf("Neuron %d \n", i);
    for (int j = 0; j < MAX_DATA; j++) {
      printf(" \t \t %d --- %f \t \n",j, neuronset[i].weight[j] );
    }
  }
}

void InitializeSetData(int max){
  #if DEBUG
  printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
  #endif
  for(int i=0; i<DATA_SIZE; i++){
    dataset[i].data[0] = randomFloat(max);
    dataset[i].data[1] = randomFloat(max);
  }
  #if DEBUG
  printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
  #endif
}

void Initializeneuronset(int max){
  #if DEBUG
  printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
  #endif
  for(int i=0; i<SIZE; i++){
    for (int j = 0; j < MAX_DATA; j++) {
      neuronset[i].weight[j] = randomFloat(max);
    }
  }
  #if DEBUG
  printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
  #endif
}

Dataset sortData(){
  #if DEBUG
  printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
  #endif
  int val = randomInt(20);
  int flag = 1;
  for(int i=0; i<DATA_SIZE; i++){
    if(ind[i]!=1) flag = 0;
  }
  if(flag){
    for(int i=0; i<DATA_SIZE; i++) ind[i] = 0;
  }
  if(ind[val] != 1) val = randomInt(20);
  ind[val] = 1;
  #if DEBUG
  printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
  #endif
  return dataset[val];
}

void potentiel(Dataset data){
  #if DEBUG
  printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
  #endif
  for(int i=0; i<SIZE;i++){
    float sum = 0;
    for(int j=0; j<MAX_DATA; j++)
    sum += pow(neuronset[i].weight[j] -  data.data[j],2);
    neuronset[i].potentiel = sqrt(sum);
  }
  #if DEBUG
  printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
  #endif
}

void activity(){
  for(int i=0; i<SIZE; i++)
  neuronset[i].activite = (float) (1.0 / (1.0 + neuronset[i].potentiel));
}

int indiceNeuron(){
  int gagnant = -1;
  float max = (float) -1.0;
  for(int i=0; i<SIZE; i++){
    if(neuronset[i].activite > max) {
      gagnant = i;
      max = neuronset[i].activite;
    }
  }
  return gagnant;
}

float Phi(int index, int gagnant) {
  int distance = abs(gagnant - index);
  if(distance == 0) return 1;
  else if(distance == 1) return 0.5;
  else if(distance == 2) return -0.05;
  else return 0;
}

void updateNeuron(Dataset dataset, int winner){
  // // printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
  for(int i=0; i<SIZE; i++){
    for(int j=0; j<MAX_DATA; j++){
      neuronset[i].weight[j] =  neuronset[i].weight[j] + (EPSILON * (dataset.data[j] - neuronset[i].weight[j]) * Phi(winner, i));
    }
  }
}

/* affiche la chaine fmt a partir des coordonn�es x,y*/
void draw_text(float x, float y, const char *fmt, ...)
{
  char    buf[1024];      //Holds Our String
  char    *text = buf;
  va_list   ap;       // Pointer To List Of Arguments

  if (fmt == NULL)        // If There's No Text
  return;         // Do Nothing

  va_start(ap, fmt);        // Parses The String For Variables
  vsprintf(text, fmt, ap);      // And Converts Symbols To Actual Numbers
  va_end(ap);         // Results Are Stored In Text

  glDisable(GL_TEXTURE_2D);
  glRasterPos2i( x, y );
  while (*text)
  glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);

  glEnable(GL_TEXTURE_2D);
}

GLuint charger_texture(unsigned char * data)
{
  GLuint textureBidule;
  glGenTextures(1, &textureBidule); /* Texture name generation */
  glBindTexture(GL_TEXTURE_2D, textureBidule); /* Binding of texture name */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);;
  return textureBidule;
}

unsigned char * transform_img_to_vector(const char * filename, int *width , int *height){
  Image* image = NULL;
  image = readPPM(filename);
  if (image == NULL) EXIT_ON_ERROR("error loading img");
  int i, j;
  unsigned char* data = NULL;
  *width = image->x;
  *height = image->y;
  data = (unsigned char*)malloc(3 * image->x * image->y * sizeof(unsigned char));

  for (i = 0; i < image->x * image->y; i++){
    j = i * 3;
    data[j] = image->data[i].r;
    data[j + 1] = image->data[i].g;
    data[j + 2] = image->data[i].b;
  }
  if (image != NULL){
    free(image->data);
    image->data = NULL;
    free(image);
    image = NULL ;
  }
  return data;
}

void image_to_dataset(const char * filename, int *width , int *height){
  Image* image = NULL;
  image = readPPM(filename);
  if (image == NULL) EXIT_ON_ERROR("error loading img");
  int i;
  *width = image->x;
  *height = image->y;
  printf("Neuronset image size : %d\n",   image->x * image->y);
  printf("Dataset image size : %d\n",   image->x * image->y);
  for (i = 0; i < image->x * image->y; i+=3){
    dataset[i].data[0] = image->data[i].r;
    dataset[i].data[1] = image->data[i].g;
    dataset[i].data[2] = image->data[i].b;
  }
  if (image != NULL){
    free(image->data);
    image->data = NULL;
    free(image);
    image = NULL ;
  }
}

void load_cities(){
  int i, j = 0;
  FILE * file = NULL;
  char buffer[50];
  char *store[3];
  char * parsed_buffer;
  file = fopen("Villes_et_positions_dans_image.txt", "rb");
  if (file == NULL) EXIT_ON_ERROR("error while loading cities txt file");
  while (fgets(buffer, sizeof(buffer), file) != NULL){
    buffer[strlen(buffer) - 1] = '\0';
    j = 0;
    parsed_buffer = strtok(buffer, " ");
    while (parsed_buffer != NULL){
      store[j++] = parsed_buffer;
      parsed_buffer = strtok (NULL, " ");
    }
    strcpy(ville[i].name, store[0]);
    ville[i].x = atoi(store[1])-5; // shift du au resize de l'image
    ville[i].y = atoi(store[2])-5;
    i++;
  }
  fclose(file);
}

// EXO2 : transforme les points de la ville en données dataset
void city_to_dataset() {
  for(int i=0; i<NB_VILLE; i++) {
    dataset[i].data[0] = ville[i].x;
    dataset[i].data[1] = ville[i].y;
  }
}

void writeImage(){
  Image* image = NULL;
  image = (Image *) malloc(sizeof(Image));
  image->x = height;
  image->y = width;
  int i;
  image->data = (Pixel*)malloc(image->x * image->y * sizeof(Pixel));
  printf("Begin creating image\n");
  for (i = 0; i < image->x * image->y; i+=3){
    image->data[i].r = neuronset[i].weight[0];
    image->data[i].g = neuronset[i].weight[1] ;
    image->data[i].b = neuronset[i].weight[2];
  }
  writePPM("Rhouse_64.ppm", image);
  printf("Image created\n");
  if (image != NULL)
  {
    free(image->data);
    image->data = NULL;
    free(image);
    image = NULL ;
  }
}
/* Initialize OpenGL Graphics */
void initGL(int w, int h)
{

  #if MODE
  int taille_point = 15;
  #else
  int taille_point = 5;
  #endif

  glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT
  glEnable(GL_TEXTURE_2D);     // Enable 2D texturing

  glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
  glLoadIdentity();

  #if MODE
  glOrtho(0.0, w, h, 0.0, -1, 1);
  #else
  glOrtho(0.0, 200, 200, 0.0, -1, 1);
  #endif

  glPointSize(taille_point);
  glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
}
/* *************************************************** */
/* main */
int main(int argc, char **argv){
  srand (time (NULL));

  #if MODE
  InitializeSetData(740);
  Initializeneuronset(740);
  unsigned char * data = NULL;
  if (argc != 2) EXIT_ON_ERROR("You must specified a .ppm file");
  data = transform_img_to_vector(argv[1], &width, &height);
  if (COMPRESSION){
    Initializeneuronset(COLORS);
    image_to_dataset(argv[1], &width, &height);
  }
  else{
    load_cities();
    city_to_dataset();
    printDataset();
  }
  #else
  InitializeSetData(200);
  Initializeneuronset(200);
  #endif

  /* GLUT init */
  glutInit(&argc, argv);            // Initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
  glutInitWindowSize(width, height);   // Set the window's initial width & height
  glutCreateWindow("Kohonen");      // Create window with the name of the execudatasetle

  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);

  /* OpenGL 2D generic init */
  initGL(width, height);

  #if MODE
  textureID = charger_texture(data);
  if (data != NULL)
  {
    free(data);
    data = NULL;
  }
  #endif

  /* Main loop */
  glutMainLoop();

  #if MODE
  /* Delete used resources and quit */
  glDeleteTextures(1, &textureID);
  #endif

  return 0;
}

/* *************************************************** */

/* fonction d'affichage appel�e a chaque refresh*/
void affichage(){
  // Clear color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix
  glLoadIdentity();

  #if MODE
  /* Draw a quad */
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0); glVertex2i(0,   0);
  glTexCoord2i(0, 1); glVertex2i(0,   height);
  glTexCoord2i(1, 1); glVertex2i(width, height);
  glTexCoord2i(1, 0); glVertex2i(width, 0);
  glEnd();
  if(!COMPRESSION){
    for(int i=0; i<NB_VILLE; i++){
      glBegin(GL_POINTS);
      glColor3f(0.0, 1.0, 1.0);
      glVertex2f(ville[i].x, ville[i].y);
      glEnd();
      glColor3f(0, 0, 0);
      draw_text(ville[i].x - 20, ville[i].y + 20, "%s", ville[i].name);
    }

    for(int i=0; i<SIZE; i++) {
      glBegin(GL_POINTS);
      glColor3f(1.0, 0.0, 0.0);
      glVertex2f(neuronset[i].weight[0], neuronset[i].weight[1]);
      glEnd();
      glColor3f(1.0, 0.0, 0.0);
      draw_text(neuronset[i].weight[0] + 5, neuronset[i].weight[1], "%d", i);
      if(i > 0) {
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(neuronset[i - 1].weight[0], neuronset[i - 1].weight[1]);
        glVertex2f(neuronset[i].weight[0], neuronset[i].weight[1]);
        glEnd();
      }
    }
    draw_text(60, 70, "nb iter: %d", cpt);
    glColor3f(1.0, 1.0, 1.0);
  }

  #else

  // VOTRE CODE D'AFFICHAGE ICI, voir l'exemple ci-dessous
  for(int i = 0;i < DATA_SIZE; i++){
    glBegin(GL_POINTS);
    glColor3f(0.0,1.0,1.0);
    glVertex2f(dataset[i].data[0],dataset[i].data[1]);
    glColor3f(0.0, 1.0, 1.0);
    draw_text(dataset[i].data[0] + 2,dataset[i].data[1], "%d", i);
    glEnd();
  }

  for(int i=0; i<SIZE; i++) {
    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(neuronset[i].weight[0], neuronset[i].weight[1]);
    glEnd();
    glColor3f(1.0, 0.0, 0.0);
    draw_text(neuronset[i].weight[0] + 2, neuronset[i].weight[1], "%d", i);
    if(i > 0) {
      glBegin(GL_LINE_LOOP);
      glColor3f(1.0, 0.0, 0.0);
      glVertex2f(neuronset[i - 1].weight[0], neuronset[i - 1].weight[1]);
      glVertex2f(neuronset[i].weight[0], neuronset[i].weight[1]);
      glEnd();
    }
  }
  draw_text(5, 5, "nb iter: %d", cpt);
  glColor3f(1.0, 1.0, 1.0);
  #endif

  glFlush();
  glutSwapBuffers();
}
// VOTRE CODE DE KOHONEN ICI
void idle(){
  int winner;
  if(calc){
    cpt++;
    #if COMPRESSION
    // Pour mieux optimiser, on va tricher un petit peu, en ne prenant en compte que les
    // pixels dont le code RGB est superieur au nombre de couleurs que l'on souhaite avoir apres compression
    for(int i=0; i<DATA_SIZE; i++){
      if(dataset[i].data[0] > COLORS && (dataset[i].data[1] > COLORS && dataset[i].data[2] > COLORS)){
        potentiel(dataset[i]);
        activity();
        winner = indiceNeuron();
        updateNeuron(dataset[i], winner);
        // progress bar ;)
        printf("\rIn progress %d %%", (i*100/DATA_SIZE));
        // sert a ne pas refresh le terminal à chaque ecriture
        fflush(stdout);
      }else{
        neuronset[i].weight[0] =  dataset[i].data[0];
        neuronset[i].weight[1] =  dataset[i].data[1];
        neuronset[i].weight[2] =  dataset[i].data[2];
      }
    }
    printf("finish idle\n");
    writeImage();
    exit(0);
    #else
    #if SORTDATA
    Dataset data = sortData();
    potentiel(data);
    activity();
    winner = indiceNeuron();
    updateNeuron(data, winner);
    #else
    printf("sss\n");
    for(int i=0; i<SIZE; i++){
      potentiel(dataset[i]);
      activity();
      winner = indiceNeuron();
      updateNeuron(dataset[i], winner);
    }
    #endif
    #endif
    glutPostRedisplay();
  }
}


void clavier(unsigned char touche, int x, int y) {
  switch (touche) {
    case 'p':
    calc = !calc;
    break;

    case 'q': /* la touche 'q' permet de quitter le programme */
    exit(0);
  } /* switch */

} /* clavier */


void reshape(GLsizei newwidth, GLsizei newheight){
  // On ecrase pas width et height dans le cas image car il s'agira de la taille de l'image
  #if MODE
  #else
  width = newwidth;
  height = newheight;
  #endif
  // Set the viewport to cover the new window
  glViewport(0, 0,newwidth ,newheight );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  #if MODE
  glOrtho(0.0, width, height, 0.0, -1, 1);
  #else
  glOrtho(0.0, 200, 200, 0.0, -1, 1);
  #endif

  glMatrixMode(GL_MODELVIEW);

  glutPostRedisplay();
}


/* getion des boutons de  la souris*/
void mouse(int bouton, int etat, int x, int y) {
  /* si on appuie sur la bouton de gauche */
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_DOWN) {
    presse = 1; // vrai
    xold = x; // sauvegarde de la position de la souris
    yold = y;
  }

  /* si on relache la souris */
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_UP) {
    presse = 0; // faux
  }
} /* mouse */



/*gestion des mouvements de la souris */
void mousemotion(int x, int y) {
  if (presse) { /* si le bouton gauche est presse */
    /* on mofifie les angles de rotation de l'objet en fonction de la position actuelle de la souris et de la derniere position sauvegard?e */
    anglex = anglex + (x - xold);
    angley = angley + (y - yold);
    glutPostRedisplay();
  }

  xold = x; /* sauvegarde des valeurs courante des positions de la souris */
  yold = y;
} /* mousemotion */

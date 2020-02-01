#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "image.h"
#include "funkcije.h"

/* Imena fajlova sa teksturama. */
#define SKY "textures/sky.bmp"
#define ROAD "textures/stars.bmp"
#define MARS "textures/mars.bmp"
#define JUPITER "textures/jupiter.bmp"
#define MERCURY "textures/mercury.bmp"
#define NEPTUNE "textures/neptune.bmp"

/* Ostale konstante. */
#define INTERVAL 7
#define ROAD_TIMER_ID 1
#define JUMP_INTERVAL 1
#define JUMP_TIMER_ID 2
#define NUM_OF_OBSTACLES 95
#define PI 3.1415926535

/* Kodovi tastera */
#define SPACEBAR 32
#define ESC 27

/* Identifikatori tekstura. */
static GLuint textures[6];

/* Dimenzije prozora */
static int window_width, window_height;

/* OpenGL inicijalizacija. */
static void initialize(void);

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_special_key_press(int key, int x, int y);
static void on_timer(int timer_id);
static void on_jump_timer(int timer_id);

/* Koordinate traktora. */
static float x_tractor = 3.0;
static float y_tractor = -0.5;
static float z_tractor = 0;
static float wheel_rotation = 0;
static float has_jumped = 0;
static float time_passed = 0;

/* Koordinate staze. */
static float x_road = 10;
static float y_road = -1;
static float z_road = 0;
static float road_width = 6;
static float road_length = 500;

/* Koordinate prepreka. */
static int obstacles_x[NUM_OF_OBSTACLES];
static int obstacles_y[NUM_OF_OBSTACLES];
static int obstacles_z[NUM_OF_OBSTACLES];
static int obstacles_type[NUM_OF_OBSTACLES];

/* Da li je igra u toku ili ne. */
static int game_active = 0;

/* Promenljive za cuvanje i ispis rezultata. */
static int score = 0;
static char score_string[20];

int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Svemirski traktor");
    glutSetIconTitle("Svemirski traktor");

    /* Registruju se callback funkcije. */
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
    glutSpecialFunc(on_special_key_press);
    glutTimerFunc(INTERVAL, on_timer, ROAD_TIMER_ID);
    glutTimerFunc(JUMP_INTERVAL, on_jump_timer, JUMP_TIMER_ID);

    /* Random seed. */
    srand(time(NULL));

    /* Generisanje koordinata za prepreke. */
    generate_obstacles(NUM_OF_OBSTACLES, obstacles_x, obstacles_y, obstacles_z,
       obstacles_type, y_road);

    /* Obavlja se OpenGL inicijalizacija. */
    initialize();

    /* Program ulazi u glavnu petlju. */
    glutMainLoop();

    return 0;
}

static void initialize(void)
{
    /* Objekat koji se koristi za ucitavanje tekstura iz fajla. */
    Image* image;

    /* Pozicija i boja svetla. Sve koordinate jednake = belo svetlo. */
    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    /* Postavlja se boja pozadine. */
    glClearColor(0, 0, 0, 0);

    /* Ukljucuje se testiranje z-koordinate piksela. */
    glEnable(GL_DEPTH_TEST);

    /* Ukljucuje se osvetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    /* Inicijalizacija objekta kojim cemo ucitavati. */
    image = image_init(0, 0);

    /* Generisanje identifikatora teksture. */
    glGenTextures(6, textures);

    /* Ucitavanje tekstura za nebo i stazu. */
    load_repeating_texture(image, SKY, textures[0]);
    load_repeating_texture(image, ROAD, textures[1]);

    /* Ucitavanje tekstura za planete. */
    load_planet_texture(image, MARS, textures[2]);
    load_planet_texture(image, JUPITER, textures[3]);
    load_planet_texture(image, MERCURY, textures[4]);
    load_planet_texture(image, NEPTUNE, textures[5]);

     /* Iskljucujemo aktivnu teksturu */
     glBindTexture(GL_TEXTURE_2D, 0);

     /* Unistava se objekat za citanje tekstura iz fajla. */
     image_done(image);

     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    /* Pritiskom ESC izlazi se iz igre. */
    case ESC:
        /* Oslobadjaju se korisceni resursi i zavrsava se program. */
        glDeleteTextures(6, textures);
        exit(0);
        break;
    /* Pritiskom na SPACEBAR, traktor skace. */
    case SPACEBAR:
      if(!has_jumped) {
        has_jumped = 1;
        glutTimerFunc(JUMP_INTERVAL, on_jump_timer, JUMP_TIMER_ID);
      }
      break;
    /* Pritiskom na 's' (start), zapocinje se igra. */
    case 's':
    case 'S':
        /* Za lakse testiranje igre, treba staviti uslov petlje pod komentare.
           To omogucava da se igra proizvoljno ubrzava pritiskom na taster 's',
           sto dalje omogucava da brze (ne) stignemo do kraja. */
        if (game_active != 1) {
          game_active = 1;
          glutTimerFunc(INTERVAL, on_timer, ROAD_TIMER_ID);
        }
        break;
    /* Pritiskom na 'p' (pause), pauzira se igra. */
    case 'p':
    case 'P':
        game_active = 0;
        break;
      }
}

static void on_special_key_press(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
          break;
        case GLUT_KEY_DOWN:
          break;
        case GLUT_KEY_LEFT:
            if(game_active && z_tractor < road_width / 2) {
              z_tractor += 1;
              glutPostRedisplay();
            }
            break;
        case GLUT_KEY_RIGHT:
            if(game_active && z_tractor > -road_width / 2) {
              z_tractor -= 1;
              glutPostRedisplay();
            }
            break;
    }
}

static void on_reshape(int width, int height)
{
    /* Pamti se velicina prozora. */
    window_width = width;
    window_height = height;

    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float) width / height, 1, 25);
}

static void on_timer(int timer_id) {
    if (!game_active)
        return;

    // Pomeramo ravan koja predstavlja put pozitivno u x pravcu, cime dobijamo
    // izluziju pomeranja traktora unapred.
    x_road+=0.1;

    glutPostRedisplay();
    if(game_active) {
      glutTimerFunc(INTERVAL, on_timer, ROAD_TIMER_ID);
    }
}

static void on_jump_timer(int timer_id) {
    if (!game_active || !has_jumped)
        return;

    time_passed += 0.05;

    glutPostRedisplay();
    glutTimerFunc(INTERVAL, on_jump_timer, ROAD_TIMER_ID);
}

static void on_display(void)
{
    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_COLOR_MATERIAL);

    /* Postavljanje perspektive. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(15.0, 5.0, 0.0,  // 10 3 0
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);



    /* Iscrtavanje neba (pozadine). */
    draw_skyplane(textures[0]);

    /* Iscrtavanje staze. */
    draw_road(textures[1], x_road, y_road, z_road, road_width, road_length);

    /* Iscrtavanje prepreka. */
    for(int i = 0; i < NUM_OF_OBSTACLES; i++){

      /* Provera da li nas je prepreka "prosla". Ako da, vracamo je na kraj. */
      // if(obstacles_x[i] + x_road > 10) obstacles_x[i] -= NUM_OF_OBSTACLES * DISTANCE_BETWEEN_OBSTACLES;
      /* Ova komanda bi nam trebala da je igra zaista beskonacna. Medjutim,
       da bi projekat bio "kompletniji" i zabavniji, izbacujemo je zarad sale
       na racun beskonacnosti svemira. */
      glPushMatrix();
        glTranslatef(obstacles_x[i] + x_road, obstacles_y[i], obstacles_z[i]);
        if(obstacles_type[i] == 0)
          draw_star();
        else
          draw_planet(textures[1 + obstacles_type[i]]);
      glPopMatrix();

      /* Provera kolizije. */
      if(!has_jumped && has_crashed(x_tractor, y_tractor, z_tractor, obstacles_x[i] + x_road,
         obstacles_y[i], obstacles_z[i])) {
        game_active = 0;
        print_game_over(score_string, window_width, window_height);
      }
    }

    /* Iscrtavanje traktora. */
    if(has_jumped == 1) {
        if (time_passed > 2*PI) {
            has_jumped = 0;
            time_passed = 0;
        }
    }

    draw_tractor(x_tractor, y_tractor + 1 - cos(time_passed), z_tractor, wheel_rotation);
    wheel_rotation-=1;

    score = (int)x_road - 10;
    sprintf(score_string, "%d", score);
    print(score_string, GLUT_BITMAP_TIMES_ROMAN_24, window_width - 80,
      window_height - 30, 1, 1, 1, window_width, window_height);

    if(x_road > 510) {
      print_game_won(score_string, window_width, window_height);
      game_active = 0;
    }

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

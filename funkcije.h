/* Konstante za velicinu traktora. */
#define SMALL_WHEEL_WIDTH (0.25)
#define SMALL_WHEEL_R (0.25)
#define BIG_WHEEL_WIDTH (0.3)
#define BIG_WHEEL_R (0.5)

#define DISTANCE_BETWEEN_OBSTACLES 5


/* Funkcije za iscrtavanje. */
void draw_wheel(float wheel_r, float wheel_width, float wheel_rotation);
void draw_wheels(float wheel_rotation);
void draw_cabin(void);
void draw_tractor(float x, float y, float z, float wheel_rotation);
void draw_skyplane(GLuint texture);
void draw_road(GLuint texture, float x, float y, float z, float width, float length);
void draw_planet(GLuint texture);
void draw_star(void);
void draw_obstacle(GLuint texture);

/* Ostale funkcije. */
void load_planet_texture(Image* image, char* file_name, GLuint binding);
void load_repeating_texture(Image* image, char* file_name, GLuint binding);
int has_crashed(float x_tractor, float y_tractor, float z_tractor, float x, float y, float z);
void generate_obstacles(int n, int obstacles_x[],
    int obstacles_y[], int obstacles_z[], int obstacles_type[], float y_road);
void print(char *text, void *font, int x, int y, float r, float g, float b, int window_width, 	  int window_height);
void print_game_over(char *score_string, int window_width, int window_height);
void print_game_won(char *score_string, int window_width, int window_height);

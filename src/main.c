#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/util.h>
#define GLEW_STATIC
#include <GL/glew.h>
/* #include <GL/glu.h> */
/* #include <GL/glext.h> */
#include <stdio.h>
#include <stdint.h>

/* GLFW include */
#include <GLFW/glfw3.h>

/* cglm include */
#include <cglm/cglm.h>

// constants
const uint32_t HEIGHT = 600;
const uint32_t WIDTH = 800;
const char *TITLE = "OpenGL Window";

vec3 player_position = {0.0f, 0.0f, 0.0f};

/* Shaders */
const char *vertexShaderSource =
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0);\n"
    "}\n\0";

/* globals */
GLFWwindow* window;

/* Custom defines */
#define TRUE 1
#define FALSE 0

/* Forward declaration of functions */
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, float camera_speed);
void Destroy(GLFWwindow* window);

int main(void) {
  int return_value = 0;
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create the window
  window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
  if (window == NULL) {
    printf("Failed to create window!\n");
    Destroy(window);
    return_value = -1;
    return return_value;
  }
  glfwMakeContextCurrent(window);

  // need to initialize glew after initialing the current context in glfw
  glewInit();

  // generate viewport
  glViewport(0, 0, WIDTH, HEIGHT);

  printf("Viewport set\n");
  
  // callback function for resizing window handling
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /* shape data */
  /* GLfloat vertices[] = { */
  /*   // first triangle */
  /*    0.5f,  0.5f, 0.0f,  // top right */
  /*    0.5f, -0.5f, 0.0f,  // bottom right */
  /*   -0.5f,  0.5f, 0.0f,  // top left  */
  /*   // second triangle */
  /*    0.5f, -0.5f, 0.0f,  // bottom right */
  /*   -0.5f, -0.5f, 0.0f,  // bottom left */
  /*   -0.5f,  0.5f, 0.0f   // top left */
    
  /* }; */

  /* GLfloat vertices[] = { */
  /*   // first plane front */
  /*   // x     y     z        r     g     b */
  /*    0.5f,  0.5f, 0.0f,   0.5f, 0.0f, 0.0f,  // top right */
  /*    0.5f, -0.5f, 0.0f,   0.0f, 0.5f, 0.0f, // bottom right */
  /*   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 0.5f,  // bottom left */
  /*   -0.5f,  0.5f, 0.0f,   0.5f, 0.0f, 0.5f, // top left */

  /*   // second plane back */
  /*   // x     y     z        r     g     b */
  /*    0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,  // top right */
  /*    0.5f, -0.5f, -0.5f,   0.0f, 0.5f, 0.0f, // bottom right */
  /*   -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.5f,  // bottom left */
  /*   -0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.5f, // top left */
  /* };*/

    GLfloat vertices[] = {
    // first plane front
    // x     y     z        r     g     b
     0.5f,  0.5f, 0.5f,   0.5f, 0.0f, 0.0f,  // top right 0
     0.5f, -0.5f, 0.5f,   0.0f, 0.5f, 0.0f, // bottom right 1
    -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 0.5f,  // bottom left 2 
    -0.5f,  0.5f, 0.5f,   0.5f, 0.0f, 0.5f, // top left 3

    // second plane back
    // x     y     z        r     g     b
     0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,  // top right 4
     0.5f, -0.5f, -0.5f,   0.0f, 0.5f, 0.0f, // bottom right 5
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.5f,  // bottom left 6
    -0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.5f, // top left 7
  };
  
  GLuint indices[] = {
    // plane 1 front
    0, 1, 3, // first triangle
    1, 2, 3, // second triangle

    // plane 2 back
    4, 5, 7,
    5, 6, 7,

    // plane 3 right
    0, 4, 5,
    5, 1, 0,

    //plane 4, left
    3, 7, 6,
    6, 2, 3,

    // plane 5, bottom
    5, 6, 1,
    1, 2, 6,

    // plane 6, top
    0, 3, 4,
    4, 7, 3
    
  };

  unsigned int positions_size = 3;
  vec3 positions[4] = {
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f},
    { 2.0f, 1.0f, 1.0f },
  };

  printf("Setting up VAO\n");
  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);

  printf("Making VBO\n");
  /* setting up VBO, */
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // linking vertex attributes
  printf("linking vertices attributes\n");
  // position attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);

  // color attributes
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  
  // bind the EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  printf("Making shaders\n");
  /* compile shaders */
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  printf("Shaders complete\n");
  
  printf("Here\n");

  // draw in wireframe mode
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_DEPTH_TEST);

  /* camera */
  vec3 cameraPos = { 0.0f, 0.0f, 3.0f };
  vec3 cmameraTarget = { 0.0f, 0.0f, 0.0f };
  vec3 cameraDirection;

  /* speed */
  float delta_time = 0.0f;
  float last_frame = 0.0f;
  float current_frame = 0.0f;
  float camera_speed = 0.0f;
  
  while(!glfwWindowShouldClose(window)) {
    // time stuff
    current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    camera_speed = 2.5f * delta_time;
    
    // process some input
    processInput(window, camera_speed);

    glUseProgram(shaderProgram);

    
    /* sample math stuff */
    // models for model, view, and projection
    mat4 model = GLM_MAT4_IDENTITY;
    mat4 view = GLM_MAT4_IDENTITY;
    mat4 projection = GLM_MAT4_IDENTITY;

    // math for model
    vec3 scale_vector = { 0.75f, 0.75f, 0.75f };
    /* vec3 translate_vector = {0.5f, -0.5f, 0.5f }; */
    glm_scale_to(model, scale_vector, model);
    /* glm_translate(model, translate_vector); */
    /* glm_rotate_z(model, glm_rad(30.0f), model); */
    float rotation = (float)glfwGetTime();
    /* glm_rotate_y(model, rotation, model); */
    /* glm_rotate_z(model, rotation, model); */
    /* glm_rotate_x(model, rotation, model); */
    glm_rotate(model, rotation, (vec3){ 1.0f, 1.0f, 1.0f });

    // math for view
    /* vec3 translate_vector = { 0.0f, 0.0f, -10.0f }; */
    glm_translate_to(view, player_position, view);

    // math for projection
    glm_perspective(glm_rad(45.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f, projection);
    
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    
    /* glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model); */
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection);

    // rendering here
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VAO);
    /* glDrawArrays(GL_TRIANGLES, 0, 3); */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    for (unsigned int i = 0; i < positions_size; ++i) {
      glm_translate(model, positions[i]);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);
      glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
    }
    /* glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0); */
    glBindVertexArray(0);

    
    // check and call events and swap the buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  Destroy(window);
  
  return return_value;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float camera_speed) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, TRUE);
  }
  if(glfwGetKey(window, GLFW_KEY_A)) {
    player_position[0] += camera_speed;
  }
  if(glfwGetKey(window, GLFW_KEY_D)) {
    player_position[0] -= camera_speed;
  }
  if(glfwGetKey(window, GLFW_KEY_W)){
    player_position[2] += camera_speed;
  }
  if(glfwGetKey(window, GLFW_KEY_S)) {
    player_position[2] -= camera_speed;
  }
  if(glfwGetKey(window, GLFW_KEY_R)) {
    player_position[0] = 0;
    player_position[1] = 0;
    player_position[2] = 0;
  }
}

void Destroy(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}


#include <stdint.h>
#include <math.h>
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

const GLchar* vsource =
    "#version 130\n"
    "in vec2 position;\n"
    "in float color;\n"
    "out float Color;\n"
    "void main() {\n"
    "Color = color;\n"
    "gl_Position = vec4(position, 0.0, 1.0);\n"
    "}";

const GLchar* fsource =
    "#version 130\n"
    "in float Color;\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "outColor = vec4(Color, Color, Color, 1.0);\n"
    "}";

int main(int argc, char* argv[])
{
    /* Setup SDL */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    /* Create an OpenGL window */
    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    glewInit();


    /* Set up a VAO */
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float vertices[] = {
        0.0, 1.0f, 0.25f,
        1.0f, -1.0f, 0.5f,
        -1.0f, -1.0f, 0.75f
    };

    /* Get a VBO */
    GLuint vbo;
    glGenBuffers(1, &vbo);

    /* load vertices to VBO */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW);

    GLint compile_status;

    /* add the vertex shader */
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vsource, NULL);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        printf("vertex shader aint compile\n");
        return 1;
    }

    /* add the fragment shader */
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fsource, NULL);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        printf("fragment shader aint compile\n");
        return 1;
    }

    /* build a GLSL program */
    GLuint shader_prog = glCreateProgram();
    glAttachShader(shader_prog, vshader);
    glAttachShader(shader_prog, fshader);
    glBindFragDataLocation(shader_prog, 0, "outColor");

    /* Use it! */
    glLinkProgram(shader_prog);
    glUseProgram(shader_prog);
    GLint pos_attr = glGetAttribLocation(shader_prog, "position");
    glEnableVertexAttribArray(pos_attr);
    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    GLint col_attr = glGetAttribLocation(shader_prog, "color");
    glEnableVertexAttribArray(col_attr);
    glVertexAttribPointer(col_attr, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));

    /* main event loop */
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(window);
    }

    /* Shut down */
    glDeleteProgram(shader_prog);
    glDeleteShader(fshader);
    glDeleteShader(vshader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(ctx);
    SDL_Quit();
    return 0;
}

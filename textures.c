#include <stdint.h>
#include <math.h>
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <SOIL/SOIL.h>

const GLchar* vsource =
    "#version 130\n"
    "in vec2 position;\n"
    "in vec3 color;\n"
    "in vec2 texcoord;\n"
    "out vec3 Color;\n"
    "out vec2 Texcoord;\n"
    "void main() {\n"
    "Texcoord = texcoord;\n"
    "Color = color;\n"
    "gl_Position = vec4(position, 0.0, 1.0);\n"
    "}";

const GLchar* fsource =
    "#version 130\n"
    "in vec3 Color;\n"
    "in vec2 Texcoord;\n"
    "out vec4 outColor;\n"
    "uniform sampler2D texKitten;\n"
    "uniform sampler2D texPuppy;\n"
    "uniform float mixFactor;\n"
    "void main() {\n"
    "vec4 colKitten = texture(texKitten, Texcoord);\n"
    "vec4 colPuppy = texture(texPuppy, Texcoord);\n"
    "outColor = mix(colKitten, colPuppy, mixFactor);\n"
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
        //  Position      Color             Texcoords
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
    };

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    /* Get a VBO */
    GLuint vbo;
    glGenBuffers(1, &vbo);

    /* load vertices to VBO */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 7 * 4 * sizeof(float), vertices, GL_STATIC_DRAW);

    /* Get an EBO */
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), elements, GL_STATIC_DRAW);

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

    /* Specify vertex buffer layout */
    GLint pos_attr = glGetAttribLocation(shader_prog, "position");
    glEnableVertexAttribArray(pos_attr);
    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

    GLint col_attr = glGetAttribLocation(shader_prog, "color");
    glEnableVertexAttribArray(col_attr);
    glVertexAttribPointer(col_attr, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));

    GLint tex_attr = glGetAttribLocation(shader_prog, "texcoord");
    glEnableVertexAttribArray(tex_attr);
    glVertexAttribPointer(tex_attr, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));

    /* Make a buffer for the 2D texture */
    GLuint tex[2];
    glGenTextures(2, tex);

    /* Load the image */
    int imgwidth, imgheight;
    unsigned char* image = SOIL_load_image("kitten.png", &imgwidth, &imgheight, 0, SOIL_LOAD_RGB);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgwidth, imgheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shader_prog, "texKitten"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    image = SOIL_load_image("puppy.png", &imgwidth, &imgheight, 0, SOIL_LOAD_RGB);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgwidth, imgheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shader_prog, "texPuppy"), 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* main event loop */
    SDL_Event event;
    GLint mixfac = glGetUniformLocation(shader_prog, "mixFactor");
    while (1) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
        }

        glUniform1f(mixfac, sin(SDL_GetTicks() / 250.0f) / 2 + 0.5f);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
    }

    /* Shut down */
    glDeleteTextures(2, tex);
    glDeleteProgram(shader_prog);
    glDeleteShader(fshader);
    glDeleteShader(vshader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(ctx);
    SDL_Quit();
    return 0;
}

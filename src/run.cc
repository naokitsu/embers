//
// Created by Naokitsu on 1/15/2024.
//
#include "embers/run.h"

#include <ranges>
#include <bits/ranges_algo.h>

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int run() {
  using namespace embers;

  const int height = 900;
  const int width = 2300;

  GLFWwindow *window;
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
      std::cout << "Failed to create GLFW window" << std::endl;
      return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
    }
  }

  glViewport(0, 0, width, height);
  glClearColor(.2f, .3f, .3f, 1.f);

  const char *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec4 vertexColor;\n"
    "void main() {\n"
    "gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);\n"
    "vertexColor = vec4(aColor, 1.0);"
    "}\0";

  const std::vector fragment_shader_sources =
  {
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n"
    "uniform float ourColor;\n"
    "void main() {\n"
    "FragColor = vertexColor / ourColor; \n"
    "}\0",

    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n"
    "uniform float ourColor;\n"
    "layout(pixel_center_integer) in vec4 gl_FragCoord;\n"
    "void main() {\n"
    "FragColor = vertexColor / (ourColor * 2.0); \n"
    "}\0"
  };
  constexpr int x = 2;

  shader::Program programs[x];
  {
    auto shader_expected = shader::Source(vertex_shader_source, shader::Type::kVertex).Compile();
    if (!shader_expected.has_value()) {
      std::cout << shader_expected.error().Message() << std::endl;
      return -1;
    }
    const shader::Shader vertex_shader = shader_expected.value();
    for (int i = 0; i < x; ++i) {
      auto f_shader_expected = std::move(shader::Source(fragment_shader_sources[i], shader::Type::kFragment).Compile());
      if (!f_shader_expected.has_value()) {
        std::cout << f_shader_expected.error().Message() << std::endl;
        return -1;
      }
      auto program_expected = shader::Program::Builder()
                              .AttachShader(vertex_shader)
                              .AttachShader(f_shader_expected.value())
                              .Link();
      if (!program_expected.has_value()) {
        std::cout << program_expected.error().Message() << std::endl;
        return -1;
      }
      programs[i] = program_expected.value();
    }
  }

  constexpr float vertices[x][3 * 5] = {
    {
      -0.5f, 0.7f, 1.0f, 0.0f, 0.0f,
      0.7f, 0.7f, 0.0f, 1.0f, 0.0f,
      0.7f, -0.7f, 0.0f, 0.0f, 1.0f
    },
    {
      -0.7f, 0.7f, 1.0f, 1.0f, 0.0f,
      0.5f, -0.7f, 1.0f, 0.0f, 1.0f,
      -0.7f, -0.7f, 0.0f, 1.0f, 1.0f
    }
  };

  constexpr unsigned int indices[] = {
    0, 1, 2,
    3, 2, 0
  };

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  unsigned int element_buffer_object;
  glGenBuffers(1, &element_buffer_object);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  unsigned int vertex_array_object[x];
  unsigned int vertex_buffer_object[x];

  glGenVertexArrays(x, vertex_array_object);
  glGenBuffers(x, vertex_buffer_object);

  for (int i = 0; i < x; ++i) {
    glBindVertexArray(vertex_array_object[i]);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i]), vertices[i], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  const int max_fps = 30;
  const double seconds_per_frame = 1.0 / max_fps;

  double t1 = glfwGetTime(), t2;
  while (!glfwWindowShouldClose(window)) {
    auto t2 = t1;
    t1 = glfwGetTime();
    auto sleep_duration = std::chrono::duration<double>(seconds_per_frame + t2 - t1);
    std::this_thread::sleep_for(sleep_duration);

    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < x; ++i) {
      programs[i]
        .use()
        .setUniform(
                    "ourColor",
                    static_cast<GLfloat>((sin(glfwGetTime()) + 1) / 2)
                   );
      glBindVertexArray(vertex_array_object[i]);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

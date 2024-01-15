//
// Created by Naokitsu on 1/14/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include <set>
#include <stdexcept>
#include <glad/glad.h>

namespace shader {

class Shader {
  public:
  enum Type {
    kVertex = GL_VERTEX_SHADER,
    kFragment = GL_FRAGMENT_SHADER,
  };
  private:
  GLuint shader_;
  public:
  explicit Shader(const char *source, Type type);
  ~Shader();

  explicit operator GLuint() {
    return shader_;
  }

};

class ShaderException : public std::runtime_error {
  public:
  ShaderException(char const* message) noexcept : std::runtime_error(message) {}; // NOLINT(*-explicit-constructor)
  char const* what() const noexcept override {
    return std::runtime_error::what();
  };
};

class Program {
  public:
  class Builder {
    GLint program_;
    std::set<GLuint> attached_shaders_;
    bool is_built_ = false;
    public:
    Builder();
    Builder &AttachShader(Shader shader);
    Builder &DetachShader(Shader shader);
    Program Link();
    ~Builder();
  };

  private:
  GLint program_;
  public:
  explicit Program(GLint program = 0);
  ~Program();

  Program &use();

  Program &setUniform(const char* name, GLboolean value);
  Program &setUniform(const char* name, GLint value);
  Program &setUniform(const char* name, GLfloat value);

  explicit operator GLuint() {
    return program_;
  }
};



}


#endif //SHADER_H

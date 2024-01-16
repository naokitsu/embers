//
// Created by Naokitsu on 1/14/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include <set>
#include <stdexcept>
#include <bits/unique_ptr.h>
#include <glad/glad.h>

namespace embers::shader {
class Shader;

enum Type {
  kVertex   = GL_VERTEX_SHADER,
  kFragment = GL_FRAGMENT_SHADER,
};

class ShaderException final: public std::runtime_error {
  public:
    explicit ShaderException(const char *message) noexcept;
    [[nodiscard]] const char *what() const noexcept override;
};

class Source {
  public:
    Source(const char *source, Type type, size_t length = 0);
    Source(std::istream &input_stream, Type type, size_t length = 0);
    Source(const Source &) = delete;
    [[nodiscard]] Shader Compile() const;
  private:
    std::unique_ptr<char[]> source_;
    Type type_;

    static std::unique_ptr<char[]> ReadAllFromStream(std::istream &input_stream);
};

class Shader {
  public:
    explicit Shader(GLuint shader = 0);
    Shader(const Shader &) = delete;
    ~Shader();
    explicit operator GLuint() const {
      return shader_;
    }
  private:
    GLuint shader_;
};

class Program {
  public:
    class Builder {
      GLint program_;
      std::set<GLuint> attached_shaders_;
      bool is_built_ = false;
      public:
        Builder();
        Builder &AttachShader(Shader &shader);
        Builder &DetachShader(Shader &shader);
        Program Link();
        ~Builder();
    };
  private:
    GLint program_;
  public:
    explicit Program(GLint program = 0);
    Program(const Program &) = delete;
    Program(Program &&program) noexcept;
    ~Program();

    Program &use();

    Program &setUniform(const char *name, GLboolean value);
    Program &setUniform(const char *name, GLint value);
    Program &setUniform(const char *name, GLfloat value);

    explicit operator GLuint() const {
      return program_;
    }

  Program &operator=(const Program &) = delete;
  Program &operator=(Program &&program) noexcept;
};
}

#endif //SHADER_H

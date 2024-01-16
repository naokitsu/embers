//
// Created by Naokitsu on 1/14/2024.
//

#include "embers/shader.h"

#include <algorithm>
#include <istream>
#include <memory>
#include <cstring>

namespace embers::shader {
// ShaderException

ShaderException::ShaderException(const char *message) noexcept : std::runtime_error(message) {}

const char *ShaderException::what() const noexcept { return std::runtime_error::what(); }

// Source

Source::Source(const char *source, const Type type, const size_t length)
  : source_(new char[length ? length + 1 : strlen(source)])
, type_(type) {
  // I don't like I have to copy the array, but I don't see a way to transfer
  // ownership to the class since `source` *can be* static and at the same time
  // can live shorter than Source
  std::copy_n(source, length ? length + 1 : strlen(source), source_.get());
  source_[(length ? length + 1 : strlen(source))] = '\0';
}

Source::Source(std::istream &input_stream, const Type type, const size_t length)
  : source_(length ? std::unique_ptr<char[]>(new char[length + 1]) : ReadAllFromStream(input_stream))
, type_(type) {
  if (length) {
    input_stream.read(source_.get(), length);
    source_[length] = '\0';
    return;
  }
}

Shader Source::Compile() const {
  GLint success = GL_FALSE;
  GLuint shader = glCreateShader(type_);
  const char *const sources = source_.get();
  glShaderSource(shader, 1, &sources, nullptr);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success != GL_FALSE) {
    return Shader(shader);
  }
  GLint log_size;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
  auto error_log = std::make_unique<char[]>(log_size);
  glGetShaderInfoLog(shader, log_size, &log_size, error_log.get());
  glDeleteShader(shader);
  throw ShaderException(error_log.get());
}

std::unique_ptr<char[]> Source::ReadAllFromStream(std::istream &input_stream) {
  std::string s(std::istreambuf_iterator<char>(input_stream), {});
  return std::unique_ptr<char[]>(s.data());
}

// Shader
Shader::Shader(GLuint shader) : shader_(shader) {}

Shader::~Shader() {
  glDeleteShader(shader_);
}

// Program Builder
Program::Builder::Builder() {
  program_ = glCreateProgram();
}

Program::Builder &Program::Builder::AttachShader(Shader &shader) {
  glAttachShader(program_, static_cast<GLuint>(shader));
  attached_shaders_.insert(static_cast<GLuint>(shader));
  return *this;
}

Program::Builder &Program::Builder::DetachShader(Shader &shader) {
  glDetachShader(program_, static_cast<GLuint>(shader));
  attached_shaders_.erase(static_cast<GLuint>(shader));
  return *this;
}

Program Program::Builder::Link() {
  glLinkProgram(program_);
  GLint success = 0;
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if (success != GL_FALSE) {
    is_built_ = true;
    return Program(program_);
  }
  GLint log_size = 0;
  glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &log_size);
  auto error_log = std::make_unique<char[]>(log_size);
  glGetProgramInfoLog(program_, log_size, &log_size, error_log.get());
  throw ShaderException(error_log.get());
}

Program::Builder::~Builder() {
  if (!is_built_) {
    glDeleteProgram(program_);
  }
  for (auto shader : attached_shaders_) {
    glDetachShader(program_, shader);
  }
}

// Program
Program::Program(GLint program) : program_(program) {}

Program::~Program() {
  glDeleteProgram(program_);
}

Program &Program::use() {
  glUseProgram(static_cast<GLuint>(program_));
  return *this;
}

Program &Program::setUniform(const char *name, GLboolean value) {
  glUniform1i(glGetUniformLocation(program_, name), static_cast<GLint>(value));
  return *this;
}

Program &Program::setUniform(const char *name, GLint value) {
  glUniform1i(glGetUniformLocation(program_, name), value);
  return *this;
}

Program &Program::setUniform(const char *name, GLfloat value) {
  glUniform1f(glGetUniformLocation(program_, name), value);
  return *this;
}
}

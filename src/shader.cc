//
// Created by Naokitsu on 1/14/2024.
//

#include "embers/shader.h"

#include <memory>

namespace shader {

Shader::Shader(const char *source, const Shader::Type type) {
  GLint success = 0;
  shader_ = glCreateShader(type);
  glShaderSource(shader_, 1, &source, nullptr);
  glCompileShader(shader_);
  glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);
  if (success != GL_FALSE) {
    return;
  }
  GLint log_size = 0;
  glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &log_size);
  const auto error_log = std::make_unique<char[]>(log_size);
  glGetShaderInfoLog(shader_, log_size, &log_size, error_log.get());
  glDeleteShader(shader_);
  throw ShaderException(error_log.get());
}

Shader::~Shader() {
  glDeleteShader(shader_);
}

Program::Builder::Builder() {
  program_ = glCreateProgram();
}

Program::Builder &Program::Builder::AttachShader(Shader shader) {
  glAttachShader(program_, static_cast<GLuint>(shader));
  attached_shaders_.insert(static_cast<GLuint>(shader));
  return *this;
}

Program::Builder &Program::Builder::DetachShader(Shader shader) {
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
  const auto error_log = std::make_unique<char[]>(log_size);
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
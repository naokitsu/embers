//
// Created by Naokitsu on 1/14/2024.
//

#include "embers/shader.h"

#include <algorithm>
#include <istream>
#include <memory>
#include <sec_api/string_s.h>

namespace embers::shader {

// ShaderException
Error::Error() noexcept : message_(std::make_unique<char[]>(0)), type_(Null) {}

Error::Error(std::unique_ptr<char[]> &&message, Type type) noexcept
  : message_(std::move(message)), type_(type) {};

Error::Error(const Error &error) : message_(error.message_.get()), type_(error.type_) {}

Error::Error(Error &&error) : message_(std::move(error.message_)), type_(error.type_) {}


const char *Error::Message() const noexcept { return message_.get(); }

Error::Type Error::ErrorType() const noexcept { return type_; }


// Source
Source::Source(const char *source, Type type, size_t length)
  : source_(new char[ length ? length + 1 : strlen(source) ]), type_(type) {
  // I don't like I have to copy the array, but I don't see a way to transfer
  // ownership to the class since `source` *can be* static and at the same time
  // can live shorter than Source
  // I would prefer the Rust's implicit move :shrug:
  std::copy(source, source + (length ? length + 1 : strlen(source)), source_.get());
  source_[(length ? length + 1 : strlen(source))] = '\0';
}

Source::Source(std::istream &input_stream, Type type, size_t length)
  : source_(new char[length+1]), type_(type) {
  input_stream.read(source_.get(), length);
  source_[length] = '\0';
}

std::expected<Shader, Error> Source::Compile() const {
  GLint success = GL_FALSE;
  GLuint shader = glCreateShader(type_);
  const char* const sources = source_.get();
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
  return std::unexpected(Error(std::move(error_log), Error::Type::Compilation));
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

std::expected<Program, Error> Program::Builder::Link() {
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
  return std::unexpected(Error(std::move(error_log), Error::Type::Linking));
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


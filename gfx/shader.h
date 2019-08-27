#ifndef FILE_SHADER_H
#define FILE_SHADER_H

#include "glUtils.h"
#include <string>
#include <map>
#include <stdexcept>
#include <eigen3/Eigen/Eigen>

#include <stdio.h>
using std::string;



//using namespace std;
//TODO: use Eigen instead
namespace gfx{
class GLSLProgramException : public std::runtime_error {
public:
    GLSLProgramException( const string & msg ) :
    std::runtime_error(msg) { }
};

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        TESS_CONTROL = GL_TESS_CONTROL_SHADER,
        TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
#ifndef __APPLE__
        COMPUTE = GL_COMPUTE_SHADER
#endif
    };
}

class GLSLProgram
{
private:
    int  handle;
    bool linked;
    std::map<string, int> uniformLocations;


    bool fileExists( const string & fileName );
    string getExtension( const char * fileName );

    // Make these private in order to make the object non-copyable
    GLSLProgram( const GLSLProgram & other ) { }
    GLSLProgram & operator=( const GLSLProgram &other ) { return *this; }

public:
    GLSLProgram();
    ~GLSLProgram();

    void   compileShader( std::string fileName ) throw (GLSLProgramException);
    void   compileShader( const char *fileName ) throw (GLSLProgramException);
    void   compileShader( const char * fileName, GLSLShader::GLSLShaderType type ) throw (GLSLProgramException);
    void   compileShader( const string & source, GLSLShader::GLSLShaderType type,
                         const char *fileName = NULL ) throw (GLSLProgramException);

    //void compileShaderFromString(char*shaderCode,GLSLShader::GLSLShaderType type);
    //void compileShaderFromString(string shaderCode,GLSLShader::GLSLShaderType type);

    void   link() throw (GLSLProgramException);
    void   validate() throw(GLSLProgramException);
    void   use() throw (GLSLProgramException);

    int    getHandle();
    bool   isLinked();

    void   bindAttribLocation( GLuint location, const char * name);
    void   bindFragDataLocation( GLuint location, const char * name );

    void   setUniform( const char *name, float x, float y, float z);
    void   setUniform( const char *name, const Eigen::Vector2f & v);
    void   setUniform( const char *name, const Eigen::Vector3f & v);
    void   setUniform( const char *name, const Eigen::Vector4f & v);
    void   setUniform( const char *name, const Eigen::Matrix3f & m);
    void   setUniform( const char *name, const Eigen::Matrix4f & m);
    void   setUniform( const char *name, float val );
    void   setUniform( const char *name, int val );
    void   setUniform( const char *name, bool val );
    void   setUniform( const char *name, GLuint val );

    GLint getAttribLocation(std::string attribName);
    GLint getUniformLocation(std::string uniformName);
    GLint  getUniformLocation(const char * name );


    void   printActiveUniforms();
    void   printActiveUniformBlocks();
    void   printActiveAttribs();

    const char * getTypeString( GLenum type );
};
}
#endif

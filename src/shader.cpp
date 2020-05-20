
#include "shader.hpp"

void LOG(const std::string& _log){
    std::cout << _log << std::endl;
}

#ifdef GL_COMPUTE_SHADER 
void ShaderProgram::print(std::string _id, ShaderProgram::Status _compComp, ShaderProgram::Status _compVert,
	ShaderProgram::Status _compGeom, ShaderProgram::Status _compFrag, ShaderProgram::Status _link, std::string _errorLog) {
	if (!printDebug) return;
	LOG("   Shader: " + std::string(_id));
	LOG("Compiling: "
		+ std::string(_compComp == Status::failed ? " X |" : _compComp == Status::success ? " S |" : " - |")
		+ std::string(_compVert == Status::failed ? " X |" : _compVert == Status::success ? " S |" : " - |")
		+ std::string(_compGeom == Status::failed ? " X |" : _compGeom == Status::success ? " S |" : " - |")
		+ std::string(_compFrag == Status::failed ? " X |" : _compFrag == Status::success ? " S |" : " - |")
		+ "\n");
	LOG("  Linking: " + std::string(_link == Status::failed ? "Failed!" : _link == Status::success ? "Success!" : " - ") + "\n");

	if (_errorLog.empty()) {
		LOG("\n");
	}
	else {
		LOG("\n" + std::string(_errorLog) + "\n");
	}

    #ifndef GL_DEBUG_OUTPUT
	printOpenGlErrors(_id + " ShaderProgram::print");
    #endif
	LOG("\n");
}
#else
void ShaderProgram::print(std::string _id, ShaderProgram::Status _compVert,
	ShaderProgram::Status _compGeom, ShaderProgram::Status _compFrag, ShaderProgram::Status _link, std::string _errorLog) {
	if (!printDebug) return;
	LOG("   Shader: " + std::string(_id));
	LOG("Compiling: "
		+ std::string(_compVert == Status::failed ? " X |" : _compVert == Status::success ? " S |" : " - |")
		+ std::string(_compGeom == Status::failed ? " X |" : _compGeom == Status::success ? " S |" : " - |")
		+ std::string(_compFrag == Status::failed ? " X |" : _compFrag == Status::success ? " S |" : " - |")
		+ "\n");
	LOG("  Linking: " + std::string(_link == Status::failed ? "Failed!" : _link == Status::success ? "Success!" : " - ") + "\n");

	if (_errorLog.empty()) {
		LOG("\n");
	}
	else {
		LOG("\n" + std::string(_errorLog) + "\n");
	}

	printOpenGlErrors(_id + " ShaderProgram::print");
	LOG("\n");
}
#endif
ShaderProgram::ShaderProgram() : ShaderProgram("") {}
ShaderProgram::ShaderProgram(const std::string& _id) : id(_id){}

ShaderProgram::~ShaderProgram(){}

bool ShaderProgram::compile(const char* _compute, const char* _vertex, const char* _geom, const char* _frag) {
	#ifdef GL_COMPUTE_SHADER
    Status compStatus = Status::missing;
    #endif
	Status vertStatus = Status::missing;
	Status geomStatus = Status::missing;
	Status fragStatus = Status::missing;
	Status linkStatus = Status::missing;

	if (compute != -1) {
		glDeleteShader(compute);
		compute = -1;
	}
	if (vertex != -1) {
		glDeleteShader(vertex);
		vertex = -1;
	}
	if (geom != -1) {
		glDeleteShader(geom);
		geom = -1;
	}
	if (frag != -1) {
		glDeleteShader(frag);
		frag = -1;
	}
	if (program != -1) {
		glDeleteShader(program);
		program = -1;
	}

	//Compile Compute
#ifdef GL_COMPUTE_SHADER
	if (_compute != NULL && _compute[0] != '\0') {
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &_compute, nullptr);
		glCompileShader(compute);
		GLint isCompiled = 0;
		glGetShaderiv(compute, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(compute, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(compute);
			compStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		}
		else compStatus = Status::success;
	}
#endif

	//Compile Vertex
	if (_vertex != NULL && _vertex[0] != '\0') {
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &_vertex, nullptr);
		glCompileShader(vertex);
		GLint isCompiled = 0;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(vertex, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(vertex);
			vertStatus = Status::failed;
            #ifdef GL_COMPUTE_SHADER
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			#else
            print("vert", vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
            #endif
            return false;
		}
		else vertStatus = Status::success;
	}

	//Compile Geom
	if (_geom != NULL && _geom[0] != '\0') {
		geom = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geom, 1, &_geom, nullptr);
		glCompileShader(geom);
		GLint isCompiled = 0;
		glGetShaderiv(geom, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(geom, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(geom, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(geom);
			geomStatus = Status::failed;
			#ifdef GL_COMPUTE_SHADER
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			#else
            print("geom", vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
            #endif
			return false;
		}
		else geomStatus = Status::success;
	}

	//Compile Frag
	if (_frag != NULL && _frag[0] != '\0') {
		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &_frag, nullptr);
		glCompileShader(frag);
		GLint isCompiled = 0;
		glGetShaderiv(frag, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(frag, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(frag);
			fragStatus = Status::failed;
			#ifdef GL_COMPUTE_SHADER
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			#else
            print("frag", vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
            #endif
			return false;
		}
		else fragStatus = Status::success;
	}

	//Link
	program = glCreateProgram();
	if (_compute != nullptr) glAttachShader(program, compute);
	if (_vertex != nullptr) glAttachShader(program, vertex);
	if (_geom != nullptr) glAttachShader(program, geom);
	if (_frag != nullptr) glAttachShader(program, frag);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
		if (compute != -1)glDeleteShader(compute);
		if (vertex != -1)glDeleteShader(vertex);
		if (geom != -1)glDeleteShader(geom);
		if (frag != -1)glDeleteShader(frag);
		if (program != -1) glDeleteProgram(program);
		linkStatus = Status::failed;
		#ifdef GL_COMPUTE_SHADER
		print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
		#else
        print(id, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
        #endif
		return false;
	}
	else linkStatus = Status::success;

	if (_compute != NULL && _compute[0] != '\0')glDetachShader(program, compute);
	if (_vertex != NULL && _vertex[0] != '\0')glDetachShader(program, vertex);
	if (_geom != NULL && _geom[0] != '\0')glDetachShader(program, geom);
	if (_frag != NULL && _frag[0] != '\0')glDetachShader(program, frag);
    #ifdef GL_COMPUTE_SHADER
	print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, "");
    #else
    print("linking", vertStatus, geomStatus, fragStatus, linkStatus, "");
    #endif
	unbind();
	return true;
}

GLint ShaderProgram::getHandle() {
    return program;
}

void ShaderProgram::bind() {
	glUseProgram(getHandle());
    #ifndef GL_DEBUG_OUTPUT
	printOpenGlErrors(id + " ShaderProgram::bind");
    #endif
}

void ShaderProgram::unbind() {
	glUseProgram(0);
    #ifndef GL_DEBUG_OUTPUT
	printOpenGlErrors(id + " ShaderProgram::unbind");
    #endif
}
#include "efpch.h"

#include "ComputeShader.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <../tracy/TracyOpenGL.hpp>

using namespace std;

namespace efgl 
{

	ComputeShader::ComputeShader(const std::string& filePath)
		: m_FilePath(filePath)
	{
		std::string src = parseShader(filePath);
		m_RendererID = createShader(src);
	}


	ComputeShader::~ComputeShader()
	{
		GLCall(glDeleteProgram(m_RendererID));
	}

	void ComputeShader::Dispatch(int x, int y, int z)
	{
		TracyGpuZoneC("ComputeShader::Dispatch", tracy::Color::Beige);
		GLCall(glDispatchCompute(x, y, z));
	}

	void ComputeShader::DispatchIndirect(unsigned int byteOffset) 
	{
		TracyGpuZoneC("ComputeShader::DispatchIndirect", tracy::Color::LightSteelBlue);
		GLCall(glDispatchComputeIndirect(byteOffset));
	}

	void ComputeShader::BindBlockIndex(const std::string& blockName, unsigned int slot)
	{
		int location = GL_INVALID_INDEX;
		if (m_UniformBlockLocationCache.find(blockName) != m_UniformBlockLocationCache.end())
			location = m_UniformBlockLocationCache[blockName];
		else 
		{
			GLCall(location = glGetUniformBlockIndex(m_RendererID, blockName.c_str()));
			// This is for more info on the error that will be thrown in glUniformBlockBinding
			if (location == GL_INVALID_INDEX)
				cerr << "EF_ERROR: Uniform block " << blockName << " doesn't exist in shader at " << m_FilePath << endl;
			else m_UniformBlockLocationCache[blockName] = location;
		}

		GLCall(glUniformBlockBinding(m_RendererID, location, slot));
	}

	std::string ComputeShader::parseShader(const string& filepath)
	{
		std::ifstream stream(filepath);

		if (!stream.good())
		{
			std::cerr << "Failed to load shader at " << filepath << std::endl;
            std::cerr << "Working directory is " << std::filesystem::current_path() << std::endl;
		}
		
		string line;
		stringstream ss;
		while (getline(stream, line))
		{
			ss << line << '\n';
		}

		return ss.str();
	}

	unsigned int ComputeShader::compileShader(unsigned int type, const string& source)
	{
		GLCall(unsigned int id = glCreateShader(type));
		const char* src = source.c_str();
		GLCall(glShaderSource(id, 1, &src, nullptr));
		GLCall(glCompileShader(id));

		int result;
		GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
			auto msg = std::make_unique<char[]>(length);
			GLCall(glGetShaderInfoLog(id, length, &length, msg.get()));
			cerr << "Failed Compilation of compute shader: " << m_FilePath << endl;
			cerr << msg.get() << endl;
			GLCall(glDeleteShader(id));
			return 0;
		}

		return id;
	}

	unsigned int ComputeShader::createShader(const string& source)
	{
		GLCall(unsigned int program = glCreateProgram());
		unsigned int cs = compileShader(GL_COMPUTE_SHADER, source);

		GLCall(glAttachShader(program, cs));
		GLCall(glLinkProgram(program));
		GLCall(glValidateProgram(program));

		GLCall(glDeleteShader(cs));

		return program;
	}

	void ComputeShader::Bind() const
	{
		GLCall(glUseProgram(m_RendererID));
	}

	void ComputeShader::Unbind() const
	{
		GLCall(glUseProgram(0));
	}

	int ComputeShader::getUniformLocation(const string& name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];
		GLCall(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));
		if (location == -1)
			cerr << "Warning: Uniform " << name << " doesn't exist." << endl;

		m_UniformLocationCache[name] = location;
		return location;
	}
}

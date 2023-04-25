#include <fstream>
#include <sstream>
#include "shader.hpp"
#include "renderer.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "debug.hpp"

using std::string;
using std::stringstream;
using std::ifstream;

namespace srender
{
	shader::shader(const string *_shaderPath)
	{	load(_shaderPath); }

	shader::~shader()
	{	destroy(); }

	void shader::destroy() noexcept
	{
		if (m_shaderLoaded)
		{
			renderer::deleteShaderProgram(m_idProgram);
			m_shaderLoaded = false;
		}
	}

	void shader::load(const std::string *_shaderPath)
	{
		if (m_shaderLoaded)
		{
			debug::send("ERROR::SHADER::ATTEMPTING_TO_OVERLOAD_SHADER");
			return;
		}
		// If no path is given, will use fallback shader
		m_shaderPath = _shaderPath ? *_shaderPath : "";
		loadShader(shaderType::vertex);
		loadShader(shaderType::fragment);
		createShaderProgram();
	}

	void shader::use() const noexcept
	{	renderer::useShaderProgram(m_idProgram); }

	constexpr bool shader::isLoaded() const noexcept
	{	return m_shaderLoaded; }

	inline void shader::loadShader(const shaderType _type)
	{
		bool m_usingFallback = false;
		string codeString;

		if (_type == shaderType::program)
		{
			debug::send(
				"ERROR::SHADER::LOADING_INCORRECT_SHADER_TYPE",
				debug::type::note, debug::impact::large, debug::stage::mid
			);
			return;
		}

		if (m_shaderPath == "")
		{
			m_usingFallback = true;
			debug::newLine();
		}
		else
		{
			string path = m_shaderPath + byType(_type, string(".vert"), string(".frag"));

			debug::send(
				"Compiling shader \"" + path + "\"...",
				debug::type::process, debug::impact::small, debug::stage::mid, false, false
			);

			// Try to retrieve the vertex/fragment source code from filePath
			try
			{
				ifstream fileStream;
				stringstream codeStream;
				// Ensure ifstream objects can throw exceptions
				fileStream.exceptions(ifstream::failbit | ifstream::badbit);

				fileStream.open(path);
				codeStream << fileStream.rdbuf();
				fileStream.close();

				// Convert stream into string
				codeString = codeStream.str();
			}
			catch (ifstream::failure &e)
			{
				string msg = "ERROR::SHADER::"
					+ byType(_type, string("VERTEX"), string("FRAGMENT"))
					+ "::FAILURE_TO_READ_FILE::USING_FALLBACK_CODE:\n"
					+ string(e.what());

				debug::send(msg, debug::type::note, debug::impact::large, debug::stage::mid, true);

				m_usingFallback = true;
			}
		}

		if (!m_usingFallback)
		{
			if (
				!compileShader(
					byType(_type, &m_idVertex, &m_idFragment),
					_type,
					codeString.c_str()
				)
			)
			{
				m_usingFallback = true;
			}
		}

		// Separated to allow bool to potentially change
		if (m_usingFallback)
		{
			debug::send(
				"Compiling fallback code...",
				debug::type::process, debug::impact::small, debug::stage::mid, false, false
			);

			bool result;
			{
				const char *vertexFallback = "#version 330 core\n\
layout(location=0)in vec3 aPos;\
layout(location=1)in vec3 aNormal;\
layout(location=2)in vec2 aTexCoords;\
out vec3 FragPos;\
out vec3 Normal;\
out vec2 TexCoords;\
uniform vec3 u_scale=vec3(1.0);\
uniform mat4 u_camera;\
uniform mat4 u_model;\
uniform mat3 u_transposeInverseOfModel;\
void main(){\
vec4 vertModel=u_model*vec4(aPos*u_scale,1.0);\
TexCoords=aTexCoords;\
Normal=u_transposeInverseOfModel*aNormal;\
FragPos=vec3(vertModel);\
gl_Position=u_camera*vertModel;}";
				const char *fragmentFallback = "#version 330 core\n\
#define normalise normalize\n\
const int NR_DIR_LIGHTS=3;\
const int NR_POINT_LIGHTS=30;\
const int NR_SPOT_LIGHTS=30;\
const float near=0.1;\
const float far=500.0;\
out vec4 FragCol;\
in vec3 FragPos;\
in vec3 Normal;\
in vec2 TexCoords;\
struct Material{float shininess;sampler2D texture_diffuse0;sampler2D texture_specular0;};\
struct LightColour{vec3 ambient;vec3 diffuse;vec3 specular;};\
struct LightDirectional{LightColour colour;vec4 direction;};\
struct LightPoint{LightColour colour;vec4 position;float linear;float quadratic;};\
struct LightSpot{LightColour colour;vec4 position;vec4 direction;float linear;float quadratic;float cutoff;float blur;};\
uniform bool u_justColour=false;\
uniform vec3 u_viewPos;\
uniform vec3 u_colour=vec3(1.0);\
uniform Material u_material;\
uniform LightDirectional[NR_DIR_LIGHTS] u_dirLights;\
uniform LightPoint[NR_POINT_LIGHTS] u_pointLights;\
uniform LightSpot[NR_SPOT_LIGHTS] u_spotLights;\
vec3 m_normal;\
vec3 m_viewDir;\
float LineariseDepth(float pDepth){\
float z=pDepth*2.0-1.0;\
return (2.0*near*far)/(far+near-z*(far-near));}\
vec3 PhongShading(LightColour _colour,vec3 _lightDir,float _intensity){\
vec3 diffuseTex=texture(u_material.texture_diffuse0,TexCoords).rgb;\
vec3 specularTex=texture(u_material.texture_specular0,TexCoords).rgb;\
float diff=max(dot(m_normal,_lightDir),0.0);\
vec3 reflectDir=reflect(-_lightDir,m_normal);\
float spec=pow(max(dot(m_viewDir,reflectDir),0.0),u_material.shininess);\
vec3 ambient=_colour.ambient*diffuseTex;\
vec3 diffuse=_colour.diffuse*diffuseTex*diff*_intensity;\
vec3 specular=_colour.specular*specularTex*spec*_intensity;\
return ambient+diffuse+specular;}\
float CalculateAttentuation(float _dist,float _linear,float _quadratic){\
return 1.0/(1.0+_linear*_dist+_quadratic*(_dist*_dist));}\
vec3 CalculateDirectionalLighting(LightDirectional _light){\
vec3 lightDir=normalise(_light.direction.xyz);\
return PhongShading(_light.colour,lightDir,1);}\
vec3 CalculatePointLight(LightPoint _light){\
if(length(_light.linear)==0)return vec3(0);\
vec3 lightDiff=_light.position.xyz-FragPos;\
vec3 lightDir=normalise(lightDiff);\
float lightDist=length(lightDiff);\
float attenuation=CalculateAttentuation(lightDist,_light.linear,_light.quadratic);\
return PhongShading(_light.colour,lightDir,1)*attenuation;}\
vec3 CalculateSpotLight(LightSpot _light){\
if(length(_light.linear)==0)return vec3(0);\
vec3 lightDiff=_light.position.xyz-FragPos;\
vec3 lightDir=normalise(lightDiff);\
float lightDist=length(lightDiff);\
float attenuation=CalculateAttentuation(lightDist,_light.linear,_light.quadratic);\
float intensity=1;\
float theta=dot(lightDir,normalise(_light.direction.xyz));\
float epsilon=(_light.blur*(1-_light.cutoff)+_light.cutoff)-_light.cutoff;\
intensity=clamp((theta-_light.cutoff)/epsilon,0.0,1.0);\
return PhongShading(_light.colour,lightDir,intensity)*attenuation;}\
void main(){\
m_normal=normalise(Normal);\
m_viewDir=normalise(u_viewPos-FragPos);\
vec3 result;\
for(int i=0;i<NR_DIR_LIGHTS;++i)result+=CalculateDirectionalLighting(u_dirLights[i]);\
for(int i=0;i<NR_POINT_LIGHTS;++i)result+=CalculatePointLight(u_pointLights[i]);\
for(int i=0;i<NR_SPOT_LIGHTS;++i)result+=CalculateSpotLight(u_spotLights[i]);\
if(u_justColour) FragCol=vec4(u_colour,1);\
else FragCol=vec4(result*u_colour,1);\
return;}";

				result = compileShader(
					byType(_type, &m_idVertex, &m_idFragment),
					_type,
					byType<const char*&>(_type, vertexFallback, fragmentFallback)
				);
			}

			if (!result)
			{
				string msg = "ERROR::SHADER::"
					+ byType(_type, string("VERTEX"), string("FRAGMENT"))
					+ "::FALLBACK_CODE_FAILURE";
				debug::send(msg, debug::type::note, debug::impact::large, debug::stage::mid, true);

				exit(2);
			}
		}

		debug::send("Success!");
	}

	inline bool shader::compileShader(
		uint32_t *_id,
		shaderType _type,
		const char *_code
	) noexcept
	{
		assert(_type != shaderType::program && "Incorrect shaderType passed");

		// Creates a shader object and assigns to an id
		switch (_type)
		{
		case shaderType::vertex:
			*_id = renderer::createVertexShader();
			break;
		case shaderType::fragment:
			*_id = renderer::createFragmentShader();
			break;
		default:
			debug::send(
				"ERROR::SHADER::ATTEMPTING_TO_COMPILE_UNKNOWN_SHADER_TYPE",
				debug::type::note, debug::impact::large, debug::stage::mid, true
			);
			return false;
		}

		// Loads the shader code into the shader object
		renderer::loadShaderSource(*_id, _code);
		// Compiles the shader at run-time
		renderer::compileShader(*_id);
		// Performs error checking on the shader
		return checkForErrors(_id, _type);
	}

	inline void shader::createShaderProgram() noexcept
	{
		// Creates a shader program object assigned to id, this sets it as the active shader
		m_idProgram = renderer::createShaderProgram(m_idVertex, m_idFragment);
		// Performs error checking on the shader program
		if (!checkForErrors(&m_idProgram, shaderType::program))
		{
			renderer::deleteShaderProgram(m_idProgram);
			return;
		}
		// We no longer need the vertex and fragment shaders
		renderer::deleteShader(m_idVertex);
		renderer::deleteShader(m_idFragment);
		// Sets the shader as the active one
		renderer::useShaderProgram(m_idProgram);
		m_shaderLoaded = true;
	}

	inline bool shader::checkForErrors(
		const uint32_t *_shaderID,
		const shaderType _type
	) const noexcept
	{
		// Boolean output as int32
		int32_t success;
		char infoLog[512];

		if (_type == shaderType::program)
		{
			// Retrieves the compile status of the given shader by id
			renderer::getProgramiv(*_shaderID, &success);
			if (!success)
			{
				// In the case of a failure it loads the log and outputs
				renderer::getProgramInfoLog(*_shaderID, infoLog, 512);
				debug::send(
					"ERROR::SHADER::PROGRAM::LINKING_FAILED:\n" + string(infoLog),
					debug::type::note, debug::impact::large, debug::stage::mid, true, false
				);
				return false;
			}
		}
		else
		{
			// Retrieves the compile status of the given shader by id
			renderer::getShaderiv(*_shaderID, &success);
			if (!success)
			{
				// In the case of a failure it loads the log and outputs
				renderer::getShaderInfoLog(*_shaderID, infoLog, 512);
				string msg = "ERROR::SHADER::"
					+ byType(_type, string("VERTEX"), string("FRAGMENT"))
					+ "::COMPILATION_FAILED:\n"
					+ string(infoLog);
				debug::send(msg, debug::type::note,debug::impact::large, debug::stage::mid, true, false);
				return false;
			}
		}
		return true;
	}

	template<typename T>
	inline T shader::byType(
		const shaderType _type,
		T _vertex,
		T _fragment
	) const noexcept
	{
		assert(_type != shaderType::program && "Incorrect shaderType passed");
		return (_type == shaderType::vertex ? _vertex : _fragment);
	}

	void shader::setBool(string _name, const bool _value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setBool(m_idProgram, location, _value);
	}

	void shader::setInt(string _name, const int32_t _value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setInt(m_idProgram, location, _value);
	}

	void shader::setUint(string _name, const uint32_t _value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setUint(m_idProgram, location, _value);
	}

	void shader::setFloat(string _name, const float _value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setFloat(m_idProgram, location, _value	);
	}

	void shader::setFloat2(string _name, const glm::vec2 *_value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setFloat2(m_idProgram, location, &(*_value)[0]);
	}
	void shader::setFloat2(string _name, const glm::vec2 _value) const noexcept
	{	setFloat2(_name, &_value); }

	void shader::setFloat3(string _name, const glm::vec3 *_value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setFloat3(m_idProgram, location, &(*_value)[0]);
	}
	void shader::setFloat3(string _name, const glm::vec3 _value) const noexcept
	{	setFloat3(_name, &_value); }

	void shader::setFloat4(string _name, const glm::vec4 *_value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setFloat4(m_idProgram, location, &(*_value)[0]);
	}
	void shader::setFloat4(string _name, const glm::vec4 _value) const noexcept
	{	setFloat4(_name, &_value); }

	void shader::setMat3(string _name, const glm::mat3 *_value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setMat3(m_idProgram, location, &(*_value)[0][0]);
	}
	void shader::setMat3(string _name, const glm::mat3 _value) const noexcept
	{	setMat3(_name, &_value); }

	void shader::setMat4(string _name, const glm::mat4 *_value) const noexcept
	{
		int32_t location = renderer::getUniformLocation(m_idProgram, _name.c_str());
		if (location < 0)
		{
			debug::send("Attempting to set unknown uniform \""
				+ _name
				+ "\", location: "
				+ std::to_string(location)
			);
			return;
		}
		renderer::setMat4(m_idProgram, location, &(*_value)[0][0]);
	}
	void shader::setMat4(string _name, const glm::mat4 _value) const noexcept
	{	setMat4(_name, &_value); }
}

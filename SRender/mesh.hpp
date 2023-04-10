#pragma once
#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

namespace srender
{
	struct vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	/** A mesh is a collection of vertices used to draw shapes */
	class mesh
	{
		/** Hard coded vertices for a cube
		 * @deprecated No longer used since Model class
		 */
		static constexpr float s_cubeVerticesArr[288] = {
			// Positions				// Normals				  // Texture coords
		   -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
		    0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
		    0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
		    0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
		   -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,

		   -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
		    0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
		    0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
		    0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
		   -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
		   -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,

		   -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
		   -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		   -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
		   -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

		    0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
		    0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
		    0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		    0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		    0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
		    0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

		   -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
		    0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
		    0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
		    0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
		   -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
		   -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,

		   -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
		    0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
		    0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
		    0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
		   -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
		   -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f
		};

		/** Hard coded indices for a cube
		 * @deprecated No longer used since Model class
		 */
		static constexpr uint32_t s_indicesArr[36] = {
			0U,	1U,	2U,	3U,	4U,	5U,	// Face 1
			6U, 	7U,	8U,	9U,	10U,	11U,	// Face 2
			12U,	13U,	14U,	15U,	16U,	17U,	// Face 3
			18U,	19U,	20U,	21U,	22U,	23U,	// Face 4
			24U,	25U,	26U,	27U,	28U,	29U,	// Face 5
			30U,	31U,	32U,	33U,	34U,	35U	// Face 6
		};

		uint32_t m_idVAO = 0U;	// The id for the vertex attribute object
		uint32_t m_idVBO = 0U;	// The id for the vertex buffer object
		uint32_t m_idEBO = 0U;	// The id for the element buffer object

		std::vector<vertex> *m_vertices = nullptr;
		std::vector<uint32_t> *m_indices = nullptr;

	public:
		/** Takes the vertices array and places it in a vector
		 * @deprecated No longer used since Model class
		 * @return [std::vector<Vertex>] The vertex vector
		 */
		_NODISCARD static std::vector<vertex> generateVertices() noexcept;

		/** Takes the indices array and places it in a vector
		 * @deprecated No longer used since Model class
		 * @return [std::vector<uint32_t>] The indices vector
		 */
		_NODISCARD static std::vector<uint32_t> generateIndices() noexcept;

		mesh(
			const std::vector<vertex> *_vertices = nullptr,
			const std::vector<uint32_t> *_indices = nullptr
		) noexcept;
		~mesh();

		void draw() const noexcept;

		/** Get the id for the vertex attribute object
		 * @return [uint32_t] The id of the vertex attribute object
		 */
		_NODISCARD uint32_t getVAO() const noexcept;
		/** Get the id for the vertex buffer object
		 * @return [uint32_t] The id of the vertex buffer object
		 */
		_NODISCARD uint32_t getVBO() const noexcept;
		/** Get the id for the element buffer object
		 * @return [uint32_t] The id of the element buffer object
		 */
		_NODISCARD uint32_t getEBO() const noexcept;
	};
}

#pragma once
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

namespace srender
{
/** A wrapper class for a 4x4 matrix, with a bunch of utility functions */
class transform
{
	glm::mat4 m_transform;

public:
	transform() noexcept;
	transform(const glm::mat4 _value) noexcept;
	virtual ~transform() = default;
	// Copy, takes lvalue
	transform(transform const &_other) noexcept;
	transform &operator=(transform const &_other) noexcept;
	// Move, take rvalue
	transform(transform&& _other) noexcept;
	transform &operator=(transform&& _other) noexcept;

	virtual void setTransform(const glm::mat4 *_value) noexcept;
	virtual void setPosition(const glm::vec3 _value) noexcept;
	virtual void translate(const glm::vec3 _value) noexcept;
	virtual void setAxes(
		const glm::vec3 _right,
		const glm::vec3 _up,
		const glm::vec3 _forward
	) noexcept;
	virtual void setRight(const glm::vec3 _right) noexcept;
	virtual void setUp(const glm::vec3 _up) noexcept;
	virtual void setForward(const glm::vec3 _forward) noexcept;
	virtual void setScale(const glm::vec3 _value) noexcept;

	_NODISCARD glm::mat4 getTransform() const noexcept;
	_NODISCARD glm::vec4 getPosition() const noexcept;
	_NODISCARD glm::vec4 getRight() const noexcept;
	_NODISCARD glm::vec4 getUp() const noexcept;
	_NODISCARD glm::vec4 getForward() const noexcept;
};
}

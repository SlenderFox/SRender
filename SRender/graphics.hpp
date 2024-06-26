#pragma once
#include <string>
#include "light.hpp"
#include "model.hpp"
#include "camera.hpp"

#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif

namespace srender
{
/** Graphics is responsible for all things visible in the virtual world. */
namespace graphics
{
	enum class mode: uint8_t
	{
		point,
		line,
		fill
	};

	bool init(const float _aspect) noexcept;
	void draw();

	void terminate() noexcept;

	void loadLightsIntoShader(const shader *_shader) noexcept;
	void updateAllShaders() noexcept;
	/** Modifies either the angle or blur of all spotlights by a value.
	 * @note Max value is 90 for angle and 1 for blur, min for both is 0.
	 * @param _isAngle True to modify the angle, false to modify the blur of the spotlight.
	 * @param _value The amount to modify it by.
	 */
	void modifyAllSpotlights(
		const bool _isAngle,
		const float _value
	);

	void addNewModel(model *_model);
	void addNewLight(light *_light);

	void setClearColour(const colour _colour) noexcept;
	void setRenderMode(const mode _mode = mode::fill) noexcept;
	void setRenderDepthBuffer(const bool _state) noexcept;

	_NODISCARD uint8_t modelCount() noexcept;
	_NODISCARD uint8_t lightCount() noexcept;
	_NODISCARD model *getModelAt(const uint8_t _pos);
	_NODISCARD light *getLightAt(const uint8_t _pos);
	_NODISCARD camera *getCamera() noexcept;

	_NODISCARD constexpr float getAmbience() { return 0.15f; }
}
}

#pragma once
#include "application.hpp"

/** Example project */
class Project: public srender::application
{
	static double s_camYaw, s_camPitch;

	/** Static function to use as a callback for mouse input
	 * @param inDeltaX The horizontal position change since last call
	 * @param inDeltaY The vertical position change since last call
	 */
	static void MouseCallback(double inDeltaX, double inDeltaY) noexcept;

	/** Static function to use as a callback for scroll input
	 * @param inOffsetX The horizontal position change since last call
	 * @param inOffsetY The vertical position change since last call
	 */
	static void ScrollCallback(double inOffsetX, double inOffsetY) noexcept;

	static constexpr
	uint8_t s_numCubes = 9U;

	static const
	glm::vec3 s_cubePositions[s_numCubes];

	std::vector<srender::light*> m_lightRefs;
	std::vector<srender::entity*> m_cubes;
	srender::entity *object_backpack;

	/** Loads entities in to create the scene */
	void CreateScene();
	/** Loads lights in to light the scene */
	void CreateLights();

	/** Crappy way to check input each frame */
	void processInput() noexcept;

public:
	Project();
	~Project();

	/** Called once at the start of runtime
	 * @return [bool] False will terminate application
	 */
	bool startup() override;
	/** Called when the application shuts down */
	void shutdown() override;
	/** Called once at the start of every frame */
	void update() override;
	/** Called 60 times per second, after Update */
	void fixedUpdate() override;
	/** Called once per frame, after fixedUpdate but still before rendering */
	void fateUpdate() override;
};

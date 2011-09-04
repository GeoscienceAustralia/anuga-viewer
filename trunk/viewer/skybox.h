#ifndef SKYBOX_H_
#define SKYBOX_H_

/**
 * Create a skybox.
 *
 * @param radius Radius of the skybox in world units.
 * @param filename Name of the sky texture (sphere-mapped)
 * @return a transform with skybox geometry attached.
 */
osg::Transform* Skybox_Create(float radius, const std::string filename);

#endif // SKYBOX_H_
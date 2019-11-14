#ifndef CAMERA_GLSL
#define CAMERA_GLSL

#include "ray.glsl"

/**
 * This is the representation of a camera.
 */
struct Camera {
	/**
	 * This is the observing point.
	 */
	vec3 lookFrom;

	/**
	 * This is the observed normalized direction.
	 */
	vec3 lookAt;

	/**
	 * This is the up vector.
	 */
	vec3 up;

	/**
	 * This is the field of view angle in degrees.
	 */
	float fieldOfView;

	/**
	 * This is the aspect ratio: width:height.
	 */
	float aspect;
};

/**
 * Generate a camera ray given its UV coordinates on screen.
 *
 * @param cam the camera
 * @param s the U coordinate
 * @param t the V ccordinate
 * @return the camera ray (in world coordinates)
 */
Ray generateCameraRayFromCamera(const in Camera cam, const in float s, const in float t) {
	vec3 u, v, w;
	const float theta = cam.fieldOfView * PI / 180;
	const float half_height = tan(theta / 2.0);
	const float half_width = cam.aspect * half_height;
	w = normalize(cam.lookFrom - cam.lookAt);
	u = normalize(cross(cam.up, w));
	v = cross(w, u);
	const vec3 mLowerLeftCorner = cam.lookFrom - half_width * u - half_height * v - w;
	const vec3 mHorizontal = 2.0 * (u * half_width);
	const vec3 mVertical = 2.0 * (v * half_height);

	return Ray(vec4(cam.lookFrom, 1), vec4(normalize(mLowerLeftCorner + s * mHorizontal + t * mVertical - cam.lookFrom), 0));
}

Ray generateCameraRayFromScreenCoord(const vec3 lookFrom, const vec3 lowerLeftCorner, const vec3 horizontal, const vec3 vertical, float s, float t) {
	return Ray(vec4(lookFrom, 1), vec4(normalize(lowerLeftCorner + s * horizontal + t * vertical - lookFrom), 0));
}

#endif // CAMERA_GLSL

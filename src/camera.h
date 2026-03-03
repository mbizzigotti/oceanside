#pragma once
#include "core.h"

struct cJSON;

struct Camera {
    enum class Type {
        Viewer,
        First_Person,
    };

    struct Viewer {
        float distance; // Distance of the camera eye position to the origin (meters)
        float azimuth; // Rotation of the camera eye position around the Y axis (degrees)
        float incline; // Angle of the camera eye position over the XZ plane (degrees)
    };

    struct First_Person {
        vec3 position;
        float azimuth; // Rotation of the camera eye position around the Y axis (degrees)
        float incline; // Angle of the camera eye position over the XZ plane (degrees)
        bool is_enabled;
    };

    Type type;
    
    Viewer       viewer;
    First_Person first_person;

    u32 width;
    u32 height;
    float far;  // far clipping plane
    float near; // near clipping plane
    float fov;

    static Camera get_default();
    
    bool handle_event(RGFW_event &event);
    void update(float dt);
    void load(cJSON *object);
    mat4 get_transform(vec3 *view_position = 0);
};

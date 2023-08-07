
#ifndef _RAYTRACER_H
#define _RAYTRACER_H

#include <memory>

#include "scene_parser.h"
#include "commandLineArgumentParser.h"
#include "image.h"
#include "camera.h"
#include "group.h"
#include "hit.h"
#include "material.h"
#include <vector>

class RayTracer
{
    // 场景信息的指针
    std::shared_ptr<SceneParser> sceneParser;
    // 命令信息的指针
    std::shared_ptr<CommandLineArgumentParser> cmdlParser;

    // 场景信息
    Vec3f bgColor;
    Group *group;
    unsigned long numLights;
    vector<Light *> lights;
    Vec3f ambient;

    Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const;
    bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
                              float index_i, float index_t, Vec3f &transmitted) const;

public:
    RayTracer(const std::shared_ptr<SceneParser> &_sceneParser, const shared_ptr<CommandLineArgumentParser> &_cmdlParser);

    std::shared_ptr<SceneParser> getSceneParser() { return sceneParser; }

    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const;
};

#endif

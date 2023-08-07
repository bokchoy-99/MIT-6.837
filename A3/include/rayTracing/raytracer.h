
#ifndef _RAYTRACER_H
#define _RAYTRACER_H

#include <memory>

#include "scene_parser.h"
#include "image.h"
#include "camera.h"
#include "group.h"
#include "hit.h"
#include "material.h"

class RayTracer
{
    std::shared_ptr<SceneParser> _sceneParser;

public:
    std::shared_ptr<SceneParser> getSceneParser() { return _sceneParser; }
    void readSceneFromFile(char *input_file);
    void renderToImage(Image &img, bool shade_back = false);
    void renderDepthToImage(Image &img, float depMin, float depMax);
    void renderNormalToImage(Image &img);
};

#endif

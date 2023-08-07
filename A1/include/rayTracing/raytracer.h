
#ifndef _RAYTRACER_H
#define _RAYTRACER_H

#include <memory>

#include "scene_parser.h"
#include "image.h"
#include "camera.h"
#include "group.h"
#include "hit.h"
#include "material.h"

class RayTracer {
    std::shared_ptr<SceneParser> sceneParser;

public:
    void readSceneFromFile(char *input_file);
    void renderToImage(Image &img);
    void renderDepthToImage(Image &img, float depMin, float depMax);
};

#endif

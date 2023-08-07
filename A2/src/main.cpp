#include "commandLineArgumentParser.h"
#include "image.h"
#include "raytracer.h"

int main(int argc, char* argv[]) {
    auto cmdlParser = CommandLineArgumentParser();
    cmdlParser.parse(argc, argv);

    auto img = Image(cmdlParser.width, cmdlParser.height);
    RayTracer rayTracer;
    rayTracer.readSceneFromFile(cmdlParser.input_file);
    rayTracer.renderToImage(img, cmdlParser.shade_back);
    img.SaveTGA(cmdlParser.output_file);

    if (cmdlParser.depth_file != nullptr) {
        rayTracer.renderDepthToImage(img, cmdlParser.depth_min, cmdlParser.depth_max);
        img.SaveTGA(cmdlParser.depth_file);
    }

    if (cmdlParser.normal_file != nullptr) {
        rayTracer.renderNormalToImage(img);
        img.SaveTGA(cmdlParser.normal_file);
    }
}

// main -input ../assets/scene2_01_diffuse.txt -size 200 200 -output output2_01.tga
// main -input ../assets/scene2_02_ambient.txt -size 200 200 -output output2_02.tga

// main -input ../assets/scene2_03_colored_lights.txt -size 200 200 -output output2_03.tga -normals normals2_03.tga

// main -input ../assets/scene2_04_perspective.txt -size 200 200 -output output2_04.tga -normals normals2_04.tga

// 球中包含球，测试背面渲染
// main -input ../assets/scene2_05_inside_sphere.txt -size 200 200 -output output2_05.tga -depth 9 11 depth2_05.tga -normals normals2_05.tga -shade_back
// main -input ../assets/scene2_05_inside_sphere.txt -size 200 200 -output output2_05_no_back.tga

// 球体 + 平面
// main -input ../assets/scene2_06_plane.txt -size 200 200 -output output2_06.tga -depth 8 20 depth2_06.tga -normals normals2_06.tga

// 球体 + 三角形，这种平面的法向量会影响到渲染，光线和法向量有时会同向有时反向，（法向量表示哪个面有颜色吗？）
// main -input ../assets/scene2_07_sphere_triangles.txt -size 200 200 -output output2_07.tga -depth 9 11 depth2_07.tga -normals normals2_07.tga -shade_back
// main -input ../assets/scene2_07_sphere_triangles.txt -size 200 200 -output output2_07_no_back.tga

// 三角网格对象，有三角形拼成的物体
// main -input ../assets/scene2_08_cube.txt -size 200 200 -output output2_08.tga
// main -input ../assets/scene2_09_bunny_200.txt -size 200 200 -output output2_09.tga
// main -input ../assets/scene2_10_bunny_1k.txt -size 200 200 -output output2_10.tga

// 对球体缩放，y方向（即hor方向）缩小倍
// main -input ../assets/scene2_11_squashed_sphere.txt -size 200 200 -output output2_11.tga -normals normals2_11.tga

// 旋转球体（看不出区别，就z轴转一转）
// main -input ../assets/scene2_12_rotated_sphere.txt -size 200 200 -output output2_12.tga -normals normals2_12.tga

// 先缩放再旋转的
// main -input ../assets/scene2_13_rotated_squashed_sphere.txt -size 200 200 -output output2_13.tga -normals normals2_13.tga

// 对cube进行平移、缩放、旋转
// main -input ../assets/scene2_14_axes_cube.txt -size 200 200 -output output2_14.tga
// 缩放与旋转的先后顺序造成最终结果不同
// main -input ../assets/scene2_15_crazy_transforms.txt -size 200 200 -output output2_15.tga

// 平移+旋转
// main -input ../assets/scene2_16_t_scale.txt -size 200 200 -output output2_16.tga -depth 2 7 depth2_16.tga

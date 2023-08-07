#ifndef _COMMANDLINEARGUMENTPARSER_H
#define _COMMANDLINEARGUMENTPARSER_H

class CommandLineArgumentParser
{
public:
    char *input_file = nullptr;
    int width = 100;
    int height = 100;
    char *output_file = nullptr;
    float depth_min = 0;
    float depth_max = 1;
    char *depth_file = nullptr;
    char *normal_file = nullptr;
    bool shade_back = false;
    bool use_gui = false;
    bool cast_shadow = false;
    int max_bounces = 0;
    float cutoff_weight = 0;
    bool visualize_grid = false;
    int grid_nx = 0, grid_ny = 0, grid_nz = 0;
    bool stats = false;
    int samples_num = 0;
    char *sampler_strategy = nullptr;
    char *samples_file = nullptr;
    int samples_zoom = 0;
    char *filter_file = nullptr;
    int filter_zoom = 0;
    char *filter_strategy = nullptr;
    float filter_argument = 0;

    void parse(int argc, char *argv[]);
};

#endif // A_COMMANDLINEARGUMENTPARSER_H
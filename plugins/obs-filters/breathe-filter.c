#include <obs-module.h>
#include <graphics/vec2.h>

struct breathe_filter_data {
    obs_source_t *context;

    gs_effect_t *effect;
    gs_eparam_t *param_add;
    gs_eparam_t *param_mul;
    gs_eparam_t *param_image;
    gs_eparam_t *param_multiplier;

    struct vec2 scroll_speed; // 保留字段，当前未使用
    gs_samplerstate_t *sampler;
    bool limit_cx;
    bool limit_cy;
    uint32_t cx;
    uint32_t cy;

    struct vec2 size_i;
    struct vec2 offset;

    bool loop;

    // 缩放相关字段
    int shrink_pixels;      // 缩小的像素数（例如 20 表示缩小 20 像素）
    int expand_pixels;      // 放大的像素数（例如 20 表示放大 20 像素）
    float scale_speed;      // 缩放速度（像素/秒）
    float current_scale;    // 当前缩放比例（仍然使用比例，但内部计算）
    float target_scale;     // 目标缩放比例（仍然使用比例，但内部计算）
    int scale_state;        // 当前状态：0=缩小，1=回到原大小，2=放大，3=回到原大小
    float scale_time;       // 累计时间，用于控制速度
    bool enable;            // 是否启用滤镜
};

static const char *breathe_filter_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("BreatheFilter");
}

static void *breathe_filter_create(obs_data_t *settings, obs_source_t *context)
{
    struct breathe_filter_data *filter = bzalloc(sizeof(*filter));
    char *effect_path = obs_module_file("crop_filter.effect");

    filter->context = context;

    obs_enter_graphics();
    filter->effect = gs_effect_create_from_file(effect_path, NULL);
    obs_leave_graphics();

    bfree(effect_path);

    if (!filter->effect) {
        bfree(filter);
        return NULL;
    }

    filter->param_add = gs_effect_get_param_by_name(filter->effect, "add_val");
    filter->param_mul = gs_effect_get_param_by_name(filter->effect, "mul_val");
    filter->param_image = gs_effect_get_param_by_name(filter->effect, "image");
    filter->param_multiplier = gs_effect_get_param_by_name(filter->effect, "multiplier");

    obs_source_update(context, settings);
    return filter;
}

static void breathe_filter_destroy(void *data)
{
    struct breathe_filter_data *filter = data;

    obs_enter_graphics();
    gs_effect_destroy(filter->effect);
    gs_samplerstate_destroy(filter->sampler);
    obs_leave_graphics();

    bfree(filter);
}

static void breathe_filter_update(void *data, obs_data_t *settings)
{
    struct breathe_filter_data *filter = data;
    bool was_enabled = filter->enable;

    filter->limit_cx = obs_data_get_bool(settings, "limit_cx");
    filter->limit_cy = obs_data_get_bool(settings, "limit_cy");
    filter->cx = (uint32_t)obs_data_get_int(settings, "cx");
    filter->cy = (uint32_t)obs_data_get_int(settings, "cy");

    filter->shrink_pixels = (int)obs_data_get_int(settings, "shrink_pixels");
    filter->expand_pixels = (int)obs_data_get_int(settings, "expand_pixels");
    filter->scale_speed = (float)obs_data_get_double(settings, "scale_speed");
    filter->enable = obs_data_get_bool(settings, "enable");

    filter->loop = obs_data_get_bool(settings, "loop");

    struct gs_sampler_info sampler_info = {
        .filter = GS_FILTER_LINEAR,
        .address_u = filter->loop ? GS_ADDRESS_WRAP : GS_ADDRESS_BORDER,
        .address_v = filter->loop ? GS_ADDRESS_WRAP : GS_ADDRESS_BORDER,
    };

    obs_enter_graphics();
    gs_samplerstate_destroy(filter->sampler);
    filter->sampler = gs_samplerstate_create(&sampler_info);
    obs_leave_graphics();

    // 只在首次启用或禁用时重置状态，避免设置其他参数时干扰当前动画
    if (was_enabled != filter->enable) {
        // 初始化缩放状态
        if (filter->enable) {
            // 如果是首次启用，从当前大小开始渐变
            filter->current_scale = 1.0f;     // 从原始大小开始
            filter->scale_state = 0;          // 从"缩小"开始
            filter->scale_time = 0.0f;
            filter->target_scale = 0.0f;      // 目标将在 tick 函数中计算
        } else {
            // 如果禁用，立即重置为原始大小
            filter->current_scale = 1.0f;
            filter->target_scale = 1.0f;
            filter->scale_state = 0;
            filter->scale_time = 0.0f;
        }
    }
}

static bool limit_cx_clicked(obs_properties_t *props, obs_property_t *p, obs_data_t *settings)
{
    bool limit_size = obs_data_get_bool(settings, "limit_cx");
    obs_property_set_visible(obs_properties_get(props, "cx"), limit_size);

    UNUSED_PARAMETER(p);
    return true;
}

static bool limit_cy_clicked(obs_properties_t *props, obs_property_t *p, obs_data_t *settings)
{
    bool limit_size = obs_data_get_bool(settings, "limit_cy");
    obs_property_set_visible(obs_properties_get(props, "cy"), limit_size);

    UNUSED_PARAMETER(p);
    return true;
}

static obs_properties_t *breathe_filter_properties(void *data)
{
    obs_properties_t *props = obs_properties_create();
    obs_property_t *p;

    obs_properties_add_bool(props, "enable", obs_module_text("BreatheFilter.Enable"));
    
    p = obs_properties_add_int_slider(props, "shrink_pixels", obs_module_text("BreatheFilter.ShrinkPixels"), 0, 150, 1);
    obs_property_set_long_description(p, "设置画面缩小的像素数量（基于宽度），高度将保持等比例缩放");
    
    p = obs_properties_add_int_slider(props, "expand_pixels", obs_module_text("BreatheFilter.ExpandPixels"), 0, 150, 1);
    obs_property_set_long_description(p, "设置画面放大的像素数量（基于宽度），高度将保持等比例缩放");
    
    p = obs_properties_add_float_slider(props, "scale_speed", obs_module_text("BreatheFilter.ScaleSpeed"), 5.0, 150.0, 1.0);
    obs_property_set_long_description(p, "设置缩放速度（像素/秒），值越大，呼吸效果越快");

    p = obs_properties_add_bool(props, "limit_cx", obs_module_text("BreatheFilter.LimitWidth"));
    obs_property_set_modified_callback(p, limit_cx_clicked);
    obs_properties_add_int(props, "cx", obs_module_text("Crop.Width"), 1, 8192, 1);

    p = obs_properties_add_bool(props, "limit_cy", obs_module_text("BreatheFilter.LimitHeight"));
    obs_property_set_modified_callback(p, limit_cy_clicked);
    obs_properties_add_int(props, "cy", obs_module_text("Crop.Height"), 1, 8192, 1);

    obs_properties_add_bool(props, "loop", obs_module_text("BreatheFilter.Loop"));

    UNUSED_PARAMETER(data);
    return props;
}

static void breathe_filter_defaults(obs_data_t *settings)
{
    obs_data_set_default_bool(settings, "limit_size", false);
    obs_data_set_default_int(settings, "cx", 100);
    obs_data_set_default_int(settings, "cy", 100);
    obs_data_set_default_bool(settings, "loop", true);
    obs_data_set_default_bool(settings, "enable", false);    // 默认关闭滤镜
    obs_data_set_default_int(settings, "shrink_pixels", 10); // 默认缩小 10 像素，减小缩放范围
    obs_data_set_default_int(settings, "expand_pixels", 10); // 默认放大 10 像素，减小缩放范围
    obs_data_set_default_double(settings, "scale_speed", 15.0); // 默认速度降低为 15 像素/秒，使动画更平滑
}

static void breathe_filter_tick(void *data, float seconds)
{
    struct breathe_filter_data *filter = data;

    // 如果滤镜未启用，保持原始大小并重置状态
    if (!filter->enable) {
        filter->current_scale = 1.0f;
        filter->target_scale = 1.0f;
        filter->scale_state = 0;  // 重置为初始状态
        filter->scale_time = 0.0f;
        return;
    }

    // 获取源的基本宽度和高度
    obs_source_t *target = obs_filter_get_target(filter->context);
    uint32_t base_cx = obs_source_get_base_width(target);
    uint32_t base_cy = obs_source_get_base_height(target);
    
    if (base_cx == 0 || base_cy == 0)
        return;

    // 始终使用宽度作为缩放基准
    uint32_t reference_dimension = base_cx;
    
    // 根据当前的状态计算目标比例，确保平滑过渡
    if (filter->target_scale == 0.0f || (filter->scale_state == 0 && filter->target_scale == 1.0f) || 
        (filter->scale_state == 2 && filter->target_scale == 1.0f)) {
        switch (filter->scale_state) {
        case 0: // 缩小
            // 计算缩小的目标比例（从1.0减去相对比例）
            if (filter->shrink_pixels > 0) {
                filter->target_scale = 1.0f - (float)filter->shrink_pixels / (float)reference_dimension;
                // 确保不会缩小到零或负值
                filter->target_scale = fmaxf(0.1f, filter->target_scale);
            } else {
                // 如果缩小像素设为0，直接跳到状态2（放大）
                filter->scale_state = 2;
                filter->target_scale = 1.0f + (float)filter->expand_pixels / (float)reference_dimension;
            }
            break;
        case 1: // 回到原大小
            filter->target_scale = 1.0f;
            break;
        case 2: // 放大
            // 计算放大的目标比例（从1.0加上相对比例）
            if (filter->expand_pixels > 0) {
                filter->target_scale = 1.0f + (float)filter->expand_pixels / (float)reference_dimension;
            } else {
                // 如果放大像素设为0，直接跳到状态0（缩小）
                filter->scale_state = 0;
                filter->target_scale = 1.0f - (float)filter->shrink_pixels / (float)reference_dimension;
                filter->target_scale = fmaxf(0.1f, filter->target_scale);
            }
            break;
        case 3: // 回到原大小
            filter->target_scale = 1.0f;
            break;
        }
    }

    // 更新累计时间
    filter->scale_time += seconds;

    // 计算像素增量和对应的比例增量
    float pixel_delta = filter->scale_speed * seconds;
    float scale_delta = pixel_delta / (float)reference_dimension; // 使用宽度计算缩放增量

    // 更新当前缩放比例，确保平滑过渡
    if (filter->current_scale > filter->target_scale) {
        filter->current_scale -= scale_delta;
        if (filter->current_scale < filter->target_scale)
            filter->current_scale = filter->target_scale;
    } else if (filter->current_scale < filter->target_scale) {
        filter->current_scale += scale_delta;
        if (filter->current_scale > filter->target_scale)
            filter->current_scale = filter->target_scale;
    }

    // 根据状态切换逻辑
    if (fabs(filter->current_scale - filter->target_scale) < 0.001f) {
        switch (filter->scale_state) {
        case 0: // 缩小到目标像素
            filter->scale_state = 1;           // 切换到返回状态
            filter->target_scale = 1.0f;       // 目标回到原大小
            break;
        case 1: // 返回到原大小
            filter->scale_state = 2;           // 切换到放大状态
            filter->target_scale = 0.0f;       // 临时值，将在下一帧重新计算
            break;
        case 2: // 放大到目标像素
            filter->scale_state = 3;           // 切换到返回状态
            filter->target_scale = 1.0f;       // 目标回到原大小
            break;
        case 3: // 返回到原大小
            filter->scale_state = 0;           // 切换到缩小状态
            filter->target_scale = 0.0f;       // 临时值，将在下一帧重新计算
            break;
        }
    }
}

static const char *get_tech_name_and_multiplier(enum gs_color_space current_space, enum gs_color_space source_space,
                        float *multiplier)
{
    const char *tech_name = "Draw";
    *multiplier = 1.f;

    switch (source_space) {
    case GS_CS_SRGB:
    case GS_CS_SRGB_16F:
        if (current_space == GS_CS_709_SCRGB) {
            tech_name = "DrawMultiply";
            *multiplier = obs_get_video_sdr_white_level() / 80.0f;
        }
        break;
    case GS_CS_709_EXTENDED:
        switch (current_space) {
        case GS_CS_SRGB:
        case GS_CS_SRGB_16F:
            tech_name = "DrawTonemap";
            break;
        case GS_CS_709_SCRGB:
            tech_name = "DrawMultiply";
            *multiplier = obs_get_video_sdr_white_level() / 80.0f;
            break;
        case GS_CS_709_EXTENDED:
            break;
        }
        break;
    case GS_CS_709_SCRGB:
        switch (current_space) {
        case GS_CS_SRGB:
        case GS_CS_SRGB_16F:
            tech_name = "DrawMultiplyTonemap";
            *multiplier = 80.0f / obs_get_video_sdr_white_level();
            break;
        case GS_CS_709_EXTENDED:
            tech_name = "DrawMultiply";
            *multiplier = 80.0f / obs_get_video_sdr_white_level();
            break;
        case GS_CS_709_SCRGB:
            break;
        }
    }

    return tech_name;
}

static void breathe_filter_render(void *data, gs_effect_t *effect)
{
    UNUSED_PARAMETER(effect);

    struct breathe_filter_data *filter = data;
    struct vec2 mul_val;
    uint32_t base_cx;
    uint32_t base_cy;
    uint32_t cx;
    uint32_t cy;

    obs_source_t *target = obs_filter_get_target(filter->context);
    base_cx = obs_source_get_base_width(target);
    base_cy = obs_source_get_base_height(target);

    cx = filter->limit_cx ? filter->cx : base_cx;
    cy = filter->limit_cy ? filter->cy : base_cy;

    if (base_cx && base_cy) {
        vec2_set(&filter->size_i, 1.0f / (float)base_cx, 1.0f / (float)base_cy);
    } else {
        vec2_zero(&filter->size_i);
        obs_source_skip_video_filter(filter->context);
        return;
    }

    // 应用缩放因子
    float scale_factor = filter->current_scale;
    vec2_set(&mul_val, (float)cx / (float)base_cx * scale_factor, (float)cy / (float)base_cy * scale_factor);

    const enum gs_color_space preferred_spaces[] = {
        GS_CS_SRGB,
        GS_CS_SRGB_16F,
        GS_CS_709_EXTENDED,
    };

    const enum gs_color_space source_space = obs_source_get_color_space(
        obs_filter_get_target(filter->context), OBS_COUNTOF(preferred_spaces), preferred_spaces);
    float multiplier;
    const char *technique = get_tech_name_and_multiplier(gs_get_color_space(), source_space, &multiplier);
    const enum gs_color_format format = gs_get_format_from_space(source_space);
    if (obs_source_process_filter_begin_with_color_space(filter->context, format, source_space,
                                                         OBS_NO_DIRECT_RENDERING)) {
        gs_effect_set_vec2(filter->param_add, &filter->offset);
        gs_effect_set_vec2(filter->param_mul, &mul_val);
        gs_effect_set_float(filter->param_multiplier, multiplier);

        gs_effect_set_next_sampler(filter->param_image, filter->sampler);

        gs_blend_state_push();
        gs_blend_function(GS_BLEND_ONE, GS_BLEND_INVSRCALPHA);

        obs_source_process_filter_tech_end(filter->context, filter->effect, cx, cy, technique);

        gs_blend_state_pop();
    }
}

static uint32_t breathe_filter_width(void *data)
{
    struct breathe_filter_data *filter = data;
    obs_source_t *target = obs_filter_get_target(filter->context);

    return filter->limit_cx ? filter->cx : obs_source_get_base_width(target);
}

static uint32_t breathe_filter_height(void *data)
{
    struct breathe_filter_data *filter = data;
    obs_source_t *target = obs_filter_get_target(filter->context);

    return filter->limit_cy ? filter->cy : obs_source_get_base_height(target);
}

static void breathe_filter_show(void *data)
{
    struct breathe_filter_data *filter = data;
    filter->offset.x = 0.0f;
    filter->offset.y = 0.0f;
    
    // 只有当滤镜被启用时才需要从头开始动画
    if (filter->enable) {
        // 总是从原始大小开始，确保平滑过渡
        filter->current_scale = 1.0f;     // 初始为原始大小
        filter->scale_state = 0;          // 从"缩小"开始
        filter->scale_time = 0.0f;
        filter->target_scale = 0.0f;      // 在 tick 函数中计算
    }
}

static enum gs_color_space breathe_filter_get_color_space(void *data, size_t count,
                              const enum gs_color_space *preferred_spaces)
{
    const enum gs_color_space potential_spaces[] = {
        GS_CS_SRGB,
        GS_CS_SRGB_16F,
        GS_CS_709_EXTENDED,
    };

    struct breathe_filter_data *const filter = data;
    const enum gs_color_space source_space = obs_source_get_color_space(
        obs_filter_get_target(filter->context), OBS_COUNTOF(potential_spaces), potential_spaces);

    enum gs_color_space space = source_space;
    for (size_t i = 0; i < count; ++i) {
        space = preferred_spaces[i];
        if (space == source_space)
            break;
    }

    return space;
}

struct obs_source_info breathe_filter = {
    .id = "breathe_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_SRGB,
    .get_name = breathe_filter_get_name,
    .create = breathe_filter_create,
    .destroy = breathe_filter_destroy,
    .update = breathe_filter_update,
    .get_properties = breathe_filter_properties,
    .get_defaults = breathe_filter_defaults,
    .video_tick = breathe_filter_tick,
    .video_render = breathe_filter_render,
    .get_width = breathe_filter_width,
    .get_height = breathe_filter_height,
    .show = breathe_filter_show,
    .video_get_color_space = breathe_filter_get_color_space,
};
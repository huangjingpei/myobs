#include <obs-module.h>
#include <graphics/vec2.h>

struct scroll_filter_data {
	obs_source_t *context;

	gs_effect_t *effect;
	gs_eparam_t *param_add;
	gs_eparam_t *param_mul;
	gs_eparam_t *param_image;
	gs_eparam_t *param_multiplier;

	struct vec2 scroll_speed; // 当前速度（动态调整）
	gs_samplerstate_t *sampler;
	bool limit_cx;
	bool limit_cy;
	uint32_t cx;
	uint32_t cy;

	struct vec2 size_i;
	struct vec2 offset;

	bool loop;

	// 新增字段
	float left_pixels;      // 向左平移的像素数
	float right_pixels;     // 向右平移的像素数
	float speed;            // 平移速度（像素/秒）
	float target_offset_x;  // 目标偏移量
	int state;              // 当前状态：0=向左，1=返回，2=向右
	float accumulated_time; // 累计时间，用于控制速度
	bool enable_scroll;     // 新增：是否启用平移开关，默认关闭
};

static const char *scroll_filter_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("ScrollFilter");
}

static void *scroll_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct scroll_filter_data *filter = bzalloc(sizeof(*filter));
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

static void scroll_filter_destroy(void *data)
{
	struct scroll_filter_data *filter = data;

	obs_enter_graphics();
	gs_effect_destroy(filter->effect);
	gs_samplerstate_destroy(filter->sampler);
	obs_leave_graphics();

	bfree(filter);
}

static void scroll_filter_update(void *data, obs_data_t *settings)
{
	struct scroll_filter_data *filter = data;

	filter->limit_cx = obs_data_get_bool(settings, "limit_cx");
	filter->limit_cy = obs_data_get_bool(settings, "limit_cy");
	filter->cx = (uint32_t)obs_data_get_int(settings, "cx");
	filter->cy = (uint32_t)obs_data_get_int(settings, "cy");

	filter->left_pixels = (float)obs_data_get_double(settings, "left_pixels");
	filter->right_pixels = (float)obs_data_get_double(settings, "right_pixels");
	filter->speed = (float)obs_data_get_double(settings, "speed");

	filter->loop = obs_data_get_bool(settings, "loop");
	filter->enable_scroll = obs_data_get_bool(settings, "enable_scroll"); // 获取开关状态

	struct gs_sampler_info sampler_info = {
		.filter = GS_FILTER_LINEAR,
		.address_u = filter->loop ? GS_ADDRESS_WRAP : GS_ADDRESS_BORDER,
		.address_v = filter->loop ? GS_ADDRESS_WRAP : GS_ADDRESS_BORDER,
	};

	obs_enter_graphics();
	gs_samplerstate_destroy(filter->sampler);
	filter->sampler = gs_samplerstate_create(&sampler_info);
	obs_leave_graphics();

	// 初始化状态（仅在启用时初始化）
	if (filter->enable_scroll) {
		filter->state = 0;       // 从“向左”开始
		filter->offset.x = 0.0f; // 初始位置
		filter->offset.y = 0.0f;
		filter->accumulated_time = 0.0f;
		filter->target_offset_x = -filter->left_pixels * filter->size_i.x; // 向左目标偏移
		filter->scroll_speed.x = -filter->speed;                           // 初始速度（向左）
	} else {
		filter->state = -1;      // 禁用状态
		filter->offset.x = 0.0f; // 停止移动
		filter->scroll_speed.x = 0.0f;
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

static obs_properties_t *scroll_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();
	obs_property_t *p;

	// 新增平移距离和速度参数
	obs_properties_add_float_slider(props, "left_pixels", obs_module_text("ScrollFilter.LeftPixels"), 0.0, 1000.0,
					1.0);
	obs_properties_add_float_slider(props, "right_pixels", obs_module_text("ScrollFilter.RightPixels"), 0.0, 1000.0,
					1.0);
	obs_properties_add_float_slider(props, "speed", obs_module_text("ScrollFilter.Speed"), 10.0, 500.0, 1.0);

	// 添加开关
	p = obs_properties_add_bool(props, "enable_scroll", obs_module_text("ScrollFilter.EnableScroll"));
	obs_property_set_description(p, "Enable scrolling effect (default: off)");

	p = obs_properties_add_bool(props, "limit_cx", obs_module_text("ScrollFilter.LimitWidth"));
	obs_property_set_modified_callback(p, limit_cx_clicked);
	obs_properties_add_int(props, "cx", obs_module_text("Crop.Width"), 1, 8192, 1);

	p = obs_properties_add_bool(props, "limit_cy", obs_module_text("ScrollFilter.LimitHeight"));
	obs_property_set_modified_callback(p, limit_cy_clicked);
	obs_properties_add_int(props, "cy", obs_module_text("Crop.Height"), 1, 8192, 1);

	obs_properties_add_bool(props, "loop", obs_module_text("ScrollFilter.Loop"));

	UNUSED_PARAMETER(data);
	return props;
}

static void scroll_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_bool(settings, "limit_cx", false);
	obs_data_set_default_int(settings, "cx", 100);
	obs_data_set_default_bool(settings, "limit_cy", false);
	obs_data_set_default_int(settings, "cy", 100);
	obs_data_set_default_bool(settings, "loop", true);
	obs_data_set_default_double(settings, "left_pixels", 100.0); // 默认向左 100 像素
	obs_data_set_default_double(settings, "right_pixels", 25.0); // 默认向右 25 像素
	obs_data_set_default_double(settings, "speed", 200.0);       // 默认速度 200 像素/秒
	obs_data_set_default_bool(settings, "enable_scroll", false); // 默认关闭开关
}

static void scroll_filter_tick(void *data, float seconds)
{
	struct scroll_filter_data *filter = data;

	if (!filter->enable_scroll) {
		filter->offset.x = 0.0f; // 禁用时停止移动
		filter->scroll_speed.x = 0.0f;
		return; // 不执行平移逻辑
	}

	// 更新累计时间
	filter->accumulated_time += seconds;

	// 计算当前帧的偏移增量
	float delta_x = filter->size_i.x * filter->scroll_speed.x * seconds;
	filter->offset.x += delta_x;

	// 根据状态切换逻辑
	switch (filter->state) {
	case 0: // 向左平移
		if (filter->offset.x <= filter->target_offset_x) {
			filter->offset.x = filter->target_offset_x; // 到达目标
			filter->state = 1;                          // 切换到返回状态
			filter->scroll_speed.x = filter->speed;     // 设置返回速度（向右）
			filter->target_offset_x = 0.0f;             // 目标回到原位
		}
		break;

	case 1: // 返回原位
		if (filter->offset.x >= filter->target_offset_x) {
			filter->offset.x = 0.0f;                                           // 回到原位
			filter->state = 2;                                                 // 切换到向右状态
			filter->scroll_speed.x = filter->speed;                            // 设置向右速度
			filter->target_offset_x = filter->right_pixels * filter->size_i.x; // 向右目标偏移
		}
		break;

	case 2: // 向右平移
		if (filter->offset.x >= filter->target_offset_x) {
			filter->offset.x = filter->target_offset_x;                        // 到达目标
			filter->state = 0;                                                 // 切换到向左状态
			filter->scroll_speed.x = -filter->speed;                           // 设置向左速度
			filter->target_offset_x = -filter->left_pixels * filter->size_i.x; // 向左目标偏移
		}
		break;
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

static void scroll_filter_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);

	struct scroll_filter_data *filter = data;
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

	vec2_set(&mul_val, (float)cx / (float)base_cx, (float)cy / (float)base_cy);

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

static uint32_t scroll_filter_width(void *data)
{
	struct scroll_filter_data *filter = data;
	obs_source_t *target = obs_filter_get_target(filter->context);

	return filter->limit_cx ? filter->cx : obs_source_get_base_width(target);
}

static uint32_t scroll_filter_height(void *data)
{
	struct scroll_filter_data *filter = data;
	obs_source_t *target = obs_filter_get_target(filter->context);

	return filter->limit_cy ? filter->cy : obs_source_get_base_height(target);
}

static void scroll_filter_show(void *data)
{
	struct scroll_filter_data *filter = data;
	filter->offset.x = 0.0f;
	filter->offset.y = 0.0f;
	if (filter->enable_scroll) {
		filter->state = 0; // 从“向左”开始
		filter->accumulated_time = 0.0f;
		filter->scroll_speed.x = -filter->speed;                           // 初始速度
		filter->target_offset_x = -filter->left_pixels * filter->size_i.x; // 初始目标
	} else {
		filter->state = -1; // 禁用状态
		filter->scroll_speed.x = 0.0f;
	}
}

static enum gs_color_space scroll_filter_get_color_space(void *data, size_t count,
							 const enum gs_color_space *preferred_spaces)
{
	const enum gs_color_space potential_spaces[] = {
		GS_CS_SRGB,
		GS_CS_SRGB_16F,
		GS_CS_709_EXTENDED,
	};

	struct scroll_filter_data *const filter = data;
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

struct obs_source_info scroll_filter = {
	.id = "scroll_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_SRGB,
	.get_name = scroll_filter_get_name,
	.create = scroll_filter_create,
	.destroy = scroll_filter_destroy,
	.update = scroll_filter_update,
	.get_properties = scroll_filter_properties,
	.get_defaults = scroll_filter_defaults,
	.video_tick = scroll_filter_tick,
	.video_render = scroll_filter_render,
	.get_width = scroll_filter_width,
	.get_height = scroll_filter_height,
	.show = scroll_filter_show,
	.video_get_color_space = scroll_filter_get_color_space,
};

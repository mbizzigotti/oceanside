#pragma once
#include "../core.h"
#include "../graphics.h"

struct TextRenderer {
	static constexpr uint32_t MAX_QUAD_COUNT = (1 << 12);

	u32 vertex_offset;
	u32 index_offset;
	u32 index_start;
	u32 quad_count;
    u32 line_offset;

	std::vector<shader::TextVertex> vertices;

	void add_text(Color color, const char* format, ...);

	void setup(Graphics &gfx);
	void write_buffers(Graphics &gfx);
	void write_dynamic_buffers(Graphics &gfx);
	void render(VkCommandBuffer cmd, VkPipelineLayout layout);
};

global TextRenderer text;

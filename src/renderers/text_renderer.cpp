#include "text_renderer.h"
#include "../graphics.h"
#include "../3rdparty/stb_easy_font.h"

void TextRenderer::setup(Graphics &gfx) {
	vertex_offset = gfx.allocate_buffer(Graphics::VERTEX_BUFFER, MAX_QUAD_COUNT * 4 * sizeof(shader::TextVertex));
	index_offset = gfx.allocate_buffer(Graphics::INDEX_BUFFER, MAX_QUAD_COUNT * 6 * sizeof(uint32_t));
	index_start = index_offset / sizeof(u32);

	vertices.resize(MAX_QUAD_COUNT * 4);
}

void TextRenderer::add_text(Color color, const char *format, ...) {
	char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

	uint32_t color_value = (uint32_t)(color);
	u8 color_bytes[4] = {
		u8(color_value >> 24),
		u8(color_value >> 16),
		u8(color_value >>  8),
		u8(color_value >>  0),
	};
	shader::TextVertex* start = vertices.data() + quad_count * 4;
	int remaining = vertices.size() - quad_count * 4;
	quad_count += stb_easy_font_print(4, 4 + line_offset, (char*)buffer, color_bytes, start, remaining);
	line_offset += 12; // Font height is 12 "pixels"
}

void TextRenderer::write_buffers(Graphics &gfx) {
	std::vector<u32> indices;
	for (uint32_t i = 0; i < MAX_QUAD_COUNT; ++i) {
		indices.emplace_back(i*4 + 0);
		indices.emplace_back(i*4 + 1);
		indices.emplace_back(i*4 + 2);
		indices.emplace_back(i*4 + 2);
		indices.emplace_back(i*4 + 3);
		indices.emplace_back(i*4 + 0);
	}
	gfx.write_index_buffer(index_offset, indices.data(), indices.size() * sizeof(u32));
}

void TextRenderer::write_dynamic_buffers(Graphics &gfx) {
	u64 size = quad_count * 4 * sizeof(shader::TextVertex);
	gfx.write_vertex_buffer(vertex_offset, vertices.data(), size);
	gfx.flush_partition(Graphics::VERTEX_BUFFER, vertex_offset, size);
}

void TextRenderer::render(VkCommandBuffer cmd, VkPipelineLayout layout) {
	shader::PerDraw draw = {
		.vertex_offset = vertex_offset,
		.shader_id = shader::Shader::Text,
		.material_id = shader::Material::Text,
	};
	vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(draw), &draw);
	vkCmdDrawIndexed(cmd, quad_count * 6, 1, index_start, 0, 0);

	quad_count = 0;
	line_offset = 0;
}

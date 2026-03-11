#include "skybox_loader.h"
#include "tokenizer.h"
#include "../object.h"
#include "../3rdparty/stb_image.h"
#include "../3rdparty/cJSON.h"
#include "../graphics.h"

struct CubemapParser {
    AutoFree<string> contents;
    string images[6];

    Result parse(string filename, string contents);
};

const char* temp_cstr(string s) {
    static char buffer[128];
    assert(s.size() < sizeof(buffer));
    memcpy(buffer, s.data(), s.size());
    buffer[s.size()] = 0;
    return buffer;
}

Result load_skybox(cJSON *j_skybox, Graphics& gfx, Skybox*) {
    cJSON *j_filename = cJSON_GetObjectItem(j_skybox, "File");
    if (!cJSON_IsString(j_filename) || (j_filename->valuestring == NULL))
        return ERROR("Skybox must contain a file to load!");

    optional<string> opt_contents = read_entire_file(j_filename->valuestring);
    if (!opt_contents.has_value())
        return ERROR("Failed to read file \"%s\"", j_filename->valuestring);

    AutoFree<string> contents { opt_contents.value() };
    
    CubemapParser parser;
    if (parser.parse(j_filename->valuestring, contents))
        return ERROR("Failed to parse cubemap file");

    // Temperarily set the current path to the scene's directory when loading
    auto old_path = std::filesystem::current_path();
    std::filesystem::current_path(path(j_filename->valuestring).parent_path());

    int width, height, comp;
    void *pixels = stbi_load(temp_cstr(parser.images[0]), &width, &height, &comp, 4);

    assert(width == height);
    u32 size = u32(width);

    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    Graphics::Image image = gfx.create_image("Skybox", usage, VK_FORMAT_R8G8B8A8_UNORM, size, size, Graphics::IMAGE_FLAG_CREATE_CUBE_MAP);

    for (int i = 0; i < 6; ++i) {
        if (pixels == 0)
            pixels = stbi_load(temp_cstr(parser.images[i]), &width, &height, &comp, 4);

        gfx.write_image(image, pixels, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i);
        free(pixels);
        pixels = 0;
    }

    VkSampler sampler = 0;
    VkSamplerCreateInfo sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
    };
    assert(vkCreateSampler(gfx.device, &sampler_info, 0, &sampler) == VK_SUCCESS);

    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = image.view;
    imageInfo.sampler = sampler;
    VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.dstBinding = 2;
    write.dstSet = gfx.descriptor_set;
    write.pImageInfo = &imageInfo;
    vkUpdateDescriptorSets(gfx.device, 1, &write, 0, 0);

    printf("Info: Loaded Skybox [%ix%i] \"%s\"\n", width, height, j_filename->valuestring);
    std::filesystem::current_path(old_path);
    return Success;
}

Result CubemapParser::parse(string filename, string contents)
{
    Tokenizer tokenizer { filename, contents, true };
    while (tokenizer.has_tokens()) {
        Token tok = tokenizer.get();
        if (tok.is_symbol() && tok.symbol.value == '#') {
            while (!tokenizer.next().is_end_or_newline());
            continue;
        }
        if (!tok.is_identifier()) {
            tokenizer.next();
            continue;
        }

        tokenizer.next();
        string id = tok.identifier.value;
        int start = tokenizer.location.offset;

        Token t = tokenizer.next();
        while (!t.is_end_or_newline())
        {
            if (t.is_symbol() && t.symbol.value == '"') {
                int end = tokenizer.location.offset;
                // Have to subtract 1 here, to account for the last token (")
                int length = end - start - 1;
                int index = 0;

                if (id == "front")      index = 0;
                else if (id == "back")  index = 1;
                else if (id == "up")    index = 2;
                else if (id == "down")  index = 3;
                else if (id == "right") index = 4;
                else if (id == "left")  index = 5;

                images[index] = string(contents.data() + start, length);
                break;
            }
            t = tokenizer.next();
        }

        while (!tokenizer.next().is_end_or_newline());
    }
    return Success;
}

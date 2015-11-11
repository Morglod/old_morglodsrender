#pragma once

const uint32_t map_size = 20;
const uint32_t map_planes_num = map_size * map_size;
const uint32_t map_blocks_num = 2;

glm::vec3 map_blocks[map_blocks_num];
mr::BufferPtr map_blocks_ubo;

glm::vec3 map_planes[map_planes_num];
mr::BufferPtr map_planes_ubo;

void _Map_Do_Write(void* data, size_t size, mr::BufferPtr& out_b) {
    using namespace mr;

    const auto dataMem = Memory::Ref(data, size);
    Buffer::CreationFlags flags;
    out_b = Buffer::Create(dataMem, flags);
    out_b->MakeResident(MR_RESIDENT_READ_ONLY);
}

void InitMap() {
    for(size_t iy = 0; iy < map_size; ++iy) {
        for(size_t ix = 0; ix < map_size; ++ix) {
            map_planes[ix + iy*map_size] = glm::vec3((float)ix,0.0f,(float)iy);
        }
    }

    map_blocks[0] = glm::vec3(0.0f,0.0f,0.0f);
    map_blocks[1] = glm::vec3(1.0f,0.0f,0.0f);

    _Map_Do_Write(&map_blocks[0], map_blocks_num * sizeof(glm::vec3), map_blocks_ubo);
    _Map_Do_Write(&map_planes[0], map_planes_num * sizeof(glm::vec3), map_planes_ubo);
}

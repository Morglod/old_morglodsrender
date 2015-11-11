#pragma once

struct Vertex {
    float xyz[3];
    float texCoord[2];
};

const size_t box_vertexNum = 8;
static Vertex box_vertexData[box_vertexNum] = {
    // forward
    {-0.5f,  0.5f,  0.5f, 0,0 },
	{ 0.5f,  0.5f,  0.5f, 1,0 },
	{-0.5f, -0.5f,  0.5f, 0,1 },
	{ 0.5f, -0.5f,  0.5f, 1,1 },
	//backward
	{-0.5f,  0.5f, -0.5f, 1,0 },
	{ 0.5f,  0.5f, -0.5f, 0,0 },
	{-0.5f, -0.5f, -0.5f, 1,1 },
	{ 0.5f, -0.5f, -0.5f, 0,1 },
};

const size_t box_indexNum = 24;
static uint16_t box_indexData[box_indexNum] = {
	0,1,2,
	1,3,2,

	5,1,7,
	3,1,7,

	4,5,6,
	7,6,5,

	0,6,4,
	2,6,0
};

mr::VertexBufferPtr box_vertexBuffer = nullptr;
mr::IndexBufferPtr box_indexBuffer = nullptr;
std::future<bool> box_fv, box_fi;

const size_t plane_vertexNum = 4;
static Vertex plane_vertexData[plane_vertexNum] = {
    {-0.5f, -0.5f, 0.5f, 1,1},
    {0.5f, -0.5f, 0.5f, 0,1},
    {0.5f, -0.5f, -0.5f, 0,0},
    {-0.5f, -0.5f, -0.5f, 1,0},
};

const size_t plane_indexNum = 6;
static uint16_t plane_indexData[plane_indexNum] = {
    0, 1, 2,
    3, 0, 2
};

mr::VertexBufferPtr plane_vertexBuffer = nullptr;
mr::IndexBufferPtr plane_indexBuffer = nullptr;
std::future<bool> plane_fv, plane_fi;

void _Geom_Do_Write(void* vdata, size_t vdataSize, void* idata, size_t idataSize, mr::BufferPtr& out_v, mr::BufferPtr& out_i, std::future<bool>& out_fv, std::future<bool>& out_fi) {
    using namespace mr;

    const auto vertexDataMem = Memory::Ref(vdata, vdataSize);
    const auto indexDataMem = Memory::Ref(idata, idataSize);

    Buffer::CreationFlags flags;
    out_v = Buffer::Create(Memory::Zero(vertexDataMem->GetSize()), flags);
    out_i = Buffer::Create(Memory::Zero(indexDataMem->GetSize()), flags);

    out_v->MakeResident(MR_RESIDENT_READ_ONLY);
    out_i->MakeResident(MR_RESIDENT_READ_ONLY);

    out_fv = out_v->WriteAsync(vertexDataMem);
    out_fi = out_i->WriteAsync(indexDataMem);
}

void Pre_InitGeom() {
    using namespace mr;

    BufferPtr box_v, box_i,
              plane_v, plane_i;

    _Geom_Do_Write(&box_vertexData[0], box_vertexNum* sizeof(Vertex), &box_indexData[0], box_indexNum * sizeof(uint16_t), box_v, box_i, box_fv, box_fi);
    _Geom_Do_Write(&plane_vertexData[0], plane_vertexNum* sizeof(Vertex), &plane_indexData[0], plane_indexNum * sizeof(uint16_t), plane_v, plane_i, plane_fv, plane_fi);

    auto vdecl = VertexDecl::Create();
    auto vdef = vdecl->Begin();
    vdef.Pos()
        .Custom(DataType::Float, 2, 0, true)
        .End();

    box_vertexBuffer = VertexBuffer::Create(box_v, vdecl, box_vertexNum);
    plane_vertexBuffer = VertexBuffer::Create(plane_v, vdecl, plane_vertexNum);

    box_indexBuffer = IndexBuffer::Create(box_i, IndexDataType::UShort, box_indexNum);
    plane_indexBuffer = IndexBuffer::Create(plane_i, IndexDataType::UShort, plane_indexNum);
}

void Post_InitGeom() {
    box_fi.wait();
    box_fv.wait();
    plane_fi.wait();
    plane_fv.wait();
}

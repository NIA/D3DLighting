#pragma once
#include "main.h"
#include "Vertex.h"

class Model
{
private:
    IDirect3DDevice9        *device;

    unsigned    vertices_count;
    unsigned    primitives_count;

    D3DPRIMITIVETYPE        primitive_type;
    IDirect3DVertexBuffer9  *vertex_buffer;
    IDirect3DIndexBuffer9   *index_buffer;

    D3DXVECTOR3 bone_center;
    D3DXMATRIX bones[BONES_COUNT];

    D3DXVECTOR3 position;
    D3DXVECTOR3 rotation;
    D3DXMATRIX rotation_and_position;

    void update_matrix();

    void release_interfaces();

public:
    Model(  IDirect3DDevice9 *device,
            D3DPRIMITIVETYPE primitive_type,
            const Vertex *vertices,
            unsigned vertices_count,
            const Index *indices,
            unsigned indices_count,
            unsigned primitives_count,
            D3DXVECTOR3 position,
            D3DXVECTOR3 rotation,
            D3DXVECTOR3 bone_center);
    
    virtual void draw() const;
    void set_bones(float time);
    const D3DXMATRIX &get_bone(unsigned number) const;
    const D3DXMATRIX &get_rotation_and_position() const;

    virtual ~Model();
};
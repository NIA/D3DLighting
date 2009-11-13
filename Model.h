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
    unsigned vertex_size;

    D3DXVECTOR3 position;
    D3DXVECTOR3 rotation;
    D3DXMATRIX rotation_and_position;

    void update_matrix();

    void release_interfaces();

public:
    Model(  IDirect3DDevice9 *device,
            D3DPRIMITIVETYPE primitive_type,
            unsigned vertex_size,
            const Vertex *vertices,
            unsigned vertices_count,
            const Index *indices,
            unsigned indices_count,
            unsigned primitives_count,
            D3DXVECTOR3 position,
            D3DXVECTOR3 rotation);
    
    virtual void set_time(float time) { UNREFERENCED_PARAMETER(time); }
    const D3DXMATRIX &get_rotation_and_position() const;
    virtual void draw() const;

    virtual ~Model();
};

class SkinningModel : public Model
{
private:
    D3DXVECTOR3 bone_center;
    D3DXMATRIX bones[BONES_COUNT];
public:
    SkinningModel(  IDirect3DDevice9 *device,
                    D3DPRIMITIVETYPE primitive_type,
                    const SkinningVertex *vertices,
                    unsigned vertices_count,
                    const Index *indices,
                    unsigned indices_count,
                    unsigned primitives_count,
                    D3DXVECTOR3 position,
                    D3DXVECTOR3 rotation,
                    D3DXVECTOR3 bone_center);

    const D3DXMATRIX &get_bone(unsigned number) const;
    // Overrides:
    virtual void set_time(float time);
};
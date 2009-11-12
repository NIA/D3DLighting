#include "cylinder.h"

const Index CYLINDER_EDGES_PER_BASE = 120;
const Index CYLINDER_EDGES_PER_HEIGHT = 84;
const Index CYLINDER_EDGES_PER_CAP = 24;

const Index CYLINDER_VERTICES_COUNT 
    = (CYLINDER_EDGES_PER_BASE)*(CYLINDER_EDGES_PER_HEIGHT + CYLINDER_EDGES_PER_CAP + 1) // vertices per CYLINDER_EDGES_PER_HEIGHT+1 levels plus last level again, plus CYLINDER_EDGES_PER_CAP-1 levels per cap
    + 1; // plus the center of the cap
const DWORD CYLINDER_INDICES_COUNT
    = 2*(CYLINDER_EDGES_PER_BASE + 1)*(CYLINDER_EDGES_PER_HEIGHT + CYLINDER_EDGES_PER_CAP - 1) // indices per CYLINDER_EDGES_PER_HEIGHT levels plus CYLINDER_EDGES_PER_CAP-1 levels per cap
    + (2*CYLINDER_EDGES_PER_BASE + 1); // plus cap

void cylinder( D3DXVECTOR3 base_center, float radius, float height,
                Vertex *res_vertices, Index *res_indices)
// Writes data into arrays given as `res_vertices' and `res_indices',
{
    Index vertex = 0; // current vertex
    DWORD index = 0; // current index

    const float STEP_ANGLE = 2*D3DX_PI/CYLINDER_EDGES_PER_BASE;
    const float STEP_UP = height/CYLINDER_EDGES_PER_HEIGHT;
    const float STEP_RADIAL = radius/CYLINDER_EDGES_PER_CAP;
    
    _ASSERT(res_vertices != NULL);
    _ASSERT(res_indices != NULL);

    D3DCOLOR colors[] =
    {
        D3DCOLOR_XRGB(0, 150, 250),
        D3DCOLOR_XRGB(250, 30, 10),
        D3DCOLOR_XRGB(250, 250, 0),
        D3DCOLOR_XRGB(30, 250, 0),
        D3DCOLOR_XRGB(250, 250, 250),
    };
    const unsigned colors_num = array_size(colors);
    Index part_size = (CYLINDER_EDGES_PER_HEIGHT+colors_num+1)/colors_num; // `+ colors_num' just for excluding a bound of interval [0, colors_num)
    for( Index level = 0; level <= CYLINDER_EDGES_PER_HEIGHT; ++level )
    {
        for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
        {
            res_vertices[vertex] = Vertex( base_center
                                           + D3DXVECTOR3( radius*cos(step*STEP_ANGLE),
                                                          radius*sin(step*STEP_ANGLE),
                                                          level*STEP_UP),
                                           colors[level/part_size],
                                           static_cast<float>(level)/CYLINDER_EDGES_PER_HEIGHT,
                                           D3DXVECTOR3( cos(step*STEP_ANGLE),
                                                        sin(step*STEP_ANGLE),
                                                        0)
                                          );
            if( level != 0 )
            {
                res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE; // from previous level
                res_indices[index++] = vertex;                           // from current level
                if( step == CYLINDER_EDGES_PER_BASE - 1 ) // last step
                {
                    res_indices[index++] = vertex - 2*CYLINDER_EDGES_PER_BASE + 1; // first from previuos level
                    res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE + 1; // first from current level
                }
            }
            ++vertex;
        }
    }
    // Cap
    D3DXVECTOR3 normal_up( 0, 0, 1.0f );
    part_size = (CYLINDER_EDGES_PER_BASE + colors_num)/colors_num; // `+ colors_num' just for excluding a bound of interval [0, colors_num)
    for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
    {
        res_vertices[vertex] = res_vertices[vertex - CYLINDER_EDGES_PER_BASE];
        res_vertices[vertex].set_normal( normal_up );
        res_vertices[vertex].color = colors[step/part_size];
        ++vertex;
    }
    for( Index level = 1; level < CYLINDER_EDGES_PER_CAP; ++level )
    {
        for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
        {
            float local_radius =  radius - STEP_RADIAL*level;
            res_vertices[vertex] = Vertex( base_center
                                           + D3DXVECTOR3( local_radius*cos(step*STEP_ANGLE),
                                                          local_radius*sin(step*STEP_ANGLE),
                                                          height),
                                           colors[step/part_size],
                                           1.0f,
                                           normal_up
                                          );
            if( level != 0 )
            {
                res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE; // from previous level
                res_indices[index++] = vertex;                           // from current level
                if( step == CYLINDER_EDGES_PER_BASE - 1 ) // last step
                {
                    res_indices[index++] = vertex - 2*CYLINDER_EDGES_PER_BASE + 1; // first from previuos level
                    res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE + 1; // first from current level
                }
            }
            ++vertex;
        }
    }
    res_vertices[vertex] = Vertex( base_center + D3DXVECTOR3( 0, 0, height), colors[0], 1.0f, normal_up );
    for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
    {
        res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE + step;
        res_indices[index++] = vertex;
    }
    res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE;
}

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

namespace
{
    struct GENERATION_PARAMS
    {
        // output buffers
        Vertex *res_vertices;
        Index *res_indices;
        // sizes
        D3DXVECTOR3 base_center;
        float radius;
        float height;
        // colors
        D3DCOLOR *colors;
        unsigned colors_num;
        // options
        bool radial_strips; // radial (depending on step) or vertical (depending on level) color distribution
        bool vertical;      // vertical (for cylinder side) or horisontal (for caps) moving whe generating
        bool top;           // for cap generation: is it top or bottom cap. Ignored when vertical == true
    };

    void generate_levels(Index &vertex, DWORD &index, const GENERATION_PARAMS &params)
    {
        const float STEP_ANGLE = 2*D3DX_PI/CYLINDER_EDGES_PER_BASE;
        const float STEP_UP = params.height/CYLINDER_EDGES_PER_HEIGHT;
        const float STEP_RADIAL = params.radius/CYLINDER_EDGES_PER_CAP;

        Index levels_count = params.vertical ? CYLINDER_EDGES_PER_HEIGHT + 1 : CYLINDER_EDGES_PER_CAP;
        Index levels_or_steps_count = params.radial_strips ? CYLINDER_EDGES_PER_BASE : levels_count;
        Index part_size = (levels_or_steps_count + params.colors_num)/params.colors_num; // `+ colors_num' just for excluding a bound of interval [0, colors_num)
        
        D3DXVECTOR3 normal_if_horisontal = D3DXVECTOR3(0, 0, params.top ? 1.0f : -1.0f);
        float z_if_horisontal = params.top ? params.height : 0.0f;
        float weight_if_horisontal = params.top ? 1.0f : 0.0f;
    
        for( Index level = 0; level < levels_count; ++level )
        {
            for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
            {
                float radius = params.vertical ? params.radius : (params.radius - STEP_RADIAL*level);
                float z, weight;
                D3DXVECTOR3 normal;
                if (params.vertical)
                {
                    z = level*STEP_UP;
                    weight = static_cast<float>(level)/CYLINDER_EDGES_PER_HEIGHT;
                    normal = D3DXVECTOR3( cos(step*STEP_ANGLE), sin(step*STEP_ANGLE), 0 );
                }
                else
                {
                    normal = normal_if_horisontal;
                    z = z_if_horisontal;
                    weight = weight_if_horisontal;
                }
                D3DXVECTOR3 position = params.base_center + D3DXVECTOR3( radius*cos(step*STEP_ANGLE),
                                                                         radius*sin(step*STEP_ANGLE),
                                                                         z);
                D3DCOLOR color = params.radial_strips ? params.colors[step/part_size] : params.colors[level/part_size];

                if( level == 0 && !params.vertical)
                {
                    // first level for horisontal is just copy of:
                    // * last vertices (for top cap)
                    //    OR
                    // * first vertices (for bottom cap)
                    unsigned copy_from = params.top ? vertex - CYLINDER_EDGES_PER_BASE : step;
                    params.res_vertices[vertex] = params.res_vertices[copy_from];
                    params.res_vertices[vertex].set_normal( normal );
                    params.res_vertices[vertex].color = color;
                    ++vertex;
                    continue;
                }
                params.res_vertices[vertex] = Vertex(position, color, weight, normal);
                if( level != 0 )
                {
                    params.res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE; // from previous level
                    params.res_indices[index++] = vertex;                           // from current level
                    if( step == CYLINDER_EDGES_PER_BASE - 1 ) // last step
                    {
                        params.res_indices[index++] = vertex - 2*CYLINDER_EDGES_PER_BASE + 1; // first from previuos level
                        params.res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE + 1; // first from current level
                    }
                }
                ++vertex;
            }
        }
        if( !params.vertical )
        {
            // for caps: add center vertex and triangles with it
            D3DXVECTOR3 position = params.base_center + D3DXVECTOR3( 0, 0, z_if_horisontal );
            params.res_vertices[vertex] = Vertex( position, params.colors[0], weight_if_horisontal, normal_if_horisontal );
            for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
            {
                params.res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE + step;
                params.res_indices[index++] = vertex;
            }
            params.res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE;
            ++vertex;
        }
    }
}

void cylinder( D3DXVECTOR3 base_center, float radius, float height,
                Vertex *res_vertices, Index *res_indices)
// Writes data into arrays given as `res_vertices' and `res_indices',
{
    Index vertex = 0; // current vertex
    DWORD index = 0; // current index
    
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

    GENERATION_PARAMS params;
    // output buffers
    params.res_vertices = res_vertices;
    params.res_indices = res_indices;
    // sizes
    params.base_center = base_center;
    params.radius = radius;
    params.height = height;
    // colors
    params.colors = colors;
    params.colors_num = colors_num;
    // options
    params.radial_strips = false;
    params.vertical = true;

    generate_levels(vertex, index, params);

    // Cap
    D3DXVECTOR3 normal_up( 0, 0, 1.0f );
    //for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
    //{
    //    res_vertices[vertex] = res_vertices[vertex - CYLINDER_EDGES_PER_BASE];
    //    res_vertices[vertex].set_normal( normal_up );
    //    res_vertices[vertex].color = colors[step/part_size];
    //    ++vertex;
    //}

    params.radial_strips = true;
    params.vertical = false;
    params.top = true;

    generate_levels(vertex, index, params);
}

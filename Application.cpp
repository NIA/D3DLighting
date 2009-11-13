#include "Application.h"
#include <time.h>

const unsigned VECTORS_IN_MATRIX = sizeof(D3DXMATRIX)/sizeof(D3DXVECTOR4);

namespace
{
    const int         WINDOW_SIZE = 600;
    const D3DCOLOR    BACKGROUND_COLOR = D3DCOLOR_XRGB( 5, 5, 10 );
    const bool        INITIAL_WIREFRAME_STATE = false;
    const unsigned    D3DXVEC_SIZE = sizeof(D3DXVECTOR4);
    const D3DCOLOR    BLACK = D3DCOLOR_XRGB( 0, 0, 0 );

    //---------------- SHADER CONSTANTS ---------------------------
    //    c0-c3 is the view matrix
    const unsigned    SHADER_REG_VIEW_MX = 0;
    //    c4-c7 is the first bone matrix
    //    c8-c11 is the second bone matrix
    const unsigned    SHADER_REG_BONE_MX = 4;
    //    c12 is directional light vector
    const unsigned    SHADER_REG_DIRECTIONAL_VECTOR = 12;
    const D3DXVECTOR3 SHADER_VAL_DIRECTIONAL_VECTOR  (0, 1.0f, 0.8f);
    //    c13 is directional light color
    const unsigned    SHADER_REG_DIRECTIONAL_COLOR = 13;
    const D3DCOLOR    SHADER_VAL_DIRECTIONAL_COLOR = D3DCOLOR_XRGB(204, 178, 25);
    //    c14 is diffuse coefficient
    const unsigned    SHADER_REG_DIFFUSE_COEF = 14;
    const float       SHADER_VAL_DIFFUSE_COEF = 0.7f;
    //    c15 is ambient color
    const unsigned    SHADER_REG_AMBIENT_COLOR = 15;
    const D3DCOLOR    SHADER_VAL_AMBIENT_COLOR = D3DCOLOR_XRGB(13, 33, 13);
    //    c16 is point light color
    const unsigned    SHADER_REG_POINT_COLOR = 16;
    const D3DCOLOR    SHADER_VAL_POINT_COLOR = D3DCOLOR_XRGB(25, 153, 153);
    //    c17 is point light position
    const unsigned    SHADER_REG_POINT_POSITION = 17;
    const D3DXVECTOR3 SHADER_VAL_POINT_POSITION  (0.2f, -0.91f, -1.1f);
    //    c18 are attenuation constants
    const unsigned    SHADER_REG_ATTENUATION = 18;
    const D3DXVECTOR3 SHADER_VAL_ATTENUATION  (1.0f, 0, 0.5f);
    //    c19 is specular coefficient
    const unsigned    SHADER_REG_SPECULAR_COEF = 19;
    const float       SHADER_VAL_SPECULAR_COEF = 0.4f;
    //    c20 is specular constant 'f'
    const unsigned    SHADER_REG_SPECULAR_F = 20;
    const float       SHADER_VAL_SPECULAR_F = 15.0f;
    //    c21 is eye position
    const unsigned    SHADER_REG_EYE = 21;
    //    c22 is spot light position
    const unsigned    SHADER_REG_SPOT_POSITION = 22;
    const D3DXVECTOR3 SHADER_VAL_SPOT_POSITION  (1.5f, 1.5f, -1.3f);
    //    c23 is spot light color
    const unsigned    SHADER_REG_SPOT_COLOR = 23;
    const D3DCOLOR    SHADER_VAL_SPOT_COLOR = D3DCOLOR_XRGB(255, 0, 180);
    //    c24 is spot light direction
    const unsigned    SHADER_REG_SPOT_VECTOR = 24;
    const D3DXVECTOR3 SHADER_VAL_SPOT_VECTOR  (1.0f, 1.0f, -0.5f);
    D3DXVECTOR3 spot_vector(2.0f, -0.7f, -1.1f);
    //    c25 is cos( spot light inner angle )
    const unsigned    SHADER_REG_SPOT_INNER_ANGLE = 25;
    const float       SHADER_VAL_SPOT_INNER_ANGLE = D3DX_PI/16.0f;
    //    c26 is cos( spot light outer angle )
    const unsigned    SHADER_REG_SPOT_OUTER_ANGLE = 26;
    const float       SHADER_VAL_SPOT_OUTER_ANGLE = D3DX_PI/12.0f;
    //    c27-c30 is position and rotation of model matrix
    const unsigned    SHADER_REG_POS_AND_ROT_MX = 27;
}

Application::Application() :
    d3d(NULL), device(NULL), window(WINDOW_SIZE, WINDOW_SIZE), camera(2.9f, 1.5f, 0.0f), // Constants selected for better view of cylinder
    directional_light_enabled(true), point_light_enabled(true), spot_light_enabled(true), ambient_light_enabled(true)
{
    try
    {
        init_device();
    }
    // using catch(...) because every caught exception is rethrown
    catch(...)
    {
        release_interfaces();
        throw;
    }
}

void Application::init_device()
{
    d3d = Direct3DCreate9( D3D_SDK_VERSION );
    if( d3d == NULL )
        throw D3DInitError();

    // Set up the structure used to create the device
    D3DPRESENT_PARAMETERS present_parameters;
    ZeroMemory( &present_parameters, sizeof( present_parameters ) );
    present_parameters.Windowed = TRUE;
    present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
    present_parameters.EnableAutoDepthStencil = TRUE;
    present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
    // Create the device
    if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &present_parameters, &device ) ) )
        throw D3DInitError();
    check_state( device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) );
    toggle_wireframe();
}

void Application::render()
{
    check_render( device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BACKGROUND_COLOR, 1.0f, 0 ) );
    
    // Begin the scene
    check_render( device->BeginScene() );
    // Setting constants
    float time = static_cast<float>( clock() )/static_cast<float>( CLOCKS_PER_SEC );
    D3DXVECTOR3 directional_vector;
    D3DXVec3Normalize(&directional_vector, &SHADER_VAL_DIRECTIONAL_VECTOR);
    D3DXVECTOR3 spot_vector;
    D3DXVec3Normalize(&spot_vector, &SHADER_VAL_SPOT_VECTOR);

    D3DCOLOR ambient_color = ambient_light_enabled ? SHADER_VAL_AMBIENT_COLOR : BLACK;
    D3DCOLOR directional_color = directional_light_enabled ? SHADER_VAL_DIRECTIONAL_COLOR : BLACK;
    D3DCOLOR point_color = point_light_enabled ? SHADER_VAL_POINT_COLOR : BLACK;
    D3DCOLOR spot_color = spot_light_enabled ? SHADER_VAL_SPOT_COLOR : BLACK;

    set_shader_matrix( SHADER_REG_VIEW_MX,            camera.get_matrix());
    set_shader_vector( SHADER_REG_DIRECTIONAL_VECTOR, directional_vector);
    set_shader_color(  SHADER_REG_DIRECTIONAL_COLOR,  directional_color);
    set_shader_float(  SHADER_REG_DIFFUSE_COEF,       SHADER_VAL_DIFFUSE_COEF);
    set_shader_color(  SHADER_REG_AMBIENT_COLOR,      ambient_color);
    set_shader_color(  SHADER_REG_POINT_COLOR,        point_color);
    set_shader_point(  SHADER_REG_POINT_POSITION,     SHADER_VAL_POINT_POSITION);
    set_shader_vector( SHADER_REG_ATTENUATION,        SHADER_VAL_ATTENUATION);
    set_shader_float(  SHADER_REG_SPECULAR_COEF,      SHADER_VAL_SPECULAR_COEF);
    set_shader_float(  SHADER_REG_SPECULAR_F,         SHADER_VAL_SPECULAR_F);
    set_shader_point(  SHADER_REG_EYE,                camera.get_eye());
    set_shader_point(  SHADER_REG_SPOT_POSITION,      SHADER_VAL_SPOT_POSITION);
    set_shader_color(  SHADER_REG_SPOT_COLOR,         spot_color);
    set_shader_vector( SHADER_REG_SPOT_VECTOR,        spot_vector);
    set_shader_float(  SHADER_REG_SPOT_INNER_ANGLE,   cos(SHADER_VAL_SPOT_INNER_ANGLE));
    set_shader_float(  SHADER_REG_SPOT_OUTER_ANGLE,   cos(SHADER_VAL_SPOT_OUTER_ANGLE));
    std::list<Model*>::iterator end = models.end();
    for ( std::list<Model*>::iterator iter = models.begin(); iter != end; ++iter )
    {
        // Set up
        ( (*iter)->get_shader() ).set();
        // Setting constants
        (*iter)->set_time(time);
        SkinningModel *skinning_model = dynamic_cast<SkinningModel*>(*iter);
        if( skinning_model != NULL)
        {
            // if it is really a SkinningModel
            unsigned offset = SHADER_REG_BONE_MX;
            for(unsigned i = 0; i < BONES_COUNT; ++i)
            {
                set_shader_matrix(offset, skinning_model->get_bone(i));
                offset += VECTORS_IN_MATRIX;
            }
        }
        set_shader_matrix( SHADER_REG_POS_AND_ROT_MX, (*iter)->get_rotation_and_position() );
        // Draw
        (*iter)->draw();
    }
    // End the scene
    check_render( device->EndScene() );
    
    // Present the backbuffer contents to the display
    check_render( device->Present( NULL, NULL, NULL, NULL ) );

}

IDirect3DDevice9 * Application::get_device()
{
    return device;
}

void Application::add_model(Model &model)
{
    models.push_back( &model );
}

void Application::remove_model(Model &model)
{
    models.remove( &model );
}

void Application::process_key(unsigned code)
{
    switch( code )
    {
    case VK_ESCAPE:
        PostQuitMessage( 0 );
        break;
    case VK_UP:
    case 'W':
        camera.move_up();
        break;
    case VK_DOWN:
    case 'S':
        camera.move_down();
        break;
    case VK_PRIOR:
    case VK_ADD:
    case VK_OEM_PLUS:
        camera.move_nearer();
        break;
    case VK_NEXT:
    case VK_SUBTRACT:
    case VK_OEM_MINUS:
        camera.move_farther();
        break;
    case VK_LEFT:
    case 'A':
        camera.move_clockwise();
        break;
    case VK_RIGHT:
    case 'D':
        camera.move_counterclockwise();
        break;
    case VK_SPACE:
        toggle_wireframe();
        break;
    case '1':
        directional_light_enabled = !directional_light_enabled;
        break;
    case '2':
        point_light_enabled = !point_light_enabled;
        break;
    case '3':
        spot_light_enabled = !spot_light_enabled;
        break;
    case '4':
        ambient_light_enabled = !ambient_light_enabled;
        break;
    }
}

void Application::run()
{
    window.show();
    window.update();

    // Enter the message loop
    MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            if( msg.message == WM_KEYDOWN )
            {
                process_key( static_cast<unsigned>( msg.wParam ) );
            }

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
            render();
    }
}

void Application::toggle_wireframe()
{
    static bool wireframe = !INITIAL_WIREFRAME_STATE;
    wireframe = !wireframe;

    if( wireframe )
    {
        check_state( device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME ) );
    }
    else
    {
        check_state( device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID ) );
    }
}

void Application::release_interfaces()
{
    release_interface( d3d );
    release_interface( device );
}

Application::~Application()
{
    release_interfaces();
}
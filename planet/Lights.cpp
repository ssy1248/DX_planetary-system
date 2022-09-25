#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices
LPD3DXMESH Objects[8] = {0,}; //행성들을 표기하기 위한 메시

enum Planet {SUN, MERCURY, VENUS, EARTH, MARS, JUPITER, MOON};
Planet planet;

struct c_pos
{
    FLOAT x, y, z;
};

struct s_pos
{
    FLOAT x, y, z;
};

struct CUSTOMVERTEX
{
    //D3DXVECTOR3 position; // The 3D position for the vertex
    //D3DXVECTOR3 normal;   // The surface normal for the vertex
    FLOAT x, y, z;
    DWORD color;
};

static c_pos campos = { 0.0f, 3.0f, -10.0f };

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)

HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}

HRESULT InitGeometry()
{
    // Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50 * 2 * sizeof( CUSTOMVERTEX ),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer. We are algorithmically generating a cylinder
    // here, including the normals, which are used for lighting.
    CUSTOMVERTEX* pVertices;
    if( FAILED( g_pVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
        return E_FAIL;

    for (int i = 0; i < 8; i++)
    {
        switch (i)
        {
        case 0:
            D3DXCreateSphere(g_pd3dDevice, 0.5f, 30, 30, &Objects[0], 0);
            break;
        case 1:
            D3DXCreateSphere(g_pd3dDevice, 0.2f, 30, 30, &Objects[1], 0);
            break;
        case 2:
            D3DXCreateSphere(g_pd3dDevice, 0.3f, 30, 30, &Objects[2], 0);
            break;
        case 3:
            D3DXCreateSphere(g_pd3dDevice, 0.4f, 30, 30, &Objects[3], 0);
            break;
        case 4:
            D3DXCreateSphere(g_pd3dDevice, 0.25f, 30, 30, &Objects[4], 0);
            break;
        case 5:
            D3DXCreateSphere(g_pd3dDevice, 1.5f, 30, 30, &Objects[5], 0);
            break;
        case 6:
            D3DXCreateSphere(g_pd3dDevice, 0.08f, 30, 30, &Objects[6], 0);
            break;
        case 7:
            D3DXCreateBox(g_pd3dDevice, 0.1f, 0.1f, 0.1f, &Objects[7], 0);
            break;
        }
    }

    g_pVB->Unlock();

    return S_OK;
}

VOID Cleanup()
{
    if( g_pVB != NULL )
        g_pVB->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();

    for (int i = 0; i < 8; i++)
    {
        if (Objects[i] != NULL)
            Objects[i]->Release();
    }
}

VOID SetupMatrices(int index)
{
    // Set up world matrix
    D3DXMATRIXA16 matWorld;
    D3DXMATRIXA16 EarthMat;
    D3DXMATRIXA16 translation;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixIdentity(&EarthMat);

    UINT iTime;
    FLOAT fAngle;

    switch (index)
    {
    case 0:
        iTime = timeGetTime() % 5000;
        fAngle = iTime * (2.0f * D3DX_PI) / 5000.0f;

        D3DXMatrixRotationY(&matWorld, fAngle);
        D3DXMatrixTranslation(&translation, 0.0f, 0.0f, 0.0f);
        break;
    case 1:
        iTime = timeGetTime() % 1000;
        fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;

        D3DXMatrixRotationY(&matWorld, fAngle);
        D3DXMatrixTranslation(&translation, 1.0f, 0.0f, 0.0f);
        break;
    case 2:
        iTime = timeGetTime() % 2000;
        fAngle = iTime * (2.0f * D3DX_PI) / 2000.0f;

        D3DXMatrixRotationY(&matWorld, fAngle);
        D3DXMatrixTranslation(&translation, 2.0f, 0.0f, 0.0f);
        break;
    case 3:
        iTime = timeGetTime() % 3000;
        fAngle = iTime * (2.0f * D3DX_PI) / 3000.0f;

        D3DXMatrixRotationY(&matWorld, fAngle);
        D3DXMatrixTranslation(&translation, 3.0f, 0.0f, 0.0f);
        break;
    case 4:
        iTime = timeGetTime() % 4000;
        fAngle = iTime * (2.0f * D3DX_PI) / 4000.0f;

        D3DXMatrixRotationY(&matWorld, fAngle);
        D3DXMatrixTranslation(&translation, 4.0f, 0.0f, 0.0f);
        break;
    case 5:
        iTime = timeGetTime() % 5000;
        fAngle = iTime * (2.0f * D3DX_PI) / 5000.0f;

        D3DXMatrixRotationY(&matWorld, fAngle);
        D3DXMatrixTranslation(&translation, 7.0f, 0.0f, 0.0f);
        break;
    case 6:
        iTime = timeGetTime() % 3000;
        fAngle = iTime * (2.0f * D3DX_PI) / 3000.0f;

        D3DXMatrixRotationY(&matWorld, fAngle);
        D3DXMatrixTranslation(&translation, 3.6f, 0.0f, 0.0f);
        break;
    case 7: {
        static s_pos pos = { 0.0f,0.0f,0.0f };
        if (::GetAsyncKeyState('W') & 0x8000) {
            pos.z += 0.1f;
            campos.z += 0.1f;
        }
        if (::GetAsyncKeyState('S') & 0x8000) {
            pos.z -= 0.1f;
            campos.z -= 0.1f;
        }
        if (::GetAsyncKeyState('A') & 0x8000) {
            pos.x -= 0.1f;
        }
        if (::GetAsyncKeyState('D') & 0x8000) {
            pos.x += 0.1f;
        }
        D3DXMatrixTranslation(&translation, pos.x, pos.y, pos.z);
        break;
    }
    }

    if (index == 3)
    {
        EarthMat = matWorld = translation * matWorld;
    }
    else if (index == 6)
    {
        matWorld = translation * matWorld * EarthMat;
    }
    else
        matWorld = translation * matWorld;

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

}

VOID SetupLights()
{

    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof( D3DMATERIAL9 ) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );

    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof( D3DLIGHT9 ) );
    light.Type = D3DLIGHT_POINT;
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;
    vecDir = D3DXVECTOR3( cosf( timeGetTime() / 350.0f ),
                          1.0f,
                          sinf( timeGetTime() / 350.0f ) );
    D3DXVec3Normalize( ( D3DXVECTOR3* )&light.Direction, &vecDir );
    light.Position = { 0.0f, 0.0f, -2.5f };
    light.Range = 1000.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Finally, turn on some ambient light.
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
}

VOID Render()
{
    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        D3DXVECTOR3 vEyePt(campos.x, campos.y, campos.z); //카메라 위치
        D3DXVECTOR3 vLookatPt(campos.x, campos.y - 0.25f, campos.z + 0.5f);
        D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
        D3DXMATRIXA16 matView;
        D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
        g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

        D3DXMATRIXA16 matProj;
        D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

        // Setup the lights and materials
        SetupLights();

        // Setup the world, view, and projection matrices


        for (int i = 0; i < 8; i++)
        {
            SetupMatrices(i);
            Objects[i]->DrawSubset(0);
        }

        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
        g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
        g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2 );

        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 04: Lights",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1200, 700,
                              NULL, NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}




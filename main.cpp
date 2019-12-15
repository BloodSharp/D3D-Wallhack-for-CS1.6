#define INITGUID
#include <windows.h>
#include "d3d.h"
#include "ddraw.h"

VOID DbgPrint(PCSTR form, ...){
  static char tmp[2048];va_list p;
  va_start(p,form);
  wvsprintf(tmp,form,p);
  OutputDebugString(tmp);
  va_end(p);
}

PVOID GetInterfaceMethod(PVOID intf,DWORD methodIndex){
  return (PVOID)(*(DWORD*)intf+methodIndex*4);
}

PVOID ReplaceFunction(PVOID dirApi,PVOID newDirApi){
  DWORD dwOldProtect;PVOID oldDirApi=NULL;
  if(!dirApi||!newDirApi)return FALSE;
  if(VirtualProtect(dirApi,sizeof(PVOID),PAGE_EXECUTE_READWRITE,&dwOldProtect)){
    oldDirApi=*((PVOID*)dirApi);
    *((PVOID*)dirApi)=newDirApi;
    VirtualProtect(dirApi,sizeof(PVOID),dwOldProtect,&dwOldProtect);
  }
    return oldDirApi;
}

typedef PVOID (WINAPI* tDirectDrawCreate)(GUID FAR *,LPDIRECTDRAW FAR *,IUnknown FAR *);
typedef HRESULT (WINAPI* tIDirect3D3_CreateDevice)(LPDIRECT3D3,REFCLSID,LPDIRECTDRAWSURFACE4,LPDIRECT3DDEVICE3*,LPUNKNOWN);

tIDirect3D3_CreateDevice pIDirect3D3_CreateDevice;

typedef HRESULT (WINAPI *tBeginScene)(LPDIRECT3DDEVICE3);
typedef HRESULT (WINAPI *tEndScene)(LPDIRECT3DDEVICE3);
/*typedef HRESULT (WINAPI *tDrawPrimitive)(LPDIRECT3DDEVICE3,D3DPRIMITIVETYPE,DWORD,LPVOID,DWORD,DWORD);
typedef HRESULT (WINAPI *tDrawIndexedPrimitive)(LPDIRECT3DDEVICE3,D3DPRIMITIVETYPE,INT,UINT,UINT,UINT,UINT);
typedef HRESULT (WINAPI *tDrawPrimitiveStrided)(LPDIRECT3DDEVICE3,D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,DWORD);
typedef HRESULT (WINAPI *tDrawIndexedPrimitiveStrided)(LPDIRECT3DDEVICE3,D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,LPWORD,DWORD,DWORD);
typedef HRESULT (WINAPI *tDrawPrimitiveVB)(LPDIRECT3DDEVICE3,D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER,DWORD,DWORD,DWORD);*/
typedef HRESULT (WINAPI *tDrawIndexedPrimitiveVB)(LPDIRECT3DDEVICE3,D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER,LPWORD,DWORD,DWORD);

tBeginScene pBeginScene;
tEndScene pEndScene;
/*tDrawPrimitive pDrawPrimitive;
tDrawIndexedPrimitive pDrawIndexedPrimitive;
tDrawPrimitiveStrided pDrawPrimitiveStrided;
tDrawIndexedPrimitiveStrided pDrawIndexedPrimitiveStrided;
tDrawPrimitiveVB pDrawPrimitiveVB;*/
tDrawIndexedPrimitiveVB pDrawIndexedPrimitiveVB;

HRESULT WINAPI hBeginScene(LPDIRECT3DDEVICE3 pDevice){
    //DbgPrint("hBeginScene CALLBACK!!!");
    return pBeginScene(pDevice);
}

HRESULT WINAPI hEndScene(LPDIRECT3DDEVICE3 pDevice){
  //DbgPrint("hEndScene CALLBACK!!!");
  return pEndScene(pDevice);
}
/*
HRESULT WINAPI hDrawPrimitive(LPDIRECT3DDEVICE3 pDevice,D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,LPVOID lpvVertices,DWORD dwVertexCount,DWORD dwFlags){
  DbgPrint("hDrawPrimitive CALLBACK!!!");
  return pDrawPrimitive(pDevice,dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,dwFlags);
}

HRESULT WINAPI hDrawIndexedPrimitive(LPDIRECT3DDEVICE3 pDevice,D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount){
  DbgPrint("hDrawIndexedPrimitive CALLBACK!!!");
  return pDrawIndexedPrimitive(pDevice,Type,BaseVertexIndex,MinIndex,NumVertices,StartIndex,PrimitiveCount);
}

HRESULT WINAPI hDrawPrimitiveStrided(LPDIRECT3DDEVICE3 pDevice,D3DPRIMITIVETYPE a,DWORD b,LPD3DDRAWPRIMITIVESTRIDEDDATA c,DWORD d,DWORD e){
  DbgPrint("hDrawPrimitiveStrided CALLBACK!!!");
  return pDrawPrimitiveStrided(pDevice,a,b,c,d,e);
}

HRESULT WINAPI hDrawIndexedPrimitiveStrided(LPDIRECT3DDEVICE3 pDevice,D3DPRIMITIVETYPE d3dptPrimitiveType,DWORD  dwVertexTypeDesc,LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags){
  DbgPrint("hDrawIndexedPrimitiveStrided CALLBACK!!!");
  return pDrawIndexedPrimitiveStrided(pDevice,d3dptPrimitiveType,dwVertexTypeDesc,lpVertexArray,dwVertexCount,lpwIndices,dwIndexCount,dwFlags);
}

HRESULT WINAPI hDrawPrimitiveVB(LPDIRECT3DDEVICE3 pDevice,D3DPRIMITIVETYPE a,LPDIRECT3DVERTEXBUFFER b,DWORD c,DWORD d,DWORD e){
  DbgPrint("hDrawPrimitiveVB CALLBACK!!!");
  return pDrawPrimitiveVB(pDevice,a,b,c,d,e);
}*/

HRESULT WINAPI hDrawIndexedPrimitiveVB(LPDIRECT3DDEVICE3 pDevice,D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags){
  if(dwIndexCount>=2000){
    DWORD dwZMode,dwFill,dwZFunc;

    pDevice->GetRenderState(D3DRENDERSTATE_ZENABLE, &dwZMode);
    pDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);

    pDevice->GetRenderState(D3DRENDERSTATE_ZFUNC, &dwZFunc);
    pDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_NEVER);

    pDevice->GetRenderState(D3DRENDERSTATE_FILLMODE, &dwFill );
    pDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);

    //HRESULT result=

    pDrawIndexedPrimitiveVB(pDevice,d3dptPrimitiveType,lpd3dVertexBuffer,lpwIndices,dwIndexCount,dwFlags);

    pDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, dwZMode);

    pDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,dwZFunc);

    pDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, dwFill);

    //return result;
  }
  return pDrawIndexedPrimitiveVB(pDevice,d3dptPrimitiveType,lpd3dVertexBuffer,lpwIndices,dwIndexCount,dwFlags);
}

HRESULT WINAPI hIDirect3D3_CreateDevice(LPDIRECT3D3 self,REFCLSID classId,LPDIRECTDRAWSURFACE4 surface,LPDIRECT3DDEVICE3* device,LPUNKNOWN unkouter){
  DbgPrint("*** Hooked CreateDevice %X! ***\n",device);

  HRESULT result=pIDirect3D3_CreateDevice(self,classId,surface,device,unkouter);

  DbgPrint("BeginScene %X\n",GetInterfaceMethod(*device,9));
  pBeginScene=(tBeginScene)ReplaceFunction(GetInterfaceMethod(*device,9),(PVOID)hBeginScene);

  DbgPrint("EndScene %X\n",GetInterfaceMethod(*device,10));
  pEndScene=(tEndScene)ReplaceFunction(GetInterfaceMethod(*device,10),(PVOID)hEndScene);

  /*DbgPrint("DrawPrimitive %X\n",GetInterfaceMethod(*device,28));
  pDrawPrimitive=(tDrawPrimitive)ReplaceFunction(GetInterfaceMethod(*device,28),(PVOID)hDrawPrimitive);

  DbgPrint("DrawIndexedPrimitive %X\n",GetInterfaceMethod(*device,29));
  pDrawIndexedPrimitive=(tDrawIndexedPrimitive)ReplaceFunction(GetInterfaceMethod(*device,29),(PVOID)hDrawIndexedPrimitive);

  DbgPrint("DrawPrimitiveStrided %X\n",GetInterfaceMethod(*device,32));
  pDrawPrimitiveStrided=(tDrawPrimitiveStrided)ReplaceFunction(GetInterfaceMethod(*device,32),(PVOID)hDrawPrimitiveStrided);

  DbgPrint("DrawIndexedPrimitiveStrided %X\n",GetInterfaceMethod(*device,33));
  pDrawIndexedPrimitiveStrided=(tDrawIndexedPrimitiveStrided)ReplaceFunction(GetInterfaceMethod(*device,33),(PVOID)hDrawIndexedPrimitiveStrided);

  DbgPrint("DrawPrimitiveVB %X\n",GetInterfaceMethod(*device,34));
  pDrawPrimitiveVB=(tDrawPrimitiveVB)ReplaceFunction(GetInterfaceMethod(*device,34),(PVOID)hDrawPrimitiveVB);*/

  DbgPrint("DrawIndexedPrimitiveVB %X\n",GetInterfaceMethod(*device,35));
  pDrawIndexedPrimitiveVB=(tDrawIndexedPrimitiveVB)ReplaceFunction(GetInterfaceMethod(*device,35),(PVOID)hDrawIndexedPrimitiveVB);

  return result;
}

DWORD STDCALL HookDirect3D(LPVOID a){
  HMODULE hDDraw=NULL;
  while((hDDraw=GetModuleHandle("ddraw.dll"))==NULL)Sleep(50);
  //DbgPrint("DDRAW %X\n",hDDraw);
  tDirectDrawCreate pDirectDrawCreate=(tDirectDrawCreate)GetProcAddress(hDDraw,"DirectDrawCreate");
  if(pDirectDrawCreate){
    LPDIRECTDRAW ddc=NULL;
    pDirectDrawCreate(NULL,&ddc,NULL);
    if(ddc){
      IDirectDraw4* dd4=NULL;
      ddc->QueryInterface(IID_IDirectDraw4,(PVOID*)&dd4);
      if(dd4){
        IDirect3D3 *d3d3=NULL;
        dd4->QueryInterface(IID_IDirect3D3,(PVOID*)&d3d3);
        if(d3d3){
          //DbgPrint("d3d3->CreateDevice %X\n",GetInterfaceMethod(d3d3,8));
          pIDirect3D3_CreateDevice=(tIDirect3D3_CreateDevice)ReplaceFunction(GetInterfaceMethod(d3d3,8),(PVOID)hIDirect3D3_CreateDevice);
          d3d3->Release();
        }else
          DbgPrint("No D3D3!!!\n");
      }else
        DbgPrint("No DD4!!!\n");
    }else
      DbgPrint("No DDC!!!\n",ddc);
  }else
    DbgPrint("No DirectDrawCreate!!!\n");
  return 0;
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
  if(fdwReason==DLL_PROCESS_ATTACH){
    DbgPrint("Dll Injected!!!\n");
    CreateThread(NULL,0,HookDirect3D,NULL,0,NULL);
  }
  return TRUE; // succesful
}

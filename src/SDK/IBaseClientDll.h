#pragma once

#include "sdk.h"
#include "../tools/virtuals.h"

class DVariant;
class CRecvProxyData;
class RecvProp;
class RecvTable;

typedef void*(*CreateClientClassFn)(int entnum, int serialNum);
typedef void*(*CreateEventFn)();

typedef void(*RecvVarProxyFn)(const CRecvProxyData *pData, void *pStruct, void *pOut);
typedef void(*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID);
typedef void(*ArrayLengthRecvProxyFn)(void *pStruct, int objectID, int currentArrayLength);

typedef enum
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_String,
	DPT_Array,	// An array of the base types (can't be of datatables).
	DPT_DataTable
} SendPropType;

class DVariant
{
public:
	union
	{
		float	m_Float;
		long	m_Int;
		char	*m_pString;
		void	*m_pData;
		float	m_Vector[3];
		long long	m_Int64;
	};

	SendPropType m_Type;
};

class CRecvProxyData
{
public:
	const RecvProp	*m_pRecvProp;
	DVariant		m_Value;
	int				m_iElement;
	int				m_ObjectID;
};

class RecvProp
{
public:
	int GetFlags() const
	{
		return m_Flags;
	}

	const char* GetName() const
	{
		return m_pVarName;
	}

	int GetType() const
	{
		return m_RecvType;
	}

	RecvTable* GetDataTable() const
	{
		return m_pDataTable;
	}

	RecvVarProxyFn GetProxyFn() const
	{
		return m_ProxyFn;
	}

	void SetProxyFn(RecvVarProxyFn fn)
	{
		m_ProxyFn = fn;
	}

	int GetOffset() const
	{
		return m_Offset;
	}

	void SetOffset(int o)
	{
		m_Offset = o;
	}

public:
	char					*m_pVarName;
	int						m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;

	bool					m_bInsideArray;

	const void*				m_pExtraData;

	RecvProp				*m_pArrayProp;
	ArrayLengthRecvProxyFn	m_ArrayLengthProxy;

	RecvVarProxyFn			m_ProxyFn;
	DataTableRecvVarProxyFn	m_DataTableProxyFn;

	RecvTable				*m_pDataTable;
	int						m_Offset;

	int						m_ElementStride;
	int						m_nElements;

	const char				*m_pParentArrayPropName;
};

class RecvTable
{
public:
	int GetNumProps()
	{
		return m_nProps;
	}

	RecvProp* GetRecvProp(int i)
	{
		return &m_pProps[i];
	}

	const char* GetName()
	{
		return m_pNetTableName;
	}

public:
	RecvProp		*m_pProps;
	int				m_nProps;

	void*			m_pDecoder;

	char			*m_pNetTableName;

	bool			m_bInitialized;
	bool			m_bInMainList;
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

struct ClientClass
{
	CreateClientClassFn		m_pCreateFn;
	CreateEventFn			m_pCreateEventFn;
	char					*m_pNetworkName;
	RecvTable				*m_pRecvTable;
	ClientClass				*m_pNext;
	int						m_ClassID;

	const char* GetName()
	{
		return m_pNetworkName;
	}
};

class IBaseClientDll
{
public:
	ClientClass* GetAllClasses()
	{
		typedef ClientClass* (__thiscall* GetAllClassesFn)(void*);
		return get_vfunc<GetAllClassesFn>(this, 8)(this);
	}
};
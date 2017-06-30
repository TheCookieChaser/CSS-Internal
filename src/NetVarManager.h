#pragma once

#include "SDK/SDK.h"
#include "FnvHash.h"
#include <vector>
#include <iomanip>
#include <fstream>

#pragma warning(disable : 4996)

class CNetVarManager
{
public:
	void Initialize();
	int GetOffset(const char *tableName, const char *propName);
	bool HookProp(const char *tableName, const char *propName, RecvVarProxyFn fun);
	void DumpNetvars();
private:
	int Get_Prop(const char *tableName, const char *propName, RecvProp **prop = 0);
	int Get_Prop(RecvTable *recvTable, const char *propName, RecvProp **prop = 0);
	RecvTable *GetTable(const char *tableName);
	std::vector<RecvTable*> m_tables;
	void DumpTable(RecvTable *table, int depth);
	std::ofstream m_file;
};

extern CNetVarManager* NetVarManager;
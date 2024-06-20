#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class InsertGold : public DBBind<0,0>
    {
    public:
    	InsertGold(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spInsertGold}") { }

    private:
    };

    class GetGold : public DBBind<0,3>
    {
    public:
    	GetGold(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetGold}") { }
    	void Out_ID(OUT int32& v) { BindCol(0, v); };
    	void Out_Gold(OUT int32& v) { BindCol(1, v); };
    	template<int32 N> void Out_Name(OUT WCHAR(&v)[N]) { BindCol(2, v); };

    private:
    };


     
};
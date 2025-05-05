
#include "xserror.h"

char* getErrorString(int errorCode)
{
	switch( errorCode )
	{
		case VE_SUCCESS:			return	"SUCCESS";
		case VE_API_APINAMETOOLONG:	return	"API_APINAMETOOLONG";
		case VE_API_NOT_FOUND:		return	"API_NOT_FOUND";
		case VE_STACK_OVERFLOW:		return	"STACK_OVERFLOW";
		case VE_STACK_ERROR:		return	"STACK_ERROR";
		case VE_STACK_NOTNUMBER:	return	"STACK_NOTNUMBER";
		case VE_STACK_NOTSTRING:	return	"STACK_NOTSTRING";
		case VE_STACK_NOTOBJECT:	return	"STACK_NOTOBJECT";
		case VE_INVALID_LAYER:		return	"INVALID_LAYER";
		case VE_DIV_ZERO:			return	"DIV_ZERO";
		case VE_INVALID_NUMBER:		return	"INVALID_NUMBER";
	
		case VE_INVALID_PARAMCOUNT:	return	"INVALID_PARAMCOUNT";

		case VE_VAR_NOTDEFINED:		return	"VAR_NOTDEFINED";
		case VE_INVALID_VARNAME:	return  "INVALID_VARNAME";
		case VE_INVALID_ARGS:		return	"INVALID_ARGS";
		case VE_MISSING_LEFTBRACE:	return	"MISSING_LEFTBRACE";
		case VE_MISSING_RBRACE:		return	"MISSING_RBRACE";
		case VE_MISSING_LBRACKETS:	return	"MISSING_LBRACKETS";
		case VE_MISSING_SEMICOLON:	return	"MISSING_SEMICOLON";
		case VE_END_OF_FILE:		return	"END_OF_FILE";

		default:					return	"UNKNOWN";
	}
}

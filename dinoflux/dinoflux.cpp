#include "dwHeaders.h"
#include "dinoflux.h"

#include <sys/time.h>
#include <iconv.h>



/**
 * CDinoflux data members initialization
 */

int CDinoflux::m_nDataPool = -1;
string CDinoflux::m_strCuckooHost = "5.9.23.53"; 
list<string> CDinoflux::m_lstExts;

/**
 *	InitService called from c-icap for initialize service data
 *	\param srv_xdata - not used
 *	\param server_conf - not used
 *	\return c-icap return values
 */
int CDinoflux::InitService( ci_service_xdata_t * srv_xdata, struct ci_server_conf *server_conf )
{
	//Example that shows how to initialize object pool for a C++ class
	//m_nDataPool = ci_object_pool_register( const_cast<char*>("Drainware_Skel_Data"), sizeof(CUserData) );
	
	m_nDataPool = ci_object_pool_register( const_cast<char*>("Dinoflux_Data"), sizeof(ci_membuf_t*) );

	if( m_nDataPool < 0 )
		return CI_ERROR;

	//ci_service_set_xopts( srv_xdata, CI_XCLIENTIP | CI_XAUTHENTICATEDUSER );

	return CI_OK;
}

/**
 *	PostInitService called from c-icap after the configuration was loaded, used for init AC
 *	\param srv_xdata - not used
 *	\param server_conf - not used
 *	\return c-icap return values
 */

int CDinoflux::PostInitService( ci_service_xdata_t * srv_xdata, struct ci_server_conf *server_conf )
{
	return CI_OK;
}

/**
 *	CloseService called from c-icap for free allocated resources
 */
void CDinoflux::CloseService()
{
	ci_object_pool_unregister( m_nDataPool );
}

/**
 *	InitRequestData called from c-icap for initialize data associated to the request
 *	\param pReq - c-icap request structure
 */
void *CDinoflux::InitRequestData( ci_request_t * pRequest )
{
	//Example of initialization of a C++ class for memory given by i-cap
	//CUserData *pData = reinterpret_cast<CUserData*>(ci_object_pool_alloc( m_nDataPool ));
	//new(pData) CUserData();

	ci_membuf_t **ppData = reinterpret_cast<ci_membuf_t**>(ci_object_pool_alloc( m_nDataPool ));
	*ppData = NULL;

	http_authenticate( pRequest, NULL ); //Force i-cap to obtain the user name, it'll be available at pRequest->user

	return reinterpret_cast<void*>(ppData);
}

/**
 *	InitRequestData called from c-icap for free allocated data for a request
 *	\param pVoid Previously allocated data
 */
void CDinoflux::ReleaseData( void *pVoid )
{
	//Example for free a C++ class
	//CUserData *pData = reinterpret_cast<CUserData *>(pVoid);
	//pData->~CUserData();

	ci_membuf_t **ppData = reinterpret_cast<ci_membuf_t**>(pVoid);
	if( *ppData )
		ci_membuf_free( *ppData );
	ci_object_pool_free( pVoid );
}

/**
 *	RetAllow204 Check if request has a body and return CI_MOD_ALLOW204
 *	\param pRequest Request pointer
 *	\param pData user asociated data
 */
int CDinoflux::Allow204( ci_request_t *pRequest, ci_membuf_t **ppMemBuf )
{
	if( ci_req_hasbody( pRequest ) )
		*ppMemBuf = ci_membuf_new_sized( ci_http_content_length( pRequest ) + 256 );
	return CI_MOD_ALLOW204;
}

/**
 *	GetResource Obtains the requested resource, that is, the string located between "GET" and "HTTP/1.X"
 *	\param pRequest c-icap 
 *	\param szRes Output buffer
 *	\param nSize Output buffer size
 *	\param bRetQuery true if want to copy query part ex '?param1=0'
 */
void CDinoflux::GetResource( ci_request_t * pRequest, char *szRes, size_t nSize, bool bRetQuery )
{
    szRes[ 0 ] = 0;
    ci_headers_list_t *pReqHeaders = ci_http_request_headers( pRequest );
    size_t nLast = nSize - 1;
    if( pReqHeaders )
    {
        char *pHttp = ci_http_request( pRequest );
        //ci_debug_printf( 2, "GetResource from: %s\n", pHttp );
        if( !pHttp )
        {
            ci_debug_printf( 2, "No Http Headers!!!!\n" );
            return;
        }

        while( *pHttp != ' ' && *pHttp )
            ++pHttp;
        if( !*pHttp )
        {
            ci_debug_printf( 2, "Bad Http Header!!!!\n" );
            return;
        }
        int nIndex = 0;
        ++pHttp;
        if( bRetQuery )
        {
            while( *pHttp && *pHttp != ' ' && nIndex < nLast )
                szRes[ nIndex++ ] = *pHttp, ++pHttp;
        }
        else
        {
            while( *pHttp && *pHttp != ' ' && *pHttp != '?' && nIndex < nLast )
                szRes[ nIndex++ ] = *pHttp, ++pHttp;
        }
        szRes[ nIndex ] = 0;
    }
    else
    {
        ci_debug_printf( 2, "No Headers!!!!!\n" );
    }
}

/**
 *	CheckContentType Check if the content type is supported for parse with AC
 *	\param szContentType string with the content type
 *	\return true for supported types, false for unsupported
 */

static int ToLower( int n )
{
    return tolower( n );
}


bool CDinoflux::CheckExtension( const char *szRes )
{
    string strRes = szRes;
    transform( strRes.begin(), strRes.end(), strRes.begin(), ToLower );
    for( list<string>::iterator it = m_lstExts.begin(); it != m_lstExts.end(); it++ )
    {
        size_t nExtLength = it->length();
        if( nExtLength + 1 > strRes.length() && !it->compare( strRes.c_str() + strRes.length() - nExtLength )
                && strRes[ strRes.length() - nExtLength - 1 ] == '.' )
            return true;
    }
    return false;
}


/**
 *	CheckPreview called from c-icap whith the request headers, befor get data from server
 *	\param pPreviewData not used
 *	\param nPreviewDataLen not used
 *	\param pReq The c-icap request where the headers arrives
 *	\return c-icap return values
 */
int CDinoflux::CheckPreview( char *pPreviewData, int nPreviewDataLen, ci_request_t * pReq )
{
	//Example for get C++ class from service data
	//CUserData *pData = reinterpret_cast<CUserData *>(ci_service_data( pReq ));

	//ci_headers_list_t *pReqHeaders = ci_http_request_headers( pReq ); //Not necesary for response checks
	ci_membuf_t **ppData = reinterpret_cast<ci_membuf_t**>(ci_service_data( pReq));

	int nRet = CI_OK;

	char szResource[ 4096 ] = "";
	GetResource( pReq, szResource, 4096 );

	if( !ppData )
		return CI_ERROR;

	ci_headers_list_t *pResponseHeaders = ci_http_response_headers( pReq);


	if( pResponseHeaders )
	{
		//Write your code here
		//For Check IO -> return CI_MOD_CONTINUE;
		//If some serious error occurs -> return CI_ERROR;

		if(CheckExtension( szResource ))
		{
			nRet = CI_MOD_CONTINUE;
		}



	}
	

	if(!ci_req_hasbody( pReq ) )
		return Allow204( pReq, ppData ); //Allow to continue


	return nRet;

}

/**
 *	CheckProcess Not implemented
 *	\param pReq Not used
 *	\return c-icap return values
 */
int CDinoflux::CheckProcess( ci_request_t * pReq )
{

	/*
	//ci_debug_printf( 2, "CDWFilter::CheckProcess\n" );
	//return CI_MOD_ALLOW204;
	CUserData *pData = reinterpret_cast<CUserData *>(ci_service_data( pReq ));
	if( !pData )
		return CI_ERROR;
	if( pData->m_bAllow )
		return CI_MOD_ALLOW204;
	return CI_MOD_DONE;
	*/

	return CI_MOD_DONE;
}

/**
 *	CheckIO Called by c-icap with the read or writed data of the request/response, used for filter data
 *	\param pWriteBuffer Buffer where to write result
 *	\param nWriteLen Size of the Buffer to write
 *	\param pReadBuffer Buffer with the response data
 *	\param nReadLen Size of the Read Buffer
 *	\param iseof Tell when is the final chunk
 *	\param pRequest c-icap request data
 *	\return c-icap return values
 */
int CDinoflux::CheckIO( char *pWriteBuffer, int *nWriteLen, char *pReadBuffer, int *nReadLen, int iseof, ci_request_t * pRequest )
{
	ci_membuf_t **ppData = reinterpret_cast<ci_membuf_t**>(ci_service_data( pRequest ));
	if( !ppData || *ppData )
		return CI_ERROR;
	
	if( pReadBuffer && nReadLen )
	{
		*nReadLen = ci_membuf_write( *ppData, pReadBuffer, *nReadLen, iseof );
		if( *nReadLen == CI_ERROR )
			return CI_ERROR;
		if( iseof ) //Parse when received all 'chunks'
		{
			//Here you can check all content saved in *ppData
		}
	}

	if( pWriteBuffer && nWriteLen )
	{
		*nWriteLen = ci_membuf_read( *ppData, pWriteBuffer, *nWriteLen );
		if( *nWriteLen == CI_ERROR )
			return CI_ERROR;
	}

	return CI_OK;
}

/**
 *	DinofluxConfig get cuckoo host from dinoflux.conf
 *
 *	\param szDirective "DinofluxConfig"
 *	\param argv list of arguments
 *	\return c-icap return values
 */

int CDinoflux::DinofluxConfig( char *szDirective, char **argv, void *pData )
{
	//Here you can save the contents of argv
	
	if(strlen(argv[ 0 ])> 0)
	  m_strCuckooHost = (argv[ 0 ]);

	return CI_OK;
}



/**
 *      DinofluxConfig get extensions from dnoflux.conf
 *
 *      \param szDirective "ExtensionsConfig"
 *      \param argv list of arguments
 *      \return c-icap return values
 */

int CDinoflux::ExtensionsConfig( char *szDirective, char **argv, void *pData )
{
        //Here you can save the contents of argv

        if(strlen(argv[ 0 ])> 0){
          string tempstr = (argv[ 0 ]);
	  istringstream ss(tempstr);
	  string token;
	  while(std::getline(ss, token, ',')) {
	    m_lstExts.push_back(token);
	  }
	}
        return CI_OK;
}



/**
 *	ConfigurationVariables c-icap structure for load configuration file
 */
struct ci_conf_entry CDinoflux::ConfigurationVariables[] = {
  {const_cast<char*>("DinofluxConfig"), NULL, CDinoflux::DinofluxConfig, NULL},
  {const_cast<char*>("ExtensionsConfig"), NULL, CDinoflux::ExtensionsConfig, NULL},
  {NULL, NULL, NULL, NULL}
};



/**
 *	c-icap service definition with names and callback functions
 */
extern "C"
{
CI_DECLARE_MOD_DATA ci_service_module_t service = {
     const_cast<char*>("dinoflux"),
     const_cast<char*>("dinoflux sample reporter"),
     ICAP_RESPMOD, //ICAP_RESPMOD,
	CDinoflux::InitService, /* init_service */
	CDinoflux::PostInitService, /*post_init_service */
	CDinoflux::CloseService,/*close_Service */
	CDinoflux::InitRequestData,/* init_request_data */
	CDinoflux::ReleaseData,    /*Release request data */
	CDinoflux::CheckPreview,
	CDinoflux::CheckProcess,
	CDinoflux::CheckIO,
	CDinoflux::ConfigurationVariables,
	NULL };
}//extern "C"


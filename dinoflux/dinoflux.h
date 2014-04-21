#ifndef __DINOFLUX_H__
#define __DINOFLUX_H__

/**
 *	CSkel Basic skeletom for implement a c-icap service
 *	
 */

class CDinoflux
{
public:
	//c-icap service configuration & callbacks
	static int InitService( ci_service_xdata_t * srv_xdata, struct ci_server_conf *server_conf );
	static int PostInitService( ci_service_xdata_t * srv_xdata, struct ci_server_conf *server_conf );
	static void CloseService();
	
	static void *InitRequestData( ci_request_t * pReq );
	static void ReleaseData( void *pData );

	static void GetResource(ci_request_t * pRequest, char *szRes, size_t nSize, bool bRetQuery=false);


	static int CheckPreview( char *pPreviewData, int nPreviewDataLen, ci_request_t * pReq );
	static int CheckProcess( ci_request_t * pReq );
	static int CheckIO( char *wbuf, int *wlen, char *rbuf, int *rlen, int iseof, ci_request_t * pReq );

	//Configuration
	static int DinofluxConfig( char *szDirective, char **argv, void *pData );
	static int ExtensionsConfig( char *szDirective, char **argv, void *pData );
	static ci_conf_entry ConfigurationVariables[];

	static list<string> m_lstExts;


private:
	//Service Data
	static int m_nDataPool; //Identifier for data pool assigned by c-icap
	static string m_strCuckooHost; //Cuckoo hostname
	//Helper Function
	static bool CheckExtension( const char *szRes );
	static int Allow204( ci_request_t *pRequest, ci_membuf_t **ppMemBuf );
};


#endif//__DRAINWARE_SKEL_H__


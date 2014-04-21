#ifndef __DW_SKEL_HEADERS_H__
#define __DW_SKEL_HEADERS_H__

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <list>
#include <sstream>
#include <algorithm>



using namespace std;

//C-ICAP needed headers
extern "C"
{
#include "c-icap.h"
#include "service.h"
#include "header.h"
#include "body.h"
#include "simple_api.h"
#include "lookup_table.h"
#include "debug.h"
#include "access.h"
#include "acl.h"
#include "../../common.h"
#include "commands.h"
}//extern "C"

#endif //__DW_SKEL_HEADERS_H__


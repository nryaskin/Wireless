#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "zb_secur.h"
#include "zb_secur_api.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif

#ifndef ZB_SECURITY
#error Define ZB_SECURITY
#endif


  


zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08};
zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};

#define B_ON           1
#define B_OFF          2
#define B_LEVEL_UP     4
#define B_LEVEL_DOWN   8
#define B_SET_LEVEL   16
#define B_TOGGLE      32

/*
  The test is: ZC starts PAN, ZR joins to it by association and send APS data packet, when ZC
  received packet, it sends packet to ZR, when ZR received packet, it sends
  packet to ZC etc.
 */

void data_indication(zb_uint8_t param) ZB_CALLBACK;

MAIN()
{
  ARGV_UNUSED;

  
#ifndef KEIL
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif
  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;
  zb_secur_setup_preconfigured_key(g_key, 0);

  if (zdo_dev_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "zdo_dev_start failed", (FMT__0));
  }
  else
  {
    zdo_main_loop();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}



void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_af_set_data_indication(data_indication);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
  zb_free_buf(buf);
}


void _Bulb_On(){
    TRACE_MSG(TRACE_ERROR, "BULB ON", (FMT__0));
}


void _Bulb_Off(){
    TRACE_MSG(TRACE_APS2, "BULB Off", (FMT__0));
}

void _Bulb_Level(zb_uint8_t param){
    TRACE_MSG(TRACE_APS2, "BULB change level %d", (FMT__D, param));
}

void _Bulb_Level_Up() {
    TRACE_MSG(TRACE_APS2, "BULB level up", (FMT__0));
}

void _Bulb_Level_Down() {
    TRACE_MSG(TRACE_APS2, "BULB level down", (FMT__0));
}


void _Bulb_Toggle() {
    TRACE_MSG(TRACE_APS2, "BULB toggle", (FMT__0));
}
/*
   Trivial test: dump all APS data received
 */


void data_indication(zb_uint8_t param) ZB_CALLBACK
{
    zb_uint8_t *ptr, level;
    zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);
    
    
    ZB_APS_HDR_CUT_P(asdu, ptr);
    switch ((int)ptr[0]) {
        case  B_ON       :
             _Bulb_On();
             break;
        case B_OFF       :
             _Bulb_Off();
             break;
        case B_LEVEL_UP  :
             _Bulb_Level_Up();
             break;    
	case B_LEVEL_DOWN:
	     _Bulb_Level_Down();
	     break;
	case B_SET_LEVEL :
	     level = (zb_uint8_t)ptr[1];
	     _Bulb_Level(level);
	     break;
	case B_TOGGLE    :
             _Bulb_Toggle();
	     break;
        default:         
             TRACE_MSG(TRACE_APS2, "NOT IMPLEMENTED YET", (FMT__0));
             break;
    }
        
    /* send packet back to ZR */
    //zc_send_data(asdu, ind->src_addr);

}

#ifndef APS_RETRANSMIT_TEST
/*static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr)
{
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr = NULL;
  
    ZB_BUF_INITIAL_ALLOC(buf, 1, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    req->dst_addr.addr_short = addr;*/ /* send to ZR */
/*    req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
    req->radius = 1;
    req->profileid = 2;
    req->src_endpoint = 10;
    req->dst_endpoint = 10;  
    buf->u.hdr.handle = 0x11;  
    ptr[0] = 'L';*/
    /*for (i = 0 ; i < ZB_TEST_DATA_SIZE ; ++i)
    {
     // ptr[i] = i % 32 + '0';
        ptr[i] = str[i % 6];
    }*/
/*    TRACE_MSG(TRACE_APS2, "Sending apsde_data.request", (FMT__0));  
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
  }*/
#endif

/*! @} */

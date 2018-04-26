/***************************************************************************
*                      ZBOSS ZigBee Pro 2007 stack                         *
*                                                                          *
*          Copyright (c) 2012 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*          Copyright (c) 2011 ClarIDy Solutions, Inc., Taipei, Taiwan.     *
*                       http://www.claridy.com/                            *
*                                                                          *
*          Copyright (c) 2011 Uniband Electronic Corporation (UBEC),       *
*                             Hsinchu, Taiwan.                             *
*                       http://www.ubec.com.tw/                            *
*                                                                          *
*          Copyright (c) 2011 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*                                                                          *
*                                                                          *
* ZigBee Pro 2007 stack, also known as ZBOSS (R) ZB stack is available     *
* under either the terms of the Commercial License or the GNU General      *
* Public License version 2.0.  As a recipient of ZigBee Pro 2007 stack, you*
* may choose which license to receive this code under (except as noted in  *
* per-module LICENSE files).                                               *
*                                                                          *
* ZBOSS is a registered trademark of DSR Corporation AKA Data Storage      *
* Research LLC.                                                            *
*                                                                          *
* GNU General Public License Usage                                         *
* This file may be used under the terms of the GNU General Public License  *
* version 2.0 as published by the Free Software Foundation and appearing   *
* in the file LICENSE.GPL included in the packaging of this file.  Please  *
* review the following information to ensure the GNU General Public        *
* License version 2.0 requirements will be met:                            *
* http://www.gnu.org/licenses/old-licenses/gpl-2.0.html.                   *
*                                                                          *
* Commercial Usage                                                         *
* Licensees holding valid ClarIDy/UBEC/DSR Commercial licenses may use     *
* this file in accordance with the ClarIDy/UBEC/DSR Commercial License     *
* Agreement provided with the Software or, alternatively, in accordance    *
* with the terms contained in a written agreement between you and          *
* ClarIDy/UBEC/DSR.                                                        *
*                                                                          *
****************************************************************************
PURPOSE:
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "zb_secur.h"
#include "zb_secur_api.h"

#ifndef ZB_ROUTER_ROLE
#error Router role is not compiled!
#endif

#ifndef ZB_SECURITY
#error Define ZB_SECURITY
#endif


/*! \addtogroup ZB_TESTS */
/*! @{ */

/*
  Bulb commands
 */
static void set_bulb_on_command(zb_uint8_t param) ZB_CALLBACK;
static void set_bulb_off_command(zb_uint8_t param) ZB_CALLBACK;
static void set_bulb_level_up_command(zb_uint8_t param) ZB_CALLBACK;
static void set_bulb_level_down_command(zb_uint8_t param) ZB_CALLBACK;
static void set_bulb_level_command(zb_uint8_t param) ZB_CALLBACK;
static void set_bulb_toggle(zb_uint8_t param) ZB_CALLBACK;
#define B_ON           1
#define B_OFF          2
#define B_LEVEL_UP     4
#define B_LEVEL_DOWN   8
#define B_SET_LEVEL   16
#define B_TOGGLE      32

/*
  ZR joins to ZC, then sends APS packet.
 */

#define GENERATE_REQ(X) 


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
  ZB_INIT("zdo_zr", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr", "2", "2");
#endif


  /* FIXME: temporary, until neighbor table is not in nvram */
  /* add extended address of potential parent to emulate that we've already
   * been connected to it */
  {
    zb_ieee_addr_t ieee_address;
    zb_address_ieee_ref_t ref;

    ZB_64BIT_ADDR_ZERO(ieee_address);
    ieee_address[7] = 8;

    zb_address_update(ieee_address, 0, ZB_FALSE, &ref);
  }

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
  zb_buf_t *buf1 = zb_get_out_buf();
  zb_buf_t *buf2 = zb_get_out_buf();
  zb_buf_t *buf3 = zb_get_out_buf();
  zb_buf_t *buf4 = zb_get_out_buf();
  zb_buf_t *buf5 = zb_get_out_buf();
  zb_buf_t *buf6 = zb_get_out_buf();
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));

    ZB_SCHEDULE_ALARM(set_bulb_on_command, ZB_REF_FROM_BUF(buf1), ZB_TIME_ONE_SECOND );
    ZB_SCHEDULE_ALARM(set_bulb_off_command,  ZB_REF_FROM_BUF(buf2), ZB_TIME_ONE_SECOND * 3);
    ZB_SCHEDULE_ALARM(set_bulb_toggle,  ZB_REF_FROM_BUF(buf3), ZB_TIME_ONE_SECOND * 3);
    ZB_SCHEDULE_ALARM(set_bulb_level_up_command,  ZB_REF_FROM_BUF(buf4), ZB_TIME_ONE_SECOND * 5);
    ZB_SCHEDULE_ALARM(set_bulb_level_down_command,  ZB_REF_FROM_BUF(buf5), ZB_TIME_ONE_SECOND * 8);
    *ZB_GET_BUF_PARAM(buf6, zb_uint8_t) = 100;
    ZB_SCHEDULE_ALARM(set_bulb_level_command,  ZB_REF_FROM_BUF(buf6), ZB_TIME_ONE_SECOND * 12);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}

void fill_req_fields(zb_apsde_data_req_t *req) {
    req->dst_addr.addr_short = 0;
    req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
    req->radius = 1;
    req->profileid = 2;
    req->src_endpoint = 10;
    req->dst_endpoint = 10;
}

void set_bulb_on_command(zb_uint8_t param) {
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr;
    zb_buf_t *buf = (zb_buf_t *) ZB_BUF_FROM_REF(param);
    
    ZB_BUF_INITIAL_ALLOC(buf, 1, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    fill_req_fields(req);

    ptr[0] = B_ON;  
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf)); 
}

void set_bulb_off_command(zb_uint8_t param) {
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr;
    zb_buf_t *buf = (zb_buf_t *) ZB_BUF_FROM_REF(param);

    ZB_BUF_INITIAL_ALLOC(buf, 1, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    fill_req_fields(req);

    ptr[0] = B_OFF;
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

void set_bulb_level_up_command(zb_uint8_t param) {
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr;
    zb_buf_t *buf = (zb_buf_t *) ZB_BUF_FROM_REF(param);

    ZB_BUF_INITIAL_ALLOC(buf, 1, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    fill_req_fields(req);

    ptr[0] = B_LEVEL_UP;
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

void set_bulb_level_down_command(zb_uint8_t param) {
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr;
    zb_buf_t *buf = (zb_buf_t *) ZB_BUF_FROM_REF(param);

    ZB_BUF_INITIAL_ALLOC(buf, 1, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    fill_req_fields(req);

    ptr[0] = B_LEVEL_DOWN;
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

void set_bulb_toggle(zb_uint8_t param) {
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr;
    zb_buf_t *buf = (zb_buf_t *) ZB_BUF_FROM_REF(param);

    ZB_BUF_INITIAL_ALLOC(buf, 1, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    fill_req_fields(req);

    ptr[0] = B_TOGGLE;
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

#define LEVEL 10

void set_bulb_level_command(zb_uint8_t param) {
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr;
    zb_buf_t *buf = (zb_buf_t *) ZB_BUF_FROM_REF(param);
    zb_uint8_t lvl = *ZB_GET_BUF_PARAM(buf,zb_uint8_t);

    ZB_BUF_INITIAL_ALLOC(buf, 2, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    fill_req_fields(req);

    ptr[0] = B_SET_LEVEL;
    ptr[1] = lvl;
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

/*! @} */

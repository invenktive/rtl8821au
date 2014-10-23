/******************************************************************************
 *
 * Copyright(c) 2007 - 2013 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef __OSDEP_SERVICE_H_
#define __OSDEP_SERVICE_H_


#define _FAIL		0
#define _SUCCESS	1
#define RTW_RX_HANDLED 2
//#define RTW_STATUS_TIMEDOUT -110

#undef _TRUE
#define _TRUE		1

#undef _FALSE
#define _FALSE		0


#ifdef PLATFORM_LINUX
#include <osdep_service_linux.h>
#endif

#define RTW_TIMER_HDL_NAME(name) rtw_##name##_timer_hdl
#define RTW_DECLARE_TIMER_HDL(name) void RTW_TIMER_HDL_NAME(name)(RTW_TIMER_HDL_ARGS)


#define BIT0	0x00000001
#define BIT1	0x00000002
#define BIT2	0x00000004
#define BIT3	0x00000008
#define BIT4	0x00000010
#define BIT5	0x00000020
#define BIT6	0x00000040
#define BIT7	0x00000080
#define BIT8	0x00000100
#define BIT9	0x00000200
#define BIT10	0x00000400
#define BIT11	0x00000800
#define BIT12	0x00001000
#define BIT13	0x00002000
#define BIT14	0x00004000
#define BIT15	0x00008000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000
#define BIT32	0x0100000000
#define BIT33	0x0200000000
#define BIT34	0x0400000000
#define BIT35	0x0800000000
#define BIT36	0x1000000000

extern int RTW_STATUS_CODE(int error_code);

#define CONFIG_USE_VMALLOC

//flags used for rtw_update_mem_stat()
enum {
	MEM_STAT_VIR_ALLOC_SUCCESS,
	MEM_STAT_VIR_ALLOC_FAIL,
	MEM_STAT_VIR_FREE,
	MEM_STAT_PHY_ALLOC_SUCCESS,
	MEM_STAT_PHY_ALLOC_FAIL,
	MEM_STAT_PHY_FREE,
	MEM_STAT_TX, //used to distinguish TX/RX, asigned from caller
	MEM_STAT_TX_ALLOC_SUCCESS,
	MEM_STAT_TX_ALLOC_FAIL,
	MEM_STAT_TX_FREE,
	MEM_STAT_RX, //used to distinguish TX/RX, asigned from caller
	MEM_STAT_RX_ALLOC_SUCCESS,
	MEM_STAT_RX_ALLOC_FAIL,
	MEM_STAT_RX_FREE
};

#define rtw_update_mem_stat(flag, sz) do {} while(0)
extern u8*	_rtw_vmalloc(u32 sz);
extern u8*	_rtw_zvmalloc(u32 sz);
extern void	_rtw_vmfree(void *pbuf);
extern u8*	_rtw_zmalloc(u32 sz);
extern u8*	_rtw_malloc(u32 sz);
extern void	_rtw_mfree(void *pbuf);
#ifdef CONFIG_USE_VMALLOC
#define rtw_vmalloc(sz)		_rtw_vmalloc((sz))
#define rtw_zvmalloc(sz)	_rtw_zvmalloc((sz))
#define rtw_vmfree(pbuf)	_rtw_vmfree((pbuf))
#else //CONFIG_USE_VMALLOC
#define rtw_vmalloc(sz)		_rtw_malloc((sz))
#define rtw_zvmalloc(sz)	_rtw_zmalloc((sz))
#define rtw_vmfree(pbuf)	_rtw_mfree((pbuf))
#endif //CONFIG_USE_VMALLOC
#define rtw_malloc(sz)		_rtw_malloc((sz))
#define rtw_zmalloc(sz)		_rtw_zmalloc((sz))
#define rtw_mfree(pbuf)		_rtw_mfree((pbuf))

extern void*	rtw_malloc2d(int h, int w, int size);
extern void	rtw_mfree2d(void *pbuf, int h, int w, int size);

extern int	_rtw_memcmp(void *dst, void *src, u32 sz);

extern void	_rtw_init_queue(_queue	*pqueue);
extern u32	_rtw_queue_empty(_queue	*pqueue);
extern u32	rtw_end_of_queue_search(struct list_head *queue, struct list_head *pelement);

extern u32	rtw_get_current_time(void);
extern u32	rtw_systime_to_ms(u32 systime);
extern u32	rtw_ms_to_systime(u32 ms);
extern int32_t	rtw_get_passing_time_ms(u32 start);
extern int32_t	rtw_get_time_interval_ms(u32 start, u32 end);

extern void	rtw_sleep_schedulable(int ms);

extern void	rtw_msleep_os(int ms);
extern void	rtw_usleep_os(int us);

extern u32 	rtw_atoi(u8* s);

#ifdef DBG_DELAY_OS
#define rtw_mdelay_os(ms) _rtw_mdelay_os((ms), __FUNCTION__, __LINE__)
#define rtw_udelay_os(ms) _rtw_udelay_os((ms), __FUNCTION__, __LINE__)
extern void _rtw_mdelay_os(int ms, const char *func, const int line);
extern void _rtw_udelay_os(int us, const char *func, const int line);
#else
extern void	rtw_udelay_os(int us);
#endif

extern void rtw_yield_os(void);


extern void rtw_init_timer(_timer *ptimer, void *padapter, void *pfunc);


__inline static unsigned char _cancel_timer_ex(_timer *ptimer)
{
#ifdef PLATFORM_LINUX
	return del_timer_sync(ptimer);
#endif
}

static __inline void thread_enter(char *name)
{
#ifdef PLATFORM_LINUX
	allow_signal(SIGTERM);
#endif
}

__inline static void flush_signals_thread(void)
{
#ifdef PLATFORM_LINUX
	if (signal_pending (current))
	{
		flush_signals(current);
	}
#endif
}

__inline static _OS_STATUS res_to_status(sint res)
{

#if defined (PLATFORM_LINUX) || defined (PLATFORM_MPIXEL)
	return res;
#endif


}

__inline static void rtw_dump_stack(void)
{
#ifdef PLATFORM_LINUX
	dump_stack();
#endif
}

__inline static int rtw_bug_check(void *parg1, void *parg2, void *parg3, void *parg4)
{
	int ret = _TRUE;


	return ret;

}

#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)

__inline static u32 _RND4(u32 sz)
{

	u32	val;

	val = ((sz >> 2) + ((sz & 3) ? 1: 0)) << 2;

	return val;

}

__inline static u32 _RND8(u32 sz)
{

	u32	val;

	val = ((sz >> 3) + ((sz & 7) ? 1: 0)) << 3;

	return val;

}

__inline static u32 _RND128(u32 sz)
{

	u32	val;

	val = ((sz >> 7) + ((sz & 127) ? 1: 0)) << 7;

	return val;

}

__inline static u32 _RND256(u32 sz)
{

	u32	val;

	val = ((sz >> 8) + ((sz & 255) ? 1: 0)) << 8;

	return val;

}

__inline static u32 _RND512(u32 sz)
{

	u32	val;

	val = ((sz >> 9) + ((sz & 511) ? 1: 0)) << 9;

	return val;

}

__inline static u32 bitshift(u32 bitmask)
{
	u32 i;

	for (i = 0; i <= 31; i++)
		if (((bitmask>>i) &  0x1) == 1) break;

	return i;
}

#ifndef MAC_FMT
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
#ifndef MAC_ARG
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]
#endif


extern void rtw_suspend_lock_init(void);
extern void rtw_suspend_lock_uninit(void);
extern void rtw_lock_suspend(void);
extern void rtw_unlock_suspend(void);


//File operation APIs, just for linux now
extern int rtw_is_file_readable(char *path);
extern int rtw_retrive_from_file(char *path, u8* buf, u32 sz);
extern int rtw_store_to_file(char *path, u8* buf, u32 sz);

extern u64 rtw_modular64(u64 x, u64 y);
extern u64 rtw_division64(u64 x, u64 y);


/* Macros for handling unaligned memory accesses */

#define RTW_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define RTW_PUT_BE16(a, val)			\
	do {					\
		(a)[0] = ((u16) (val)) >> 8;	\
		(a)[1] = ((u16) (val)) & 0xff;	\
	} while (0)

#define RTW_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define RTW_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((u16) (val)) >> 8;	\
		(a)[0] = ((u16) (val)) & 0xff;	\
	} while (0)

#define RTW_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
			 ((u32) (a)[2]))
#define RTW_PUT_BE24(a, val)					\
	do {							\
		(a)[0] = (uint8_t) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (uint8_t) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[2] = (uint8_t) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define RTW_PUT_BE32(a, val)					\
	do {							\
		(a)[0] = (uint8_t) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[1] = (uint8_t) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[2] = (uint8_t) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[3] = (uint8_t) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_LE32(a) ((((u32) (a)[3]) << 24) | (((u32) (a)[2]) << 16) | \
			 (((u32) (a)[1]) << 8) | ((u32) (a)[0]))
#define RTW_PUT_LE32(a, val)					\
	do {							\
		(a)[3] = (uint8_t) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[2] = (uint8_t) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (uint8_t) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[0] = (uint8_t) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
			 (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
			 (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
			 (((u64) (a)[6]) << 8) | ((u64) (a)[7]))
#define RTW_PUT_BE64(a, val)				\
	do {						\
		(a)[0] = (uint8_t) (((u64) (val)) >> 56);	\
		(a)[1] = (uint8_t) (((u64) (val)) >> 48);	\
		(a)[2] = (uint8_t) (((u64) (val)) >> 40);	\
		(a)[3] = (uint8_t) (((u64) (val)) >> 32);	\
		(a)[4] = (uint8_t) (((u64) (val)) >> 24);	\
		(a)[5] = (uint8_t) (((u64) (val)) >> 16);	\
		(a)[6] = (uint8_t) (((u64) (val)) >> 8);	\
		(a)[7] = (uint8_t) (((u64) (val)) & 0xff);	\
	} while (0)

#define RTW_GET_LE64(a) ((((u64) (a)[7]) << 56) | (((u64) (a)[6]) << 48) | \
			 (((u64) (a)[5]) << 40) | (((u64) (a)[4]) << 32) | \
			 (((u64) (a)[3]) << 24) | (((u64) (a)[2]) << 16) | \
			 (((u64) (a)[1]) << 8) | ((u64) (a)[0]))

void rtw_buf_free(uint8_t **buf, u32 *buf_len);
void rtw_buf_update(uint8_t **buf, u32 *buf_len, uint8_t *src, u32 src_len);

struct rtw_cbuf {
	u32 write;
	u32 read;
	u32 size;
	void *bufs[0];
};

bool rtw_cbuf_full(struct rtw_cbuf *cbuf);
bool rtw_cbuf_empty(struct rtw_cbuf *cbuf);
bool rtw_cbuf_push(struct rtw_cbuf *cbuf, void *buf);
void *rtw_cbuf_pop(struct rtw_cbuf *cbuf);
struct rtw_cbuf *rtw_cbuf_alloc(u32 size);
void rtw_cbuf_free(struct rtw_cbuf *cbuf);

#endif



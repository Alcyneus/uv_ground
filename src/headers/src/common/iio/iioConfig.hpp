#if !defined(_IIO_IIO_CONFIG_HPP_)
#define _IIO_IIO_CONFIG_HPP_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>
#include <iio.h>

#include "debug.hpp"  

#include "uv_config.h"

#if 	_IIO_DEBUG_	
#define IIO_MSG(...) printf(__VA_ARGS__)
#define IIO_DEBUG(...) printf(__VA_ARGS__)
#else
#define IIO_MSG(...)
#define IIO_DEBUG(...)  
#endif


#define IIO_ASSERT(expr) { \
	if (!(expr)) { \
		(void) fprintf(stderr, "IIO_ASSERTion failed (%s:%d)\n", __FILE__, __LINE__); \
		(void) abort(); \
	} \
}

#define  txdevice_name  "iio:device4"
#define  rxdevice_name  "iio:device3"

/* helper macros */
#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))


// typedef struct stream_cfg {
// 	long long bw_hz; // Analog banwidth in Hz
// 	long long fs_hz; // Baseband sample rate in Hz
// 	long long lo_hz; // Local oscillator frequency in Hz
//     long long tx_gain; // Local oscillator frequency in Hz
// 	const char* rfport; // Port name
// };

/* RX is input, TX is output */
enum iodev { RX, TX };

/* static scratch mem for strings */
static char tmpstr[64];

/* check return value of attr_write function */
static void errchk(int v, const char* what) {
	 if (v < 0) 
	 { 
		//fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); 
		IIO_DEBUG("IIO_DEBUG:%sError %d writing to channel \"%s\"\nvalue may not be supported.%s\n",RED, v, what, NONE);
	//  shutdown(); 
	 }
	 
}

/* write attribute: long long int */
static int wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
{
	int revalue = iio_channel_attr_write_longlong(chn, what, val);
	errchk(revalue, what);
	return revalue;
}

/* write attribute: string */
static void wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	errchk(iio_channel_attr_write(chn, what, str), what);
}

/* helper function generating channel names */
static char* get_ch_name(const char* type, int id)
{
	snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
	return tmpstr;
}

/* returns ad9361 phy device */
struct iio_device* get_ad9361_phy(struct iio_context *ctx)
{
	struct iio_device *dev =  iio_context_find_device(ctx, "ad9361-phy");
	IIO_ASSERT(dev && "No ad9361-phy found");
	// if(!dev){IIO_DEBUG("IIO_DEBUG: %sNo ad9361-phy found%s\n", RED, NONE);};
	return dev;
}

/* finds AD9361 streaming IIO devices */
bool get_ad9361_stream_dev(struct iio_context *ctx, enum iodev d, struct iio_device **dev)
{
	switch (d) {
	case TX: *dev = iio_context_find_device(ctx, txdevice_name); return *dev != NULL;
	case RX: *dev = iio_context_find_device(ctx, rxdevice_name);  return *dev != NULL;
	default: IIO_ASSERT(0); return false;
	// default: IIO_DEBUG("IIO_DEBUG: %sIIO type error%s\n", RED, NONE);; return false;
	}
}

/* finds AD9361 streaming IIO channels */
bool get_ad9361_stream_ch(struct iio_context *ctx, enum iodev d, struct iio_device *dev, int chid, struct iio_channel **chn)
{
	*chn = iio_device_find_channel(dev, get_ch_name("voltage", chid), d == TX);
	if (!*chn)
		*chn = iio_device_find_channel(dev, get_ch_name("altvoltage", chid), d == TX);
	return *chn != NULL;
}

/* finds AD9361 phy IIO configuration channel with id chid */
bool get_phy_chan(struct iio_context *ctx, enum iodev d, int chid, struct iio_channel **chn)
{ 
	switch (d) {
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), false); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), true);  return *chn != NULL;
	// default: IIO_ASSERT(0); return false;
	default: IIO_DEBUG("IIO_DEBUG: %sIIO type error%s\n", RED, NONE);; return false;
	}
}

/* finds AD9361 local oscillator IIO configuration channels */
bool get_lo_chan(struct iio_context *ctx, enum iodev d, struct iio_channel **chn)
{
	switch (d) {
	 // LO chan is always output, i.e. true
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 0), true); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 1), true); return *chn != NULL;
	default: IIO_ASSERT(0); return false;
	// default: IIO_DEBUG("IIO_DEBUG: %sIIO type error%s\n", RED, NONE);; return false;
	}
}

/* applies streaming configuration through IIO */
bool cfg_ad9361_streaming_ch(struct iio_context *ctx, struct stream_cfg *cfg, enum iodev type, int chid)
{
	struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring AD9361 phy channel %d\n", chid);
	if (!get_phy_chan(ctx, type, chid, &chn)) { return false; }
	//wr_ch_str(chn, "rf_port_select",     cfg->rfport);
	wr_ch_lli(chn, "rf_bandwidth",       cfg->bw_hz);
    if(type) //tx
    {
        wr_ch_lli(chn, "hardwaregain",       cfg->tx_gain);
    }
    
 //	wr_ch_lli(chn, "sampling_frequency", cfg->fs_hz);

	// Configure LO channel
	printf("* Acquiring AD9361 %s lo channel\n", type == TX ? "TX" : "RX");
	if (!get_lo_chan(ctx, type, &chn)) { return false; }
	wr_ch_lli(chn, "frequency", cfg->lo_hz);
	return true;
}

#endif // _IIO_IIO_CONFIG_HPP_
